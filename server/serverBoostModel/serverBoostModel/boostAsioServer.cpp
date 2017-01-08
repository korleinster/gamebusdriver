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
	// make_shared 를 썼기 때문에, 삭제할 필요가 없다아 ?
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

	for (int i = 0; i < m_cpuCore; ++i) { m_worker_threads.emplace_back(new thread{ [&]() -> void { g_io_service.run(); } }); }
	
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
			g_clients[m_playerIndex]->Init();
		}
		if (false == m_ServerShutdown) { acceptThread(); }		
	});
}


// player_session class ------------------------------------------------------------------------------------------------------------------------

void player_session::Init()
{
	m_connect_state = true;

	// 기본 셋팅 초기화 정보 보내기
	Packet init_this_player_buf[MAX_BUF_SIZE];

	init_this_player_buf[0] = sizeof(player_data) + 2;
	init_this_player_buf[1] = INIT_CLIENT;

	m_player_data.id = m_id;
	m_player_data.pos.x = 400;
	m_player_data.pos.y = 300;

	memcpy(&init_this_player_buf[2], g_clients[m_id]->get_player_data(), init_this_player_buf[0]);
	g_clients[m_id]->send_packet(init_this_player_buf);

	// 초기화 정보 보내기 2 - 얘 정보를 다른 애들한테 보내고, 다른 애들 정보를 얘한테 보내기
	/*Packet other_info_to_me_buf[MAX_BUF_SIZE];*/
	Packet my_info_to_other_buf[MAX_BUF_SIZE];

	/*other_info_to_me_buf[0] =*/ my_info_to_other_buf[0] = sizeof(player_data) + 2;
	/*other_info_to_me_buf[1] =*/ my_info_to_other_buf[1] = KEYINPUT;
	my_info_to_other_buf[1] = INIT_CLIENT;

	// 현재 접속한 애한테 다른 플레이어 정보 보내기
	memcpy(&my_info_to_other_buf[2], &m_player_data, my_info_to_other_buf[0]);

	for (auto players : g_clients)
	{
		if (DISCONNECTED == players->get_current_connect_state()) { continue; }
		if (m_id == players->get_id()) { continue; }

		// 다른 애들 정보를 복사해서 넣고, 얘한테 먼저 보내고... ( 얘 왜 못받는건지 알수가 읎다 도대체... )
		/*memcpy(&other_info_to_me_buf[2], players->get_player_data(), other_info_to_me_buf[0] - 2);
		send_packet(other_info_to_me_buf);*/

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

				m_process_packet(m_id, m_data_buf);

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

	//auto self(shared_from_this());
	m_socket.async_write_some(boost::asio::buffer(sendBuf, packet_size), [=](boost::system::error_code error_code, std::size_t bytes_transferred) -> void {
		if (!error_code) {
			if (packet_size != bytes_transferred) { cout << "Client No. [ " << m_id << " ] async_write_some packet bytes was NOT SAME !!\n"; }
			delete[] sendBuf;
		}
	});
}

void player_session::m_process_packet(const unsigned int& id, Packet buf[])
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
			g_clients[id]->send_packet(buf);
			break;

		case KEYINPUT:
			g_clients[id]->m_player_data.pos = reinterpret_cast<player_data*>(&buf[2])->pos;

			// 필요한 애들한테 이동 정보를 뿌려주자
			for (auto players : g_clients)
			{
				if (DISCONNECTED == players->m_connect_state) { continue; }
				if (id == players->m_id) { continue; }

				players->send_packet(buf);
			}

			break;
		default:
			break;
		}
	}
}