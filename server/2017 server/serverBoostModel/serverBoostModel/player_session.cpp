#include "stdafx.h"

bool player_session::check_login() {

	DB d;

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

		if (true == d.DB_Login(m_login_id, m_login_pw, this)) {
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

// return 값은 공격 해야하는 target id 값이 return 된다.
unsigned int player_session::ai_rand_mov()
{
	char orgin_dir = m_player_data.dir;

	// 무작위 방향 8 개 중 정하기
	int a = rand() % 8;

	m_player_data.dir = 0;
	switch (a)
	{
	case 0: m_player_data.dir = m_player_data.dir | KEYINPUT_RIGHT; break;
	case 1: m_player_data.dir = m_player_data.dir | KEYINPUT_LEFT; break;
	case 2: m_player_data.dir = m_player_data.dir | KEYINPUT_UP; break;
	case 3: m_player_data.dir = m_player_data.dir | KEYINPUT_DOWN; break;
	case 4: m_player_data.dir = m_player_data.dir | (KEYINPUT_RIGHT | KEYINPUT_DOWN); break;
	case 5: m_player_data.dir = m_player_data.dir | (KEYINPUT_LEFT | KEYINPUT_DOWN); break;
	case 6: m_player_data.dir = m_player_data.dir | (KEYINPUT_RIGHT | KEYINPUT_UP); break;
	case 7: m_player_data.dir = m_player_data.dir | (KEYINPUT_LEFT | KEYINPUT_UP); break;
	}
	
	char dir = m_player_data.dir;

	if (orgin_dir != dir) {
		sc_dir pp;
		pp.id = m_id;
		pp.dir = m_player_data.dir;
		send_packet_other_players_in_view_range(reinterpret_cast<Packet*>(&pp), m_id);
	}

	// 해당 방향으로 움직이기 - 못가는 곳 충돌 처리를 해야한다면 여기서 해야한다.
	position *pos = &m_player_data.pos;

	if ((dir & KEYINPUT_RIGHT) == (KEYINPUT_RIGHT)) { pos->x -= ai_mov_speed; pos->y -= ai_mov_speed; }
	if ((dir & KEYINPUT_LEFT) == (KEYINPUT_LEFT)) { pos->x += ai_mov_speed; pos->y += ai_mov_speed; }
	if ((dir & KEYINPUT_UP) == (KEYINPUT_UP)) { pos->x += ai_mov_speed; pos->y -= ai_mov_speed; }
	if ((dir & KEYINPUT_DOWN) == (KEYINPUT_DOWN)) { pos->x -= ai_mov_speed; pos->y += ai_mov_speed; }

	// 근처에 공격해야 할 적이 있다면 해당 id return
	unsigned int target_id = return_nearlest_player(RANGE_CHECK_AI_ATT);
	
	// 내 위치와 방향이 바뀌었다고, 주변 플레이어에게 알리기
	sc_move p;
	p.id = m_id;
	p.pos = m_player_data.pos;

	send_packet_other_players_in_view_range(reinterpret_cast<Packet*>(&p), m_id);

	// user 가 한명이라도 있다면
	if (none != return_nearlest_player(VIEW_RANGE)) {
		// 계속 움직이는건, 타이머 스레드에서 한다
		/*if (true != ai_is_rand_mov) {
			ai_is_rand_mov = true;
			g_time_queue.add_event(m_id, 3, CHANGE_AI_STATE_MOV, true);
		}*/
	}
	else {
		// 주변에 아무도 없다면, 값을 초기화 하자
		sc_disconnect packet;
		packet.id = m_id;
		send_packet_other_players(reinterpret_cast<Packet*>(&packet), m_id);

		m_target_id = none;
		set_state(none);
		ai_is_rand_mov = false;
		return none;
	}

	return target_id;
}

unsigned int player_session::return_nearlest_player(float range)
{
	float dist, min = SQUARED(VIEW_RANGE);
	unsigned int player_id = none;
	for (auto players : g_clients) {
		if (DISCONNECTED == players->get_current_connect_state()) { continue; }
		if (m_id == players->m_id) { continue; }
		if (true == players->m_player_data.is_ai) { continue; }
		dist = DISTANCE_TRIANGLE(m_player_data.pos.x, m_player_data.pos.y, players->m_player_data.pos.x, players->m_player_data.pos.y);
		if (SQUARED(range) >= dist) {
			if (min > dist) {
				player_id = players->m_id;
			}
		}
	}

	return player_id;
}

void player_session::send_packet_other_players(Packet * p, unsigned int except_id)
{
	for (auto players : g_clients) {
		if (DISCONNECTED == players->get_current_connect_state()) { continue; }
		if (except_id == players->m_id) { continue; }
		if (true == players->m_player_data.is_ai) { continue; }
		players->send_packet(p);
	}
}

void player_session::send_packet_other_players_in_view_range(Packet* p, unsigned int mid_id)
{
	for (auto players : g_clients) {
		if (DISCONNECTED == players->get_current_connect_state()) { continue; }
		if (mid_id == players->m_id) { continue; }
		if (true == players->m_player_data.is_ai) { continue; }
		if (SQUARED(VIEW_RANGE) >= DISTANCE_TRIANGLE(g_clients[mid_id]->get_player_data()->pos.x, g_clients[mid_id]->get_player_data()->pos.y, players->m_player_data.pos.x, players->m_player_data.pos.y)) {
			players->send_packet(p);
		}
	}
}

void player_session::Init()
{
	m_connect_state = true;

	// 기본 셋팅 초기화 정보 보내기

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
		m_sub_status.def = 1 + 5;
		m_sub_status.str = 5 + 10;
		m_sub_status.agi = 2 + 10;
		m_sub_status.intel = 1 + 10;
		m_sub_status.health = 3 + 10;
		m_sub_status.quest = 0;
	}
	else {
		m_player_data.id = m_id;
		/*m_player_data.pos.x = 160;
		m_player_data.pos.y = 400;*/
		m_player_data.dir = 0;
		m_player_data.state.maxhp = 100;
		m_player_data.state.mp = 10;
		/*m_player_data.state.level = 1;*/
		m_player_data.state.exp = 0;
		m_player_data.state.maxgauge = 400;
		m_player_data.state.gauge = 0;
		m_player_data.state.hp = m_player_data.state.maxhp;
		m_player_data.is_ai = false;
		//
		m_sub_status.critical = 20;	// const
		m_sub_status.def = 1 + 5;
		m_sub_status.str = 5 + 10;
		m_sub_status.agi = 2 + 10;
		m_sub_status.intel = 1 + 10;
		m_sub_status.health = 3 + 10;
		/*m_sub_status.quest = 0;*/
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

	for (auto id : m_view_list) {
		// 다른 애들 정보를 복사해서 넣고, 얘한테 먼저 보내고...
		other_info_to_me.playerData = *(g_clients[id]->get_player_data());
		send_packet(reinterpret_cast<Packet*>(&other_info_to_me));

		if (true == g_clients[id]->get_player_data()->is_ai) { continue; }	// ai 면 pass
		// 얘 정보를 이제 다른 애들한테 보내면 되는데..
		g_clients[id]->send_packet(reinterpret_cast<Packet*>(&my_info_to_other));
	}

	// 퀘스트 진행상황 보내기
	sc_quest q;
	q.quest = m_sub_status.quest;
	send_packet(reinterpret_cast<Packet*>(&q));

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

			// guest 가 아니면 DB 에 데이터 저장
			if (0 != wcscmp(L"guest", m_login_id)) {
				DB d;
				d.DB_Update(m_login_id, this);
			}

			sc_disconnect p;
			p.id = m_id;

			for (auto id : m_view_list) {				
				g_clients[id]->vl_remove(id);

				if (DISCONNECTED == g_clients[id]->m_connect_state) { continue; }
				if (true == g_clients[id]->get_player_data()->is_ai) { 
					//g_clients[id]->vl_remove(m_id);
					continue;
				}
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

void player_session::refresh_view_list()
{
	for (auto players : g_clients)
	{
		if (DISCONNECTED == players->m_connect_state) { continue; }
		if (m_id == players->get_id()) { continue; }
		if (false == is_in_view_range(players->get_id())) {
			vl_remove(players->get_id());
			if (false == players->m_player_data.is_ai) { players->vl_remove(m_id); }
			continue;
		}

		// view list 에 넣어주기
		vl_add(players->get_id());
		if (false == players->m_player_data.is_ai) { players->vl_add(m_id); }		
	}
}


bool player_session::is_in_view_range(unsigned int id) {
	float x = g_clients[id]->m_player_data.pos.x;
	float y = g_clients[id]->m_player_data.pos.y;
	float my_x = m_player_data.pos.x;
	float my_y = m_player_data.pos.y;

	if (SQUARED(VIEW_RANGE) >= DISTANCE_TRIANGLE(x, y, my_x, my_y)) { return true; }
	return false;
}

bool player_session::is_in_att_range(unsigned int id) {
	float x = g_clients[id]->m_player_data.pos.x;
	float y = g_clients[id]->m_player_data.pos.y;
	float my_x = m_player_data.pos.x;
	float my_y = m_player_data.pos.y;

	if (SQUARED(RANGE_CHECK_AI_ATT) >= DISTANCE_TRIANGLE(x, y, my_x, my_y)) { return true; }
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
			if (dead == m_state) { break; }
			m_state = mov;
			
			m_player_data.pos = *(reinterpret_cast<position*>(&buf[2]));
			
			sc_move p;
			p.id = m_id;
			p.pos = m_player_data.pos;

			// 전체 유저 검색하여 view list 갱신 작업 & 패킷 뿌리기
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

						if (true == players->get_player_data()->is_ai) {
							// player 시야에서 사라진 봇이라면, 상태 및 위치 초기화 필요 ******************************************

							continue;
						}
						sc_disconnect send_to_other;
						send_to_other.id = m_id;
						players->send_packet(reinterpret_cast<Packet*>(&send_to_other));
					}
					continue;
				}

				// view list 에 있으면 skip
				if (true == vl_find(players->get_id())) {
					players->send_packet(reinterpret_cast<Packet*>(&p));
					continue;
				}

				vl_add(players->get_id());

				sc_other_init_info other_player_info_to_me;
				other_player_info_to_me.playerData = players->m_player_data;
				send_packet(reinterpret_cast<Packet*>(&other_player_info_to_me));

				// 상대가 컴퓨터라면 패킷 보낼 필요가 없지
				if (true == players->get_player_data()->is_ai) { continue; }
				players->vl_add(m_id);	// 해당 플레이어가 봇이면, 시야에 넣어줄 필요가 없지. ( 위에서 부터 위치 이동함 )

				sc_other_init_info my_info_to_other_player;
				my_info_to_other_player.playerData = m_player_data;
				players->send_packet(reinterpret_cast<Packet*>(&my_info_to_other_player));
			}

			/// 이동 시야처리 이후...
			// 내 view list 를 검색해서..
			for (auto id : m_view_list) {
				// 만약 봇이라면...
				if (true == g_clients[id]->get_player_data()->is_ai) {					
					// 시야에 있는 컴퓨터 이므로, 랜덤 무빙을 하도록 하게 하자
					if (true != g_clients[id]->ai_is_rand_mov) {
						g_clients[id]->ai_is_rand_mov = true;
						g_time_queue.add_event(id, 0, CHANGE_AI_STATE_MOV, true);
					}
					continue;
				}

				g_clients[id]->send_packet(reinterpret_cast<Packet*>(&p));
			}
			break;
		}

		case CHANGED_DIRECTION: {
			if (dead == m_state) { break; }

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

		case KEYINPUT_POTION: {
			if (dead == m_state) { break; }

			if (true == is_hp_postion) { break; }

			is_hp_postion = true;
			g_time_queue.add_event(m_id, 0, HP_ADD, false);
			g_time_queue.add_event(m_id, 10, POSTION, false);

			break;
		}

		case KEYINPUT_ATTACK:		// 기본 공격 ( 데미지 계산, hit box 범위 조정, 전부 여기서 다 조절해야 한다. )
		{
			int attState = static_cast<int>(buf[2]);	// Player 공격 현재 상태, 콤보 및 스킬 구분하는 용도
			if (dead == m_state) { break; }
			// 왼쪽 키 = 우측 + 아래
			// 우측 키 = 왼쪽 + 위
			// 위 키 = 우측 + 위
			// 아래 키 = 왼쪽 + 아래

			// 충돌체크 검사하고 난 뒤에..
			float att_x = 0.3, att_y = 0.3;		// 테스트용 클라 공격 리치가 요정도
			float my_x = m_player_data.pos.x, my_y = m_player_data.pos.y;
			float player_size = 1.35;	// 객체 충돌 크기 반지름
			char *dir = &m_player_data.dir;
			bool is_gauge_on = false;
			unsigned int deleting_id = 0;
			
			if ((*dir & KEYINPUT_RIGHT) == (KEYINPUT_RIGHT)) { my_x -= att_x; my_y -= att_y; }
			if ((*dir & KEYINPUT_LEFT) == (KEYINPUT_LEFT)) { my_x += att_x; my_y += att_y; }
			if ((*dir & KEYINPUT_UP) == (KEYINPUT_UP)) { my_x += att_x; my_y -= att_y; }
			if ((*dir & KEYINPUT_DOWN) == (KEYINPUT_DOWN)) { my_x -= att_x; my_y += att_y; }

			m_view_lock.lock();
			for (auto id : m_view_list) {
				if (DISCONNECTED == g_clients[id]->m_connect_state) { continue; }
				//if (m_id == g_clients[id]->m_id) { continue; }
				//if (true == players->get_player_data()->is_ai) { continue; }

				float x = g_clients[id]->m_player_data.pos.x;
				float y = g_clients[id]->m_player_data.pos.y;

				// 공격을 했는데 상대가 맞았다고 판정이 된다면...================================================================
				if((player_size * player_size) >= DISTANCE_TRIANGLE(x, y, my_x, my_y)) {

					// 난 이제 공격 상태 --------------
					set_state(att);

					// 현재 공격 콤보 및 스킬에 따라서 데미지 구분해주기
					int addingDamage = 0;
					switch (attState)
					{
					case COMBO1: addingDamage = 0; break;
					case COMBO2: addingDamage = 5; break;
					case COMBO3: addingDamage = 10; break;
						// 게이지 음수 관련하여 수정 & 게이지 깎이면 통보도 해주기
					case SKILL1: {
						if (50 > m_player_data.state.gauge) { break; }
						addingDamage = 30;
						m_player_data.state.gauge -= 50;
						if (1 > m_player_data.state.gauge) { m_player_data.state.gauge = 0; }
						sc_fever p;
						p.gauge = m_player_data.state.gauge;
						send_packet(reinterpret_cast<Packet*>(&p));
						break;
					}
					case SKILL2: {
						if (50 > m_player_data.state.gauge) { break; }
						addingDamage = 20;
						m_player_data.state.gauge -= 50;
						if (1 > m_player_data.state.gauge) { m_player_data.state.gauge = 0; }
						sc_fever p;
						p.gauge = m_player_data.state.gauge;
						send_packet(reinterpret_cast<Packet*>(&p));
						break;
					}
					case SKILL3: {
						if (50 > m_player_data.state.gauge) { break; }
						addingDamage = 15;
						m_player_data.state.gauge -= 50;
						if (1 > m_player_data.state.gauge) { m_player_data.state.gauge = 0; }
						sc_fever p;
						p.gauge = m_player_data.state.gauge;
						send_packet(reinterpret_cast<Packet*>(&p));
						break;
					}
					default: addingDamage = 0; break;
					}

					// 일단 상대 체력 찢기
					std::random_device rd;
					std::mt19937_64 mt(rd());
					std::uniform_int_distribution<int> dist(0, m_sub_status.critical);
					g_clients[id]->m_player_data.state.hp -= ((m_sub_status.str + addingDamage + dist(mt)) - g_clients[id]->m_sub_status.def);
					is_gauge_on = true; // 발열 게이지를 마지막 체크 때 올려주자

					if (false == g_clients[id]->get_hp_adding()) {
						//g_clients[id]->set_hp_adding(true);
						//g_time_queue.add_event(id, 1, HP_ADD, false);	// AI 타격 일때, 따로 hp 추가해 주는 함수가 없다 !!! -> 일반 플레이어와 동일하게 처리함
					}

					// 맞은 놈이 ai 면, 반격을 하자.
					if (MAX_AI_NUM > id) {
						// 공격자의 반대 방향으로 일단 맞보도록 전환
						char ai_dir = DIR_XOR(m_player_data.dir);
						if (g_clients[id]->get_player_data()->dir != ai_dir) {
							g_clients[id]->get_player_data()->dir = ai_dir;

							sc_dir p;
							p.id = id;
							p.dir = ai_dir;
							
							// 방향 바뀐거 알려줘야 할 친구들에게 알려주자.
							for (auto players : g_clients) {
								if (DISCONNECTED == players->get_current_connect_state()) { continue; }
								if (id == players->m_id) { continue; }
								if (true == players->m_player_data.is_ai) { continue; }
								if (SQUARED(VIEW_RANGE) >= DISTANCE_TRIANGLE(g_clients[id]->get_player_data()->pos.x, g_clients[id]->get_player_data()->pos.y, players->m_player_data.pos.x, players->m_player_data.pos.y)) {
									players->send_packet(reinterpret_cast<Packet*>(&p));
								}
							}
						}

						// 맞은 AI 상태가 att 이 아니라면.. 공격 자세를 취해주어야 한다.
						if (att != g_clients[id]->m_state) {
							if (none == g_clients[id]->m_target_id) { g_clients[id]->m_target_id = m_id; }
							g_time_queue.add_event(id, 0, CHANGE_AI_STATE_ATT, true); // 하지만 이건 주변 근접하면 자동으로 공격형으로 변하도록 셋팅하자
						}
						//g_clients[id]->m_state = att;
					}
					
					sc_atk p;
					p.attacking_id = m_id;		// 공격자 id
					p.under_attack_id = id;		// 맞는 놈의 id
					p.hp = g_clients[id]->m_player_data.state.hp;	// 맞은 놈의 hp
					p.comboState = attState;

					// hp 가 0 이 되면 사망처리를 한다. -> 각각의 클라이언트에서 hp 가 0 된 녀석을 지워줌
					if (0 >= g_clients[id]->m_player_data.state.hp) {
						
						// 맞은 애가 ai 면 그냥 연결 끊어서 죽이기
						if (MAX_AI_NUM > g_clients[id]->get_id()) {
							g_clients[id]->m_connect_state = DISCONNECTED;
							g_clients[id]->ai_is_rand_mov = false;
							g_clients[id]->set_state(none);

							sc_disconnect dis_p;
							dis_p.id = id;

							send_packet_other_players(reinterpret_cast<Packet *>(&dis_p), id);
							g_time_queue.add_event(g_clients[id]->m_player_data.id, 10, DEAD_TO_ALIVE, true);	// bot 리젠
							
							// 잡은 녀석에는 경험치도 주도록 하자.
							// 만약 슬라임이고, quest 수치가 10 마리 잡는거 이하라면
							if ((MAX_AI_SLIME > id) && (MAX_AI_SLIME > m_sub_status.quest)) {
								m_sub_status.quest += 1;

								sc_chat chat;
								chat.id = m_id;
								wsprintfW(reinterpret_cast<wchar_t*>(chat.msg), L"슬라임 %d 마리 잡음", m_sub_status.quest);
								if (MAX_AI_SLIME == m_sub_status.quest) { wsprintfW(reinterpret_cast<wchar_t*>(chat.msg), L"슬라임 퀘스트 완료"); }
 								send_packet(reinterpret_cast<Packet*>(&chat));

								sc_quest q;
								q.quest = m_sub_status.quest;
								send_packet(reinterpret_cast<Packet*>(&q));
							}
							else if ((MAX_AI_GOBLIN > id) && (MAX_AI_SLIME <= id) && (MAX_AI_GOBLIN > m_sub_status.quest) && (MAX_AI_SLIME <= m_sub_status.quest)) {
								m_sub_status.quest += 1;

								sc_chat chat;
								chat.id = m_id;
								wsprintfW(reinterpret_cast<wchar_t*>(chat.msg), L"고블린 %d 마리 잡음", m_sub_status.quest - MAX_AI_SLIME);
								if (MAX_AI_GOBLIN == m_sub_status.quest) { wsprintfW(reinterpret_cast<wchar_t*>(chat.msg), L"고블린 퀘스트 완료");; }
								send_packet(reinterpret_cast<Packet*>(&chat));

								sc_quest q;
								q.quest = m_sub_status.quest;
								send_packet(reinterpret_cast<Packet*>(&q));
							}
						}
						else {
							// 죽은 애가 player 일 경우..

							sc_disconnect dis_p;
							dis_p.id = id;

							g_clients[id]->send_packet(reinterpret_cast<Packet*>(&dis_p));
							g_clients[id]->set_state(dead);

							for (auto player_view_ids : *g_clients[id]->get_view_list()) {
								// dead lock 방지용 continue;
								if (player_view_ids == m_id) { deleting_id = id; continue; }
								g_clients[player_view_ids]->vl_remove(id);

								if (true == g_clients[player_view_ids]->m_player_data.is_ai) { continue; }
								g_clients[player_view_ids]->send_packet(reinterpret_cast<Packet*>(&dis_p));
							}

							g_clients[id]->vl_clear();
							g_time_queue.add_event(g_clients[id]->m_player_data.id, 5, DEAD_TO_ALIVE, false);
						}
					}

					send_packet(reinterpret_cast<Packet*>(&p));
					if (true == g_clients[id]->get_player_data()->is_ai) { continue; }
					g_clients[id]->send_packet(reinterpret_cast<Packet*>(&p));
				}
			}
			m_view_lock.unlock();
			if (0 < deleting_id) { vl_remove(deleting_id); }

			if (true == m_player_data.is_ai) { break; }
			// 발열 게이지가 올라가야 한다면 ---- ( AI 는 필요 없으니 skip )
			if (true == is_gauge_on) {
				// 발열 게이지 값을 올리고
				m_player_data.state.gauge += 10;
				if (m_player_data.state.maxgauge < m_player_data.state.gauge) { m_player_data.state.gauge = m_player_data.state.maxgauge; }

				/// 공격을 안한지 3초 부터 게이지가 감소하도록 한다.
				g_time_queue.add_event(m_id, 3, CHANGE_PLAYER_STATE, false);

				// 패킷을 당사자에게 하나 보내주자.
				sc_fever p;
				p.gauge = m_player_data.state.gauge;
				send_packet(reinterpret_cast<Packet*>(&p));
			}
			break;
		}
		case CHAT: {

			sc_chat chat;
			
			chat.id = m_id;
			memcpy(chat.msg, reinterpret_cast<char*>(&buf[2]), MAX_BUF_SIZE - 6);
			chat.msg[MAX_BUF_SIZE - 7] = '\0';

			wchar_t *chatTXT = reinterpret_cast<wchar_t*>(chat.msg);

			wcout << "Message [ " << m_id << " ] : " << chatTXT << endl;

			if (0 == wcscmp(chatTXT, L"show me the hp")) {
				if (101 > m_player_data.state.hp) {
					m_player_data.state.hp += 10000;
					sc_chat cheat;
					memcpy(cheat.msg, reinterpret_cast<wchar_t*>(L"체력 추가 치트 적용 완료"), MAX_BUF_SIZE - 6);
					send_packet(reinterpret_cast<Packet*>(&cheat));

					sc_hp hp_p;
					hp_p.hp = m_player_data.state.hp;
					hp_p.id = m_id;
					send_packet(reinterpret_cast<Packet*>(&hp_p));
				}
				else {
					m_player_data.state.hp = 100;
					sc_chat cheat;
					memcpy(cheat.msg, reinterpret_cast<wchar_t*>(L"체력 치트 해제"), MAX_BUF_SIZE - 6);
					send_packet(reinterpret_cast<Packet*>(&cheat));

					sc_hp hp_p;
					hp_p.hp = m_player_data.state.hp;
					hp_p.id = m_id;
					send_packet(reinterpret_cast<Packet*>(&hp_p));
				}
			}
			if (0 == wcscmp(chatTXT, L"power overwhelming")) {
				m_sub_status.str += 50;
				m_player_data.state.hp += 500;
				sc_chat cheat;
				memcpy(cheat.msg, reinterpret_cast<wchar_t*>(L"공격력 강화 치트 적용 완료"), MAX_BUF_SIZE - 6);
				send_packet(reinterpret_cast<Packet*>(&cheat));
			}

			for (auto players : g_clients) {
				if (DISCONNECTED == players->get_current_connect_state()) { continue; }
				if (true == players->m_player_data.is_ai) { continue; }
				//if (m_id == players->m_id) { continue; }

				players->send_packet(reinterpret_cast<Packet*>(&chat));
			}

			break;
		}
		default:
			// 잘 안날아 오는 패킷 start, web
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
