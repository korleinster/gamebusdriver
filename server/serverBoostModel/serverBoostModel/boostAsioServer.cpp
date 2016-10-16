#pragma once
#include"stdafx.h"
using boost::asio::ip::tcp;

boostAsioServer::boostAsioServer()
{
	Init();

	getMyServerIP();
	CheckThisCPUcoreCount();

	makeWorkerThreads_and_AcceptThread();
}

boostAsioServer::~boostAsioServer()
{
	// getMyServerIP
	if (nullptr != m_resolver)	delete m_resolver;
	if (nullptr != m_query)		delete m_query;

	// Init
	if (nullptr != m_endpoint)	delete m_endpoint;
	if (nullptr != m_strand)	delete m_strand;
	if (nullptr != m_acceptor)	delete m_acceptor;
}

void boostAsioServer::getMyServerIP()
{
	// 아래 while 문은 없어도 그만, 서버의 ip 를 보여주는 loop 문이다.
	while (m_resolver_iterator != tcp::resolver::iterator()) {
		using boost::asio::ip::address;
		address addr = (m_resolver_iterator++)->endpoint().address();
		if (!addr.is_v6()) {
			cout << "This Server's IP address: " << addr.to_string() << endl;
		}
	}
}

void boostAsioServer::Init()
{
	// 입력 받은 host를 resolving 한다 - ( 현재 아래 3줄은 서버 열 때, 필수적으로 초기화 해주어야 하는 변수들 )
	m_resolver = new tcp::resolver(m_io_service);
	m_query = new tcp::resolver::query(boost::asio::ip::host_name(), "");
	m_resolver_iterator = m_resolver->resolve(*m_query);

	// endpoint 는 network address 설정
	m_endpoint = new tcp::endpoint(tcp::v4(), SERVERPORT);

	// strand 는 자신을 통해 디스패치 되는 핸들러에게, 실행중인 핸들러가 완료되어야만 다음 핸들러가 시작될 수 있도록 하는 것을 보장해줍니다.
	m_strand = new boost::asio::io_service::strand(m_io_service);

	// listen 을 위한 acceptor 를 초기화
	m_acceptor = new tcp::acceptor(m_io_service, *m_endpoint);
}

void boostAsioServer::CheckThisCPUcoreCount()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	m_cpuCore = static_cast<int>(si.dwNumberOfProcessors) * 2;
	printf("CPU Core Count = %d, threads = %d\n", m_cpuCore / 2, m_cpuCore);
}

void boostAsioServer::makeWorkerThreads_and_AcceptThread()
{	
	m_worker_threads.reserve(m_cpuCore);

	for (int i = 0; i < m_cpuCore; ++i)
	{
		m_worker_threads.emplace_back(new thread{ boost::bind( &boost::asio::io_service::run, &m_io_service), this });
	}
	
	thread acceptThread{ &boostAsioServer::acceptThread, this };
	while (m_ServerShutdown) { Sleep(1000); }

	// io_service 의 run() 은 다른 동작을 완료하면 리턴한다는데, 사실상 다음 할일을 받을 준비가 되어있는 상태를 뜻하는 듯 하다.
	m_io_service.run();
	
	// workerThread 발동
	for (auto thread : m_worker_threads) {
		thread->join();
		delete thread;
	}
	
	// acceptThread 발동
	acceptThread.join();

}

void boostAsioServer::acceptThread()
{
	while (true == (!m_ServerShutdown))
	{
		// 접속한 클라이언트에 할당할 tcp::socket 을 만든다. socket 을 통해서 클라이언트 메세지를 주고 받으므로 m_io_serviec 를 할당
		// 여기에 해당하는 iocp 는 accept, 와 g_hIocp = CreateIoCompletionPort(...) 부분이 합쳐져 있는 것과 같다.
		m_clients.emplace_back(new PLAYER_INFO(m_io_service, ++m_playerIndex));
		
		// 현재 이 부분 에러남
		/*m_clients[m_playerIndex]->getSocket()->async_connect(*m_endpoint,
			boost::bind([&](const boost::system::error_code& error) {
			if (error) { cout << "connect ERROR failed : " << m_playerIndex << "player\n"; }
			else { cout << "[ No. " << m_playerIndex << " ] Client IP = " << m_clients[m_playerIndex]->getSocket()->remote_endpoint().address().to_string() << ", Port = " << m_clients[m_playerIndex]->getSocket()->remote_endpoint().port() << " is Connected\n"; }
		}, this, boost::asio::placeholders::error));*/
	}
}

void boostAsioServer::workerThread()
{
	while (true == (!m_ServerShutdown))
	{

	}
}