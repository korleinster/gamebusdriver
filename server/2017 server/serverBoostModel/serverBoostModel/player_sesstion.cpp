#include "stdafx.h"

bool player_session::check_login() {

	m_connect_state = true;

	int login_cnt{ 2 };
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

	m_player_data.id = m_id;
	m_player_data.pos.x = 150;
	m_player_data.pos.y = 400;
	m_player_data.dir = 0;
	m_player_data.state.maxhp = 100;
	m_player_data.state.mp = 10;
	m_player_data.state.level = 1;
	m_player_data.state.exp = 0;
	m_player_data.state.gauge = 0;
	m_player_data.state.hp = m_player_data.state.maxhp;
	m_player_data.is_ai = false;

	m_sub_status.critical = 20;	// const
	m_sub_status.def = 1;
	m_sub_status.str = 5;
	m_sub_status.agi = 2;
	m_sub_status.intel = 1;
	m_sub_status.health = 3;

	if (0 == wcscmp(L"guest", m_login_id)) {
		// guest 입장이라면, 초기화를 여기에서 진행한다.
		m_player_data.id = m_id;
		m_player_data.pos.x = 150;
		m_player_data.pos.y = 400;
		m_player_data.dir = 0;
		m_player_data.state.maxhp = 100;
		m_player_data.state.mp = 10;
		m_player_data.state.level = 1;
		m_player_data.state.exp = 0;
		m_player_data.state.gauge = 0;
		m_player_data.state.hp = m_player_data.state.maxhp;
		m_player_data.is_ai = false;

		m_sub_status.critical = 20;	// const
		m_sub_status.def = 1;
		m_sub_status.str = 5;
		m_sub_status.agi = 2;
		m_sub_status.intel = 1;
		m_sub_status.health = 3;
	}
	
	sc_client_init_info init_player;
	init_player.size = sizeof(sc_client_init_info);
	init_player.type = INIT_CLIENT;
	init_player.player_info = m_player_data;
	g_clients[m_id]->send_packet(reinterpret_cast<Packet*>(&init_player));

	// 초기화 정보 보내기 2 - 얘 정보를 다른 애들한테 보내고, 다른 애들 정보를 얘한테 보내기  *****************>>>> player_data 에서 추가되는 내용을 전송 시, 수정해주어야 한다.
	sc_other_init_info my_info_to_other;
	sc_other_init_info other_info_to_me;

	my_info_to_other.playerData = m_player_data;

	for (auto players : g_clients)
	{
		if (DISCONNECTED == players->get_current_connect_state()) { continue; }
		if (m_id == players->get_id()) { continue; }

		// 다른 애들 정보를 복사해서 넣고, 얘한테 먼저 보내고...
		other_info_to_me.playerData = *(players->get_player_data());
		send_packet(reinterpret_cast<Packet*>(&other_info_to_me));

		if (true == players->get_player_data()->is_ai) { continue; }
		// 얘 정보를 이제 다른 애들한테 보내면 되는데..
		players->send_packet(reinterpret_cast<Packet*>(&my_info_to_other));
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

			sc_disconnect p;
			p.id = m_id;

			for (auto players : g_clients)
			{
				if (DISCONNECTED == players->m_connect_state) { continue; }
				if (m_id == players->m_id) { continue; }
				//if (true == players->get_player_data()->is_ai) { continue; }

				players->send_packet(reinterpret_cast<Packet*>(&p));
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
			
			m_player_data.pos = *(reinterpret_cast<position*>(&buf[2]));
			
			sc_move p;
			p.id = m_id;
			p.pos = m_player_data.pos;

			// 필요한 애들한테 이동 정보를 뿌려주자 - 현재는 애들 다 뿌린다.
			for (auto players : g_clients)
			{
				if (DISCONNECTED == players->m_connect_state) { continue; }
				if (m_id == players->m_id) { continue; }
				if (true == players->get_player_data()->is_ai) { continue; }

				players->send_packet(reinterpret_cast<Packet*>(&p));
			}
		}
			break;

		case CHANGED_DIRECTION: {
			
			m_player_data.dir = *(reinterpret_cast<char*>(&buf[2]));
			
			sc_dir p;
			p.id = m_id;
			p.dir = m_player_data.dir;

			// 필요한 애들한테 방향 정보를 뿌려주자 - 현재는 애들 다 뿌린다.
			for (auto players : g_clients)
			{
				if (DISCONNECTED == players->m_connect_state) { continue; }
				if (m_id == players->m_id) { continue; }
				if (true == players->get_player_data()->is_ai) { continue; }

				players->send_packet(reinterpret_cast<Packet*>(&p));
			}
		}
								break;

		case KEYINPUT_ATTACK:		// 기본 공격 ( 데미지 계산, hit box 범위 조정, 전부 여기서 다 조절해야 한다. )
		{
			// 충돌체크 검사하고 난 뒤에.. ( 현재는 임시 충돌 체크, 실제 클라와 연동시 충돌 범위 체크해야 한다. )
			int att_x = 0.1, att_y = 0.1;		// 테스트용 클라 공격 리치가 요정도
			int x = m_player_data.pos.x, y = m_player_data.pos.y;
			int player_size = 1;	// 테스트용 클라 원 반지름이 크기 5...
			char *dir = &m_player_data.dir;

			if ((*dir & KEYINPUT_RIGHT) == (KEYINPUT_RIGHT)) { x += att_x; }
			if ((*dir & KEYINPUT_LEFT) == (KEYINPUT_LEFT)) { x -= att_x; }
			if ((*dir & KEYINPUT_UP) == (KEYINPUT_UP)) { y -= att_y; }
			if ((*dir & KEYINPUT_DOWN) == (KEYINPUT_DOWN)) { y += att_y; }

			for (auto players : g_clients) {
				if (DISCONNECTED == players->m_connect_state) { continue; }
				if (m_id == players->m_id) { continue; }
				//if (true == players->get_player_data()->is_ai) { continue; }

				int tempx = x - players->m_player_data.pos.x;
				int tempy = y - players->m_player_data.pos.y;
				if (((tempx * tempx) + (tempy * tempy)) <= (player_size * player_size)) {
					players->m_player_data.state.hp -= 70;

					if (false == *players->get_hp_adding()) {
						*players->get_hp_adding() = true;
						time_queue.add_event(players->m_player_data.id, 1, HP_ADD, false);	// AI 타격 일때, 따로 hp 추가해 주는 함수가 없다 !!! -> 일반 플레이어와 동일하게 처리함
					}

					//Packet temp_hp_buf[MAX_BUF_SIZE]{ 0 };
					//temp_hp_buf[0] = sizeof(int) + sizeof(UINT) + sizeof(UINT) + 2;	// hp + id + packet size addition(2)
					//temp_hp_buf[1] = KEYINPUT_ATTACK;
					//*(reinterpret_cast<int*>(&temp_hp_buf[2])) = players->m_player_data.state.hp;
					//*(reinterpret_cast<int*>(&temp_hp_buf[6])) = players->m_id;		// 맞는 사람의 id
					//*(reinterpret_cast<int*>(&temp_hp_buf[10])) = m_id;				// 공격한 사람의 id

					sc_atk p;
					p.attacking_id = m_id;					// 공격자 id
					p.under_attack_id = players->m_id;		// 맞는 놈의 id
					p.hp = players->m_player_data.state.hp;	// 맞은 놈의 hp

					// hp 가 0 이 되면 사망처리를 한다. -> 각각의 클라이언트에서 hp 가 0 된 녀석을 지워주자...
					if (0 >= players->m_player_data.state.hp) {
						//*(reinterpret_cast<int*>(&temp_hp_buf[2])) = players->m_player_data.state.hp = 0; // 굳이 0 으로 만들어줄 필요는 없는듯

						// 맞은 애가 ai 면 그냥 연결 끊어서 죽이기
						if (MAX_AI_NUM > players->get_id()) {
							players->m_connect_state = DISCONNECTED;

							// 10 초후 리젠을 하는 타이머 큐에 집어넣는건 어떨까 싶다.
							// 잡은 녀석에는 경험치도 주도록 하자.
						}
					}

					for (auto other_players : g_clients) {
						if (DISCONNECTED == other_players->m_connect_state) { continue; }
						//if (players->m_id == other_players->m_id) { continue; }	// 자기 hp 가 깎였을 경우, 자기 한테도 보내야 한다...
						if (true == other_players->get_player_data()->is_ai) { continue; }

						other_players->send_packet(reinterpret_cast<Packet*>(&p));
					}
				}
			}

		}
		break;
		default:
			break;
		}
	}
}
