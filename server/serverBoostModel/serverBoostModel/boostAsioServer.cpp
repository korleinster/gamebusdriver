// 참고할만한 boost asio http://neive.tistory.com/22

#pragma once
#include"stdafx.h"

boostAsioServer::boostAsioServer() : m_acceptor(g_io_service, tcp::endpoint(tcp::v4(), SERVERPORT)), m_socket(g_io_service)
{
	getMyServerIP();
	CheckThisCPUcoreCount();

	acceptThread();
	start_io_service();
}

boostAsioServer::~boostAsioServer()
{
	// make_shared 를 쓸땐, 삭제할 필요가 없지만, 멀티쓰레드 충돌을 막기 위해 현재 일반 vector 를 사용함.. ( 추후 나만의 자료구조를 만들어야 한다 )
	for (auto ptr : g_clients) { delete ptr; }
}

void boostAsioServer::getMyServerIP()
{
	tcp::resolver			m_resolver(g_io_service);
	tcp::resolver::query	m_query(boost::asio::ip::host_name(), "");
	tcp::resolver::iterator m_resolver_iterator = m_resolver.resolve(m_query);

	while (m_resolver_iterator != tcp::resolver::iterator()) {
		using boost::asio::ip::address;
		address addr = (m_resolver_iterator++)->endpoint().address();
		if (!addr.is_v6()) { cout << "This Server's IPv4 address: " << addr.to_string() << endl; }
		//else if (addr.is_v6()) { cout << "This Server's IPv6 address: " << addr.to_string() << endl; }
	}
}

void boostAsioServer::CheckThisCPUcoreCount()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	m_cpuCore = static_cast<int>(si.dwNumberOfProcessors) * 2;
	printf("CPU Core Count = %d, threads = %d\n", m_cpuCore / 2, m_cpuCore);
}

void boostAsioServer::start_io_service()
{	
	m_worker_threads.reserve(m_cpuCore);
	g_clients.reserve(1000);

	// DB 서버를 위해 -1
	// Timer Thread 를 위해 -1
	for (int i = 0; i < m_cpuCore - 1; ++i) { m_worker_threads.emplace_back(new thread{ [&]() -> void { g_io_service.run(); } }); }
	m_worker_threads.emplace_back(new thread{ [&]() -> void { time_queue.TimerThread(); } });
	
	// DB SQL 서버에 접속
	database.Init();

	while (m_ServerShutdown) { Sleep(1000); }
	
	// workerThread 발동
	for (auto thread : m_worker_threads) {
		thread->join();
		delete thread;
	}
}

void boostAsioServer::acceptThread()
{
	m_acceptor.async_accept(m_socket, [&](boost::system::error_code error_code) {
		if (true == (!error_code)) {
			cout << "Client No. [ " << ++m_playerIndex << " ] Connected \t:: IP = " << m_socket.remote_endpoint().address().to_string() << ", Port = " << m_socket.remote_endpoint().port() << "\n";
			g_clients.emplace_back(new player_session (std::move(m_socket), m_playerIndex));
			if (false != g_clients[m_playerIndex]->check_login()) { g_clients[m_playerIndex]->Init(); }
			
		}
		if (false == m_ServerShutdown) { acceptThread(); }		
	});
}


// player_session class ------------------------------------------------------------------------------------------------------------------------

bool player_session::check_login() {

	m_connect_state = true;

	int login_cnt{ 6 };
	while (--login_cnt)
	{
		// id size + id + pw size + pw ( id 문자열 + pw 문자열 + 2 )
		Packet temp_buf[MAX_BUF_SIZE]{ 0 };
		m_socket.receive(boost::asio::buffer(temp_buf, MAX_BUF_SIZE));
		
		wcscpy(m_login_id, reinterpret_cast<wchar_t*>(&temp_buf[1]));
		wcscpy(m_login_pw, reinterpret_cast<wchar_t*>(&temp_buf[temp_buf[0] + 4]));

		// 그냥 guest 라면, DB 접속 없이 그냥 ok...
		if ((0 == wcscmp(L"guest", m_login_id)) && (0 == wcscmp(L"guest", m_login_pw))) {
			temp_buf[0] = 1;
			m_socket.send(boost::asio::buffer(temp_buf, MAX_BUF_SIZE));
			return true;
		}

		if (true == database.DB_Login(m_login_id, m_login_pw)) {
			// 로그인 성공 시 여기서 플레이어 데이터 불러와서 입력

			/// ( 성공했다고 클라한테 메세지 전송 )
			temp_buf[0] = 1;
			m_socket.send(boost::asio::buffer(temp_buf, MAX_BUF_SIZE));
			return true;
		}
		else {
			// 로그인 실패 시, 여기서 처리
			/// ( 실패했다고 클라한테 메세지 전송 )
			temp_buf[0] = 0;
			m_socket.send(boost::asio::buffer(temp_buf, MAX_BUF_SIZE));
		}
	}

	cout << "Client No. [ " << m_id << " ] Disonnected \t:: IP = " << m_socket.remote_endpoint().address().to_string() << ", Port = " << m_socket.remote_endpoint().port() << "\n";
	cout << "Reason = Login Failed\n";
	m_socket.shutdown(m_socket.shutdown_both);
	m_socket.close();
	m_connect_state = false;
	return false;
}

void player_session::Init()
{
	m_connect_state = true;

	// 기본 셋팅 초기화 정보 보내기 *****************>>>> player_data 초기화에 대한 기본 필요 정보 수정시, 여기서 해야함.
	Packet init_this_player_buf[MAX_BUF_SIZE];

	init_this_player_buf[0] = sizeof(player_data) + 2;
	init_this_player_buf[1] = INIT_CLIENT;

	if (0 == wcscmp(L"guest", m_login_id)) {
		m_player_data.id = m_id;
		m_player_data.pos.x = 100;
		m_player_data.pos.y = 100;
		m_player_data.dir = 0;
		m_player_data.state.hp = 100;
	}
	m_player_data.id = m_id;
	m_player_data.pos.x = 100;
	m_player_data.pos.y = 100;
	m_player_data.dir = 0;
	m_player_data.state.hp = 100;

	*(reinterpret_cast<player_data*>(&init_this_player_buf[2])) = m_player_data;
	g_clients[m_id]->send_packet(init_this_player_buf);

	// 자동 hp 회복 하기 용도 타이머 ********************************* ( test 중 )
	//m_is_hp_can_add = true;
	//boost::asio::deadline_timer timer_for_wait(g_io_service, boost::posix_time::seconds(5));
	//// async_wait
	//timer_for_wait.async_wait([&](const boost::system::error_code& error) {
	//	if (130 > get_player_data()->state.hp) {
	//		cout << "Player No. [ " << m_id << " ] HP + 5\n";
	//		get_player_data()->state.hp += 5;

	//		//Packet temp_hp_buf[MAX_BUF_SIZE]{ 0 };
	//		//temp_hp_buf[0] = sizeof(int) + sizeof(UINT) + 2;	// hp + id + packet size addition(2)
	//		//temp_hp_buf[1] = KEYINPUT_ATTACK;
	//		//*(reinterpret_cast<int*>(&temp_hp_buf[2])) = players->m_player_data.state.hp;
	//		//*(reinterpret_cast<int*>(&temp_hp_buf[6])) = players->m_id;

	//		//for (auto players : g_clients)
	//		//{

	//		//}
	//	}		
	//});


	// 초기화 정보 보내기 2 - 얘 정보를 다른 애들한테 보내고, 다른 애들 정보를 얘한테 보내기  *****************>>>> player_data 에서 추가되는 내용을 전송 시, 수정해주어야 한다.
	Packet other_info_to_me_buf[MAX_BUF_SIZE];
	Packet my_info_to_other_buf[MAX_BUF_SIZE];

	other_info_to_me_buf[0] = my_info_to_other_buf[0] = sizeof(player_data) + 2;
	other_info_to_me_buf[1] = my_info_to_other_buf[1] = INIT_OTHER_CLIENT;

	// 현재 접속한 애한테 다른 플레이어 정보 보내기
	*(reinterpret_cast<player_data*>(&my_info_to_other_buf[2])) = m_player_data;

	for (auto players : g_clients)
	{
		if (DISCONNECTED == players->get_current_connect_state()) { continue; }
		if (m_id == players->get_id()) { continue; }

		// 다른 애들 정보를 복사해서 넣고, 얘한테 먼저 보내고...
		*(reinterpret_cast<player_data*>(&other_info_to_me_buf[2])) = *(players->get_player_data());
		send_packet(other_info_to_me_buf);

		// 얘 정보를 이제 다른 애들한테 보내면 되는데..
		players->send_packet(my_info_to_other_buf);
	}
	
	/*
		근처 플레이어에게, 현재 플레이어의 입장을 알리며
		view list 같은 곳에서도 추가하자 ~ !!
	*/

	m_recv_packet();
}

void player_session::m_recv_packet()
{
	//auto self(shared_from_this());
	m_socket.async_read_some(boost::asio::buffer(m_recv_buf, MAX_BUF_SIZE), [&](boost::system::error_code error_code, std::size_t length) -> void {
		if (error_code) {
			if (error_code.value() == boost::asio::error::operation_aborted) { return; }
			// client was disconnected
			if (false == g_clients[m_id]->get_current_connect_state()) { return; }

			cout << "Client No. [ " << m_id << " ] Disonnected \t:: IP = " << m_socket.remote_endpoint().address().to_string() << ", Port = " << m_socket.remote_endpoint().port() << "\n";
			m_socket.shutdown(m_socket.shutdown_both);
			m_socket.close();

			m_connect_state = false;

			/*
				근처 플레이어에게, 현재 플레이어의 퇴장을 알리며
				view list 같은 곳에서도 빼주자 ~ !!
			*/

			Packet temp_buf[MAX_BUF_SIZE] = { sizeof(player_data) + 2, PLAYER_DISCONNECTED };
			memcpy(&temp_buf[2], &m_player_data, temp_buf[0]);

			for (auto players : g_clients)
			{
				if (DISCONNECTED == players->m_connect_state) { continue; }
				if (m_id == players->m_id) { continue; }

				players->send_packet(temp_buf);
			}

			return;
		}

		int current_data_processing = static_cast<int>(length);
		Packet *buf = m_recv_buf;
		while (0 < current_data_processing) {
			if (0 == m_packet_size_current) {
				m_packet_size_current = buf[0];
				if (buf[0] > MAX_BUF_SIZE) {
					cout << "player_session::m_recv_packet() Error, Client No. [ " << m_id << " ] recv buf[0] is out of MAX_BUF_SIZE\n";
					exit(-1);
				}
			}
			int need_to_build = m_packet_size_current - m_packet_size_previous;
			if (need_to_build <= current_data_processing) {
				// Packet building Complete & Process Packet
				memcpy(m_data_buf + m_packet_size_previous, buf, need_to_build);

				m_process_packet(m_data_buf);

				m_packet_size_current = 0;
				m_packet_size_previous = 0;
				current_data_processing -= need_to_build;
				buf += need_to_build;
			}
			else {
				// Packet build continue
				memcpy(m_data_buf + m_packet_size_previous, buf, current_data_processing);
				m_packet_size_previous += current_data_processing;
				current_data_processing = 0;
				buf += current_data_processing;
			}
		}
		m_recv_packet();
	});
}

void player_session::send_packet(Packet *packet)
{
	int packet_size = packet[0];
	Packet *sendBuf = new Packet[packet_size];
	memcpy(sendBuf, packet, packet_size);

#if 0
	cout << "Server Sended Packet to No.[ " << m_id << " ] player for ";
	switch (sendBuf[1])
	{
	case INIT_CLIENT:
		cout << " INIT_CLIENT\n";
		break;
	case PLAYER_DISCONNECTED:
		cout << " PLAYER_DISCONNECTED\n";
		break;
	case CHANGED_POSITION:
		cout << " CHANGED_POSITION\n";
		break;
	default:
		cout << " [ you didn't set protocol to debug ]\n";
		break;
	}
#endif // _DEBUG


	//auto self(shared_from_this());
	m_socket.async_write_some(boost::asio::buffer(sendBuf, packet_size), [=](boost::system::error_code error_code, std::size_t bytes_transferred) -> void {
		if (!error_code) {
			if (packet_size != bytes_transferred) { cout << "Client No. [ " << m_id << " ] async_write_some packet bytes was NOT SAME !!\n"; }
			delete[] sendBuf;
		}
	});
}

void player_session::m_process_packet(Packet buf[])
{
	// packet[0] = packet size		> 0번째 자리에는 무조건, 패킷의 크기가 들어가야만 한다.
	// packet[1] = type				> 1번째 자리에는 현재 패킷이 무슨 패킷인지 속성을 정해주는 값이다.
	// packet[...] = data			> 2번째 부터는 속성에 맞는 순대로 처리를 해준다.

	// buf[1] 번째의 속성으로 분류를 한 뒤에, 내부에서 2번째 부터 데이터를 처리하기 시작한다.

	{
		switch (buf[1])
		{
		case TEST:
			// 받은 패킷을 그대로 돌려준다.
			cout << "Client No. [ " << m_id << " ] TEST Packet Recived !!\n";
			printf("buf[0] = %d, buf[1] = %d, buf[2] = %d\n\n", buf[0], buf[1], buf[2]);
			send_packet(buf);
			break;

		case CHANGED_POSITION: {

			// [ 0] = size
			// [ 1] = CHANGED_POSITION type
			// [ 2] = size of position
			// [10] = user id

			m_player_data.pos = *(reinterpret_cast<position*>(&buf[2]));

			Packet temp_pos_buf[MAX_BUF_SIZE]{ 0 };
			temp_pos_buf[0] = sizeof(position) + sizeof(unsigned int) + 2;
			temp_pos_buf[1] = CHANGED_POSITION;
			*(reinterpret_cast<position*>(&temp_pos_buf[2])) = m_player_data.pos;
			*(reinterpret_cast<unsigned int*>(&temp_pos_buf[sizeof(position) + 2])) = m_id;

			// 필요한 애들한테 이동 정보를 뿌려주자 - 현재는 애들 다 뿌린다.
			for (auto players : g_clients)
			{
				if (DISCONNECTED == players->m_connect_state) { continue; }
				if (m_id == players->m_id) { continue; }

				players->send_packet(temp_pos_buf);
			}
		}
			break;

		case CHANGED_DIRECTION: {

			// [0] = size
			// [1] = CHANGED_DIRECTION type
			// [2] = size of direction
			// [3] = user id

			m_player_data.dir = *(reinterpret_cast<char*>(&buf[2]));

			Packet temp_direction_buf[MAX_BUF_SIZE]{ 0 };
			temp_direction_buf[0] = sizeof(char) + sizeof(unsigned int) + 2;
			temp_direction_buf[1] = CHANGED_DIRECTION;
			*(reinterpret_cast<char*>(&temp_direction_buf[2])) = m_player_data.dir;
			*(reinterpret_cast<unsigned int*>(&temp_direction_buf[sizeof(char) + 2])) = m_id;

			// 필요한 애들한테 방향 정보를 뿌려주자 - 현재는 애들 다 뿌린다.
			for (auto players : g_clients)
			{
				if (DISCONNECTED == players->m_connect_state) { continue; }
				if (m_id == players->m_id) { continue; }

				players->send_packet(temp_direction_buf);
			}
		}
			break;
		
		case KEYINPUT_ATTACK:
		{
			// 충돌체크 검사하고 난 뒤에.. ( 현재는 임시 충돌 체크, 실제 클라와 연동시 충돌 범위 체크해야 한다. )
			int att_x = 15, att_y = 15;		// 테스트용 클라 공격 리치가 요정도
			int x = m_player_data.pos.x, y = m_player_data.pos.y;
			int player_size = 5;	// 테스트용 클라 원 반지름이 크기 5...
			char *dir = &m_player_data.dir;

			if ((*dir & KEYINPUT_RIGHT) == (KEYINPUT_RIGHT))	{ x += att_x; }
			if ((*dir & KEYINPUT_LEFT) == (KEYINPUT_LEFT))		{ x -= att_x; }
			if ((*dir & KEYINPUT_UP) == (KEYINPUT_UP))			{ y -= att_y; }
			if ((*dir & KEYINPUT_DOWN) == (KEYINPUT_DOWN))		{ y += att_y; }

			for (auto players : g_clients) {
				if (DISCONNECTED == players->m_connect_state) { continue; }
				if (m_id == players->m_id) { continue; }

				int tempx = x - players->m_player_data.pos.x;
				int tempy = y - players->m_player_data.pos.y;
				if (((tempx * tempx) + (tempy * tempy)) <= (player_size * player_size)) {
					players->m_player_data.state.hp -= 10;

					Packet temp_hp_buf[MAX_BUF_SIZE]{ 0 };
					temp_hp_buf[0] = sizeof(int) + sizeof(UINT) + 2;	// hp + id + packet size addition(2)
					temp_hp_buf[1] = KEYINPUT_ATTACK;
					*(reinterpret_cast<int*>(&temp_hp_buf[2])) = players->m_player_data.state.hp;
					*(reinterpret_cast<int*>(&temp_hp_buf[6])) = players->m_id;

					for (auto other_players : g_clients) {
						if (DISCONNECTED == other_players->m_connect_state) { continue; }
						//if (players->m_id == other_players->m_id) { continue; }	// 자기 hp 가 깎였을 경우, 자기 한테도 보내야 한다...

						other_players->send_packet(temp_hp_buf);
					}
				}
			}

		}
			break;

		/*case PASSIVE_HP_ADDED:
		{
			cout << "Player No. [ " << m_id << " ] HP + 5\n";

			boost::asio::deadline_timer timer_for_wait(g_io_service);
			timer_for_wait.expires_from_now(boost::posix_time::seconds(1));
			timer_for_wait.async_wait([&](const boost::system::error_code& error) {
				g_io_service.post([]() {
					Packet hp_added[MAX_BUF_SIZE]{ 0 };
					hp_added[0] = 2;
					hp_added[1] = PASSIVE_HP_ADDED;
				});
			});
		}
			break;*/
		default:
			break;
		}
	}
}

void player_session::m_passive_hp_adding() {

	g_io_service.post([&]() {
		cout << "Player No. [ " << m_id << " ] HP + 5\n";

		if (130 > get_player_data()->state.hp) {
			get_player_data()->state.hp += 5;

			//Packet temp_hp_buf[MAX_BUF_SIZE]{ 0 };
			//temp_hp_buf[0] = sizeof(int) + sizeof(UINT) + 2;	// hp + id + packet size addition(2)
			//temp_hp_buf[1] = KEYINPUT_ATTACK;
			//*(reinterpret_cast<int*>(&temp_hp_buf[2])) = players->m_player_data.state.hp;
			//*(reinterpret_cast<int*>(&temp_hp_buf[6])) = players->m_id;

			//for (auto players : g_clients)
			//{

			//}
		}
		else { m_is_hp_can_add = false; }

		if (m_is_hp_can_add) { m_passive_hp_adding(); }
	});

}


// DB class ---------------------------------------------------------------------------------------------------------------

void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode)
{
	SQLSMALLINT iRec = 0;
	SQLINTEGER  iError;
	WCHAR       wszMessage[1000];
	WCHAR       wszState[SQL_SQLSTATE_SIZE + 1];

	if (RetCode == SQL_INVALID_HANDLE)
	{
		fwprintf(stderr, L"Invalid handle!\n");
		return;
	}

	while (SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage, (SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)), (SQLSMALLINT *)NULL) == SQL_SUCCESS)
	{
		// Hide data truncated..
		if (wcsncmp(wszState, L"01004", 5)) { fwprintf(stderr, L"[%5.5s] %s (%d)\n", wszState, wszMessage, iError); }
	}

}

void DB::Init() {
	// Allocate an environment

	if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv) == SQL_ERROR) {
		fwprintf(stderr, L"DBacess class :: Unable to allocate an environment handle\n");
		exit(-1);
	}

	TRYODBC(hEnv, SQL_HANDLE_ENV, SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0));
	TRYODBC(hEnv, SQL_HANDLE_ENV, SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc));
	TRYODBC(hDbc, SQL_HANDLE_DBC, SQLDriverConnect(hDbc, GetDesktopWindow(), SQLSERVERADDR, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE));
	TRYODBC(hDbc, SQL_HANDLE_DBC, SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt));

	fwprintf(stderr, L"Azure SQL Database Server Connected!\n");
	// Loop to get input and execute queries -- 연결은 여기까지 완료 되었고, 여기서 부터 명령어를 보내면 실행하게 된다.
}

void DB::Release() {
	// Free ODBC handles and exit

	if (hStmt) { SQLFreeHandle(SQL_HANDLE_STMT, hStmt); }

	if (hDbc) {
		SQLDisconnect(hDbc);
		SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	}

	if (hEnv) { SQLFreeHandle(SQL_HANDLE_ENV, hEnv); }

	wprintf(L"\nAzure SQL DataBase Server Disconnected.\n");
}

bool DB::DB_Login(wchar_t* id, wchar_t* pw) {
	wchar_t input_id[MAX_BUF_SIZE / 4]{ 0 };
	wchar_t input_pw[MAX_BUF_SIZE / 4]{ 0 };
	wchar_t Nickname[MAX_BUF_SIZE / 4]{ 0 };
	int PlayerLevel{ 0 };
	bool Admin{ false };

	SQLINTEGER pIndicators[5];		// DB 상 안에 있는 목차 갯 수

	if (SQLExecDirect(hStmt, (SQLWCHAR*)L"SELECT RTRIM(ID), RTRIM(Password), RTRIM(Nickname), RTRIM(PlayerLevel), RTRIM(Admin) FROM dbo.user_data", SQL_NTS) != SQL_ERROR) {
		// 숫자 1 은, 첫번째 파라미터 ID
		// 뒤에 배열은 몇 바이트 받았는지 담음
		SQLBindCol(hStmt, 1, SQL_C_WCHAR, (SQLPOINTER)&input_id, MAX_BUF_SIZE / 4, &pIndicators[0]);
		SQLBindCol(hStmt, 2, SQL_C_WCHAR, (SQLPOINTER)&input_pw, MAX_BUF_SIZE / 4, &pIndicators[1]);
		SQLBindCol(hStmt, 3, SQL_C_WCHAR, (SQLPOINTER)&Nickname, MAX_BUF_SIZE / 4, &pIndicators[2]);
		SQLBindCol(hStmt, 4, SQL_C_LONG, (SQLPOINTER)&PlayerLevel, sizeof(PlayerLevel), &pIndicators[3]);
		SQLBindCol(hStmt, 5, SQL_C_BIT, (SQLPOINTER)&Admin, sizeof(Admin), &pIndicators[4]);

		while (SQLFetch(hStmt) == SQL_SUCCESS)
		{
			if ((wcscmp(input_id, id) == 0) && (wcscmp(input_pw, pw) == 0)) {

				// 확인이 되었다면, 여기서 데이터를 복사해야 하는데..

				return true;
			}
		}
	}

	return false;
}

void DB::SQLcmd(SQLWCHAR* str) {

	RETCODE     RetCode;
	SQLSMALLINT sNumResults;

	RetCode = SQLExecDirect(hStmt, str, SQL_NTS);

	switch (RetCode)
	{
	case SQL_SUCCESS_WITH_INFO:	// 뭔가 실패는 했는데, 일단 계속 실행 할 수 있을 경우...
	{
		HandleDiagnosticRecord(hStmt, SQL_HANDLE_STMT, RetCode);
	}
	case SQL_SUCCESS:
	{
		// 성공했을 시... 코드 실행
		// SQLNumResultCols() 함수는 결과 세트의 해당 열 수를, 변수에 리턴 받는다. ( 열 = -> )
		TRYODBC(hStmt, SQL_HANDLE_STMT, SQLNumResultCols(hStmt, &sNumResults));

		// 결과가 0줄 이상이면 일을 처리한다. 꿍야꿍야...
		if (sNumResults > 0) {

		}
		else
		{
			SQLINTEGER cRowCount;

			// 반대로 SQLRowCount() 함수는 행 수를, 변수에 리턴 받는다. ( 행 = V )
			TRYODBC(hStmt, SQL_HANDLE_STMT, SQLRowCount(hStmt, reinterpret_cast<SQLLEN *>(&cRowCount)));

			// 행이 0 이상이면 데이터를 출력하는 형태...
			if (cRowCount >= 0) {
				wprintf(L"%Id %s affected\n", cRowCount, cRowCount == 1 ? L"row" : L"rows");
			}
		}
		break;
	}
	case SQL_ERROR: // 실패 했을 경우, 아래 함수를 통해서 에러 결과를 화면에 출력해준다.
	{
		HandleDiagnosticRecord(hStmt, SQL_HANDLE_STMT, RetCode);
	}
	default:
		fwprintf(stderr, L"Unexpected return code %hd!\n", RetCode);
	}


}


// Timer class ---------------------------------------------------------------------------------------------------------------

void TimerQueue::TimerThread() {
	while (true) {
		Sleep(1);
		//timer_lock.lock();
		while (false == timer_queue.empty()) {
			if (timer_queue.top().wakeup_time > GetTickCount()) break;
			event_type ev = timer_queue.top();
			timer_queue.pop();
			//timer_lock.unlock();
			//Overlap_ex *over = new Overlap_ex;
			//over->operation = ev.event_id;
			//PostQueuedCompletionStatus(g_hIocp, 1, ev.obj_id, &(over->original_overlap));
			//timer_lock.lock();
		}
		//timer_lock.unlock();
	}
}
