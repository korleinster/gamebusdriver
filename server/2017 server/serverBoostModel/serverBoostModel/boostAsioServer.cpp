#pragma once
#include"stdafx.h"

boostAsioServer::boostAsioServer() : m_acceptor(g_io_service, tcp::endpoint(tcp::v4(), SERVERPORT)), m_socket(g_io_service)
{
	getMyServerIP();
	CheckThisCPUcoreCount();

	// DB SQL 서버에 접속
	database.Init();

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
	m_worker_threads.emplace_back(new thread{ [&]() -> void { time_queue.TimerThread(); } });

	while (m_ServerShutdown) { Sleep(1000); }

	// workerThread 발동
	for (auto thread : m_worker_threads) {
		thread->join();
		delete thread;
	}
}

void boostAsioServer::g_client_init() {

	g_clients.reserve(MAX_AI_NUM + 1000);

	for (auto i = 0; i < MAX_AI_NUM; ++i) {
		g_clients.emplace_back(new player_session(boost::asio::ip::tcp::socket(g_io_service), ++m_playerIndex));
		g_clients[i]->get_player_data()->id = m_playerIndex;
		g_clients[i]->get_player_data()->is_ai = true;
		g_clients[i]->get_player_data()->dir = KEYINPUT_UP;
		g_clients[i]->get_player_data()->pos.x = (150 + i * 2);
		g_clients[i]->get_player_data()->pos.y = (400 + i * 2);
		/*g_clients[i]->get_player_data()->pos.x = rand() % 500;
		g_clients[i]->get_player_data()->pos.y = rand() % 500;*/
		g_clients[i]->get_player_data()->state.maxhp = 100;
		g_clients[i]->get_player_data()->state.hp = g_clients[i]->get_player_data()->state.maxhp;
	}

	cout << "\nAI bots created number of " << MAX_AI_NUM << ", Compelete\n";
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
