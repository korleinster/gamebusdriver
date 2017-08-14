#pragma once
#include"stdafx.h"

boostAsioServer::boostAsioServer() : m_acceptor(g_io_service, tcp::endpoint(tcp::v4(), SERVERPORT)), m_socket(g_io_service)
{
	getMyServerIP();
	CheckThisCPUcoreCount();

	// DB SQL 서버에 접속
	//g_database.Init();

	// ai 봇 초기화
	g_client_init();

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
	m_cpuCore = static_cast<int>(si.dwNumberOfProcessors);
	printf("CPU Core Count = %d, threads = %d\n", m_cpuCore / 2, m_cpuCore);
}

void boostAsioServer::start_io_service()
{
	// DB 서버를 위해 -1
	// Timer Thread 를 위해 -1
	for (int i = 0; i < m_cpuCore - 2; ++i) { m_worker_threads.emplace_back(new thread{ [&]() -> void { g_io_service.run(); } }); }
	m_worker_threads.emplace_back(new thread{ [&]() -> void { g_time_queue.TimerThread(); } });

	while (m_ServerShutdown) { Sleep(1000); }

	// workerThread 발동
	for (auto thread : m_worker_threads) {
		thread->join();
		delete thread;
	}
}

void boostAsioServer::g_client_init() {

	LuaScript script("script/AI_default_status.lua");

	int aiDiffers = script.get<int>("ai_differs");
	int cntAi = 0;

	MAX_AI_SLIME = script.get<int>("ai_status_slime2.howMany");
	MAX_AI_GOBLIN = script.get<int>("ai_status_goblin2.howMany");
	MAX_AI_BOSS = script.get<int>("ai_status_boss.howMany");
	MAX_AI_NUM = MAX_AI_BOSS;

	g_clients.reserve(cntAi + 1000);
	srand((unsigned)time(NULL));

	// 슬라임 AI
	for (int i = 0; i < MAX_AI_SLIME - 10; ++i)
	{
		g_clients.emplace_back(new player_session(boost::asio::ip::tcp::socket(g_io_service), ++m_playerIndex));
		//cout << "slime id = " << m_playerIndex << endl;
		g_clients[i]->get_player_data()->id = m_playerIndex;
		g_clients[i]->get_player_data()->is_ai = true;
		g_clients[i]->get_player_data()->dir = KEYINPUT_UP;

		Position pos;
		pos.x = script.get<float>("ai_status_slime.pos.x");
		pos.y = script.get<float>("ai_status_slime.pos.y");
		g_clients[i]->radius = script.get<int>("ai_status_slime.radius");
		g_clients[i]->origin_pos = pos;

		g_clients[i]->get_player_data()->pos.x = pos.x + ((rand() % g_clients[i]->radius) - (g_clients[i]->radius / 2.0f));
		g_clients[i]->get_player_data()->pos.y = pos.y + ((rand() % g_clients[i]->radius) - (g_clients[i]->radius / 2.0f));

		g_clients[i]->get_player_data()->state.maxhp = script.get<int>("ai_status_slime.status.maxHp");
		g_clients[i]->get_player_data()->state.hp = g_clients[i]->get_player_data()->state.maxhp;

		g_clients[i]->get_sub_data()->critical = script.get<int>("ai_status_slime.subStatus.crit");	// const
		g_clients[i]->get_sub_data()->def = script.get<int>("ai_status_slime.subStatus.def");
		g_clients[i]->get_sub_data()->str = script.get<int>("ai_status_slime.subStatus.str");
		g_clients[i]->get_sub_data()->agi = script.get<int>("ai_status_slime.subStatus.agi");
		g_clients[i]->get_sub_data()->intel = script.get<int>("ai_status_slime.subStatus.intel");
		g_clients[i]->get_sub_data()->health = script.get<int>("ai_status_slime.subStatus.health");

		g_clients[i]->ai_mov_speed = script.get<float>("ai_status_slime.aiMovSpeed");

		g_clients[i]->set_state(none);
		g_clients[i]->m_target_id = none;
	}

	for (int i = MAX_AI_SLIME - 10; i < MAX_AI_SLIME; ++i)
	{
		g_clients.emplace_back(new player_session(boost::asio::ip::tcp::socket(g_io_service), ++m_playerIndex));
		//cout << "slime id = " << m_playerIndex << endl;
		g_clients[i]->get_player_data()->id = m_playerIndex;
		g_clients[i]->get_player_data()->is_ai = true;
		g_clients[i]->get_player_data()->dir = KEYINPUT_UP;

		Position pos;
		pos.x = script.get<float>("ai_status_slime2.pos.x");
		pos.y = script.get<float>("ai_status_slime2.pos.y");
		g_clients[i]->radius = script.get<int>("ai_status_slime2.radius");
		g_clients[i]->origin_pos = pos;

		g_clients[i]->get_player_data()->pos.x = pos.x + ((rand() % g_clients[i]->radius) - (g_clients[i]->radius / 2.0f));
		g_clients[i]->get_player_data()->pos.y = pos.y + ((rand() % g_clients[i]->radius) - (g_clients[i]->radius / 2.0f));

		g_clients[i]->get_player_data()->state.maxhp = script.get<int>("ai_status_slime2.status.maxHp");
		g_clients[i]->get_player_data()->state.hp = g_clients[i]->get_player_data()->state.maxhp;

		g_clients[i]->get_sub_data()->critical = script.get<int>("ai_status_slime2.subStatus.crit");	// const
		g_clients[i]->get_sub_data()->def = script.get<int>("ai_status_slime2.subStatus.def");
		g_clients[i]->get_sub_data()->str = script.get<int>("ai_status_slime2.subStatus.str");
		g_clients[i]->get_sub_data()->agi = script.get<int>("ai_status_slime2.subStatus.agi");
		g_clients[i]->get_sub_data()->intel = script.get<int>("ai_status_slime2.subStatus.intel");
		g_clients[i]->get_sub_data()->health = script.get<int>("ai_status_slime2.subStatus.health");

		g_clients[i]->ai_mov_speed = script.get<float>("ai_status_slime2.aiMovSpeed");

		g_clients[i]->set_state(none);
		g_clients[i]->m_target_id = none;
	}

	// 고블린 AI
	for (int i = MAX_AI_SLIME; i < MAX_AI_GOBLIN - 10; ++i)
	{
		g_clients.emplace_back(new player_session(boost::asio::ip::tcp::socket(g_io_service), ++m_playerIndex));
		//cout << "goblin id = " << m_playerIndex << endl;
		g_clients[i]->get_player_data()->id = m_playerIndex;
		g_clients[i]->get_player_data()->is_ai = true;
		g_clients[i]->get_player_data()->dir = KEYINPUT_UP;

		Position pos;
		pos.x = script.get<float>("ai_status_goblin.pos.x");
		pos.y = script.get<float>("ai_status_goblin.pos.y");
		g_clients[i]->radius = script.get<int>("ai_status_goblin.radius");
		g_clients[i]->origin_pos = pos;

		g_clients[i]->get_player_data()->pos.x = pos.x + ((rand() % g_clients[i]->radius) - (g_clients[i]->radius / 2.0f));
		g_clients[i]->get_player_data()->pos.y = pos.y + ((rand() % g_clients[i]->radius) - (g_clients[i]->radius / 2.0f));

		g_clients[i]->get_player_data()->state.maxhp = script.get<int>("ai_status_goblin.status.maxHp");
		g_clients[i]->get_player_data()->state.hp = g_clients[i]->get_player_data()->state.maxhp;

		g_clients[i]->get_sub_data()->critical = script.get<int>("ai_status_goblin.subStatus.crit"); // const
		g_clients[i]->get_sub_data()->def = script.get<int>("ai_status_goblin.subStatus.def");
		g_clients[i]->get_sub_data()->str = script.get<int>("ai_status_goblin.subStatus.str");
		g_clients[i]->get_sub_data()->agi = script.get<int>("ai_status_goblin.subStatus.agi");
		g_clients[i]->get_sub_data()->intel = script.get<int>("ai_status_goblin.subStatus.intel");
		g_clients[i]->get_sub_data()->health = script.get<int>("ai_status_goblin.subStatus.health");

		g_clients[i]->ai_mov_speed = script.get<float>("ai_status_goblin.aiMovSpeed");
		g_clients[i]->set_state(none);
		g_clients[i]->m_target_id = none;
	}

	for (int i = MAX_AI_GOBLIN - 10; i < MAX_AI_GOBLIN; ++i)
	{
		g_clients.emplace_back(new player_session(boost::asio::ip::tcp::socket(g_io_service), ++m_playerIndex));
		//cout << "goblin id = " << m_playerIndex << endl;
		g_clients[i]->get_player_data()->id = m_playerIndex;
		g_clients[i]->get_player_data()->is_ai = true;
		g_clients[i]->get_player_data()->dir = KEYINPUT_UP;

		Position pos;
		pos.x = script.get<float>("ai_status_goblin2.pos.x");
		pos.y = script.get<float>("ai_status_goblin2.pos.y");
		g_clients[i]->radius = script.get<int>("ai_status_goblin2.radius");
		g_clients[i]->origin_pos = pos;

		g_clients[i]->get_player_data()->pos.x = pos.x + ((rand() % g_clients[i]->radius) - (g_clients[i]->radius / 2.0f));
		g_clients[i]->get_player_data()->pos.y = pos.y + ((rand() % g_clients[i]->radius) - (g_clients[i]->radius / 2.0f));

		g_clients[i]->get_player_data()->state.maxhp = script.get<int>("ai_status_goblin2.status.maxHp");
		g_clients[i]->get_player_data()->state.hp = g_clients[i]->get_player_data()->state.maxhp;

		g_clients[i]->get_sub_data()->critical = script.get<int>("ai_status_goblin2.subStatus.crit"); // const
		g_clients[i]->get_sub_data()->def = script.get<int>("ai_status_goblin2.subStatus.def");
		g_clients[i]->get_sub_data()->str = script.get<int>("ai_status_goblin2.subStatus.str");
		g_clients[i]->get_sub_data()->agi = script.get<int>("ai_status_goblin2.subStatus.agi");
		g_clients[i]->get_sub_data()->intel = script.get<int>("ai_status_goblin2.subStatus.intel");
		g_clients[i]->get_sub_data()->health = script.get<int>("ai_status_goblin2.subStatus.health");

		g_clients[i]->ai_mov_speed = script.get<float>("ai_status_goblin2.aiMovSpeed");
		g_clients[i]->set_state(none);
		g_clients[i]->m_target_id = none;
	}

	// BOSS AI
	for (int i = MAX_AI_GOBLIN; i < MAX_AI_BOSS; ++i)
	{
		g_clients.emplace_back(new player_session(boost::asio::ip::tcp::socket(g_io_service), ++m_playerIndex));
		//cout << "goblin id = " << m_playerIndex << endl;
		g_clients[i]->get_player_data()->id = m_playerIndex;
		g_clients[i]->get_player_data()->is_ai = true;
		g_clients[i]->get_player_data()->dir = KEYINPUT_UP;

		Position pos;
		pos.x = script.get<float>("ai_status_boss.pos.x");
		pos.y = script.get<float>("ai_status_boss.pos.y");
		g_clients[i]->radius = script.get<int>("ai_status_boss.radius");
		g_clients[i]->origin_pos = pos;

		g_clients[i]->get_player_data()->pos.x = pos.x + ((rand() % g_clients[i]->radius) - (g_clients[i]->radius / 2.0f));
		g_clients[i]->get_player_data()->pos.y = pos.y + ((rand() % g_clients[i]->radius) - (g_clients[i]->radius / 2.0f));

		g_clients[i]->get_player_data()->state.maxhp = script.get<int>("ai_status_boss.status.maxHp");
		g_clients[i]->get_player_data()->state.hp = g_clients[i]->get_player_data()->state.maxhp;

		g_clients[i]->get_sub_data()->critical = script.get<int>("ai_status_boss.subStatus.crit"); // const
		g_clients[i]->get_sub_data()->def = script.get<int>("ai_status_boss.subStatus.def");
		g_clients[i]->get_sub_data()->str = script.get<int>("ai_status_boss.subStatus.str");
		g_clients[i]->get_sub_data()->agi = script.get<int>("ai_status_boss.subStatus.agi");
		g_clients[i]->get_sub_data()->intel = script.get<int>("ai_status_boss.subStatus.intel");
		g_clients[i]->get_sub_data()->health = script.get<int>("ai_status_boss.subStatus.health");

		g_clients[i]->ai_mov_speed = script.get<float>("ai_status_boss.aiMovSpeed");
		g_clients[i]->set_state(none);
		g_clients[i]->m_target_id = none;
	}

	cout << "AI bots created number of " << MAX_AI_NUM << ", Compelete\n";
}

void boostAsioServer::acceptThread()
{
	m_acceptor.async_accept(m_socket, [&](boost::system::error_code error_code) {
		if (true == (!error_code)) {
			cout << "Client No. [ " << ++m_playerIndex << " ] Connected \t:: IP = " << m_socket.remote_endpoint().address().to_string() << ", Port = " << m_socket.remote_endpoint().port() << "\n";
			g_clients.emplace_back(new player_session(std::move(m_socket), m_playerIndex));
			if (false != g_clients[m_playerIndex]->check_login()) { g_clients[m_playerIndex]->Init(); }

		}
		if (false == m_ServerShutdown) { acceptThread(); }
	});
}
