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

		if (true == g_database.DB_Login(m_login_id, m_login_pw)) {
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

	// 기본 셋팅 초기화 정보 보내기

	m_player_data.id = m_id;
	m_player_data.pos.x = 160;
	m_player_data.pos.y = 400;
	m_player_data.dir = 0;
	m_player_data.state.maxhp = 100;
	m_player_data.state.mp = 10;
	m_player_data.state.level = 1;
	m_player_data.state.exp = 0;
	m_player_data.state.maxgauge = 400;
	m_player_data.state.gauge = 0;
	m_player_data.state.hp = m_player_data.state.maxhp;
	m_player_data.is_ai = false;

	m_sub_status.critical = 20;	// const
	m_sub_status.def = 1 + 10;
	m_sub_status.str = 5 + 10;
	m_sub_status.agi = 2 + 10;
	m_sub_status.intel = 1 + 10;
	m_sub_status.health = 3 + 10;

	if (0 == wcscmp(L"guest", m_login_id)) {
		// guest 입장이라면, 초기화를 여기에서 진행한다.
		m_player_data.id = m_id;
		m_player_data.pos.x = 160;
		m_player_data.pos.y = 400;
		m_player_data.dir = 0;
		m_player_data.state.maxhp = 100;
		m_player_data.state.mp = 10;
		m_player_data.state.level = 1;
		m_player_data.state.exp = 0;
		m_player_data.state.maxgauge = 400;
		m_player_data.state.gauge = 0;
		m_player_data.state.hp = m_player_data.state.maxhp;
		m_player_data.is_ai = false;

		m_sub_status.critical = 20;	// const
		m_sub_status.def = 1 + 10;
		m_sub_status.str = 5 + 10;
		m_sub_status.agi = 2 + 10;
		m_sub_status.intel = 1 + 10;
		m_sub_status.health = 3 + 10;
	}
	
	sc_client_init_info init_player;
	init_player.size = sizeof(sc_client_init_info);
	init_player.type = INIT_CLIENT;
	init_player.player_info = m_player_data;
	g_clients[m_id]->send_packet(reinterpret_cast<Packet*>(&init_player));

	// 초기화 정보 보내기 2 - 얘 정보를 다른 애들한테 보내고, 다른 애들 정보를 얘한테 보내기
	sc_other_init_info my_info_to_other;
	sc_other_init_info other_info_to_me;

	my_info_to_other.playerData = m_player_data;

	for (auto players : g_clients)
	{
		if (DISCONNECTED == players->m_connect_state) { continue; }
		if (m_id == players->get_id()) { continue; }
		if (false == is_in_view_range(players->get_id())) { continue; }

		// view list 에 넣어주기
		vl_add(players->get_id());
		players->vl_add(m_id);
	}

	for (auto id : m_view_list) {
		// 다른 애들 정보를 복사해서 넣고, 얘한테 먼저 보내고...
		other_info_to_me.playerData = *(g_clients[id]->get_player_data());
		send_packet(reinterpret_cast<Packet*>(&other_info_to_me));

		if (true == g_clients[id]->get_player_data()->is_ai) { continue; }	// ai 면 pass
		// 얘 정보를 이제 다른 애들한테 보내면 되는데..
		g_clients[id]->send_packet(reinterpret_cast<Packet*>(&my_info_to_other));
	}

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
				DB 저장도 하장
			*/

			sc_disconnect p;
			p.id = m_id;

			for (auto id : m_view_list) {				
				g_clients[id]->vl_remove(id);

				if (DISCONNECTED == g_clients[id]->m_connect_state) { continue; }
				if (true == g_clients[id]->get_player_data()->is_ai) { continue; }
				g_clients[id]->send_packet(reinterpret_cast<Packet*>(&p));
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

bool player_session::is_in_view_range(unsigned int id) {
	float x = g_clients[id]->m_player_data.pos.x;
	float y = g_clients[id]->m_player_data.pos.y;
	float my_x = m_player_data.pos.x;
	float my_y = m_player_data.pos.y;

	if ((VIEW_RANGE * VIEW_RANGE) >= DISTANCE_TRIANGLE(x, y, my_x, my_y)) { return true; }
	return false;
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

		case CHANGED_POSITION: {
			m_state = mov;
			
			m_player_data.pos = *(reinterpret_cast<position*>(&buf[2]));
			
			sc_move p;
			p.id = m_id;
			p.pos = m_player_data.pos;

			// 필요한 애들한테 이동 정보를 뿌려주자
			for (auto players : g_clients)
			{
				if (DISCONNECTED == players->m_connect_state) { continue; }
				if (m_id == players->get_id()) { continue; }
				if (false == is_in_view_range(players->get_id())) {
					if (true == vl_find(players->get_id())) {
						players->vl_remove(m_id);
						vl_remove(players->get_id());

						sc_disconnect send_to_me;
						send_to_me.id = players->get_id();
						send_packet(reinterpret_cast<Packet*>(&send_to_me));

						if (true == players->get_player_data()->is_ai) { continue; }
						sc_disconnect send_to_other;
						send_to_other.id = m_id;
						players->send_packet(reinterpret_cast<Packet*>(&send_to_other));
					}
					continue;
				}

				// view list 에 있으면 skip
				if (true == vl_find(players->get_id())) { continue;	}

				vl_add(players->get_id());
				players->vl_add(m_id);

				sc_other_init_info other_player_info_to_me;
				other_player_info_to_me.playerData = *(players->get_player_data());
				send_packet(reinterpret_cast<Packet*>(&other_player_info_to_me));

				if (true == players->get_player_data()->is_ai) { continue; }
				sc_other_init_info my_info_to_other_player;
				my_info_to_other_player.playerData = m_player_data;
				players->send_packet(reinterpret_cast<Packet*>(&my_info_to_other_player));
			}

			for (auto id : m_view_list) {
				if (true == g_clients[id]->get_player_data()->is_ai) { continue; }

				g_clients[id]->send_packet(reinterpret_cast<Packet*>(&p));
			}
			break;
		}

		case CHANGED_DIRECTION: {
			
			m_player_data.dir = *(reinterpret_cast<char*>(&buf[2]));
			
			sc_dir p;
			p.id = m_id;
			p.dir = m_player_data.dir;

			for (auto id : m_view_list) {
				if (true == g_clients[id]->get_player_data()->is_ai) { continue; }

				g_clients[id]->send_packet(reinterpret_cast<Packet*>(&p));
			}
			break;
		}

		case KEYINPUT_ATTACK:		// 기본 공격 ( 데미지 계산, hit box 범위 조정, 전부 여기서 다 조절해야 한다. )
		{
			// 왼쪽 키 = 우측 + 아래
			// 우측 키 = 왼쪽 + 위
			// 위 키 = 우측 + 위
			// 아래 키 = 왼쪽 + 아래

			// 충돌체크 검사하고 난 뒤에..
			float att_x = 0.5, att_y = 0.5;		// 테스트용 클라 공격 리치가 요정도
			float my_x = m_player_data.pos.x, my_y = m_player_data.pos.y;
			float player_size = 0.7;	// 객체 충돌 크기 반지름
			char *dir = &m_player_data.dir;
			bool is_gauge_on = false;

			if ((*dir & KEYINPUT_RIGHT) == (KEYINPUT_RIGHT)) { my_x -= att_x; my_y -= att_y; }
			if ((*dir & KEYINPUT_LEFT) == (KEYINPUT_LEFT)) { my_x += att_x; my_y += att_y; }
			if ((*dir & KEYINPUT_UP) == (KEYINPUT_UP)) { my_x += att_x; my_y -= att_y; }
			if ((*dir & KEYINPUT_DOWN) == (KEYINPUT_DOWN)) { my_x -= att_x; my_y += att_y; }

			for (auto id : m_view_list) {
				if (DISCONNECTED == g_clients[id]->m_connect_state) { continue; }
				//if (m_id == g_clients[id]->m_id) { continue; }
				//if (true == players->get_player_data()->is_ai) { continue; }

				float x = g_clients[id]->m_player_data.pos.x;
				float y = g_clients[id]->m_player_data.pos.y;
				if((player_size * player_size) >= DISTANCE_TRIANGLE(x, y, my_x, my_y)) {
					m_state = att;
					g_clients[id]->m_player_data.state.hp -= m_sub_status.str;
					is_gauge_on = true; // 발열 게이지를 마지막 체크 때 올려주자

					if (false == g_clients[id]->get_hp_adding()) {
						g_clients[id]->set_hp_adding(true);
						g_time_queue.add_event(id, 1, HP_ADD, false);	// AI 타격 일때, 따로 hp 추가해 주는 함수가 없다 !!! -> 일반 플레이어와 동일하게 처리함
					}

					// 맞은 놈이 ai 면, 반격을 하자.
					if (MAX_AI_NUM > id) {
						if (att != g_clients[id]->m_state) {
							g_clients[id]->m_target_id = m_id;
							g_time_queue.add_event(id, 1, AI_STATE_ATT, true);
						}
						g_clients[id]->m_state = att;
					}
					
					sc_atk p;
					p.attacking_id = m_id;		// 공격자 id
					p.under_attack_id = id;		// 맞는 놈의 id
					p.hp = g_clients[id]->m_player_data.state.hp;	// 맞은 놈의 hp

					// hp 가 0 이 되면 사망처리를 한다. -> 각각의 클라이언트에서 hp 가 0 된 녀석을 지워줌
					if (0 >= g_clients[id]->m_player_data.state.hp) {
						
						// 맞은 애가 ai 면 그냥 연결 끊어서 죽이기
						if (MAX_AI_NUM > g_clients[id]->get_id()) {
							g_clients[id]->m_connect_state = DISCONNECTED;

							sc_disconnect dis_p;
							dis_p.id = id;

							for (auto v_id : g_clients[id]->m_view_list) {
								if (true == g_clients[v_id]->get_player_data()->is_ai) { continue; }
								g_clients[v_id]->send_packet(reinterpret_cast<Packet*>(&dis_p));
							}
							g_clients[id]->m_view_list.clear();
							g_time_queue.add_event(g_clients[id]->m_player_data.id, 10, DEAD_TO_ALIVE, true);	// bot 리젠
							
							// 잡은 녀석에는 경험치도 주도록 하자.
						}
						else {
							// 죽은 애가 player 일 경우..

						}
					}

					send_packet(reinterpret_cast<Packet*>(&p));
					if (true == g_clients[id]->get_player_data()->is_ai) { continue; }
					g_clients[id]->send_packet(reinterpret_cast<Packet*>(&p));
				}
			}

			if (true == m_player_data.is_ai) { break; }
			// 발열 게이지가 올라가야 한다면 ---- ( AI 는 필요 없으니 skip )
			if (true == is_gauge_on) {
				// 발열 게이지 값을 올리고
				m_player_data.state.gauge += 10;
				if (m_player_data.state.maxgauge < m_player_data.state.gauge) { m_player_data.state.gauge = m_player_data.state.maxgauge; }

				/// 공격을 안한지 3초 부터 게이지가 감소하도록 한다.
				g_time_queue.add_event(m_id, 2, CHANGE_PLAYER_STATE, false);

				// 패킷을 당사자에게 하나 보내주자.
				sc_fever p;
				p.gauge = m_player_data.state.gauge;
				send_packet(reinterpret_cast<Packet*>(&p));
			}
			break;
		}
		default:
			// 잘 안날아 오는 패킷
			switch (buf[1])
			{
			case TEST:
				// 받은 패킷을 그대로 돌려준다.
				cout << "Client No. [ " << m_id << " ] TEST Packet Recived !!\n";
				printf("buf[0] = %d, buf[1] = %d, buf[2] = %d\n\n", buf[0], buf[1], buf[2]);
				send_packet(buf);
				break;

			default:
				break;
			}

			// default end
			break;
		}
	}
}
