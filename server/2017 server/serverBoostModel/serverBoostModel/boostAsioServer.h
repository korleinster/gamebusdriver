#pragma once


using boost::asio::ip::tcp;

class boostAsioServer
{
public:
	boostAsioServer();
	~boostAsioServer();

private:
	// Function
	void getMyServerIP();
	void g_client_init();
	void CheckThisCPUcoreCount();

	void start_io_service();
	void acceptThread();

	// 통신용 변수
	tcp::acceptor		m_acceptor;
	tcp::socket			m_socket;

	// 플레이어 고유 id 변수
	unsigned int	m_playerIndex{ UINT_MAX };

	// 서버 기본 정보 관련 변수
	bool			m_ServerShutdown{ false };
	int				m_cpuCore{ 0 };
	vector<thread*>	m_worker_threads;
};

extern mutex g_clients_lock;
extern vector<player_session*> g_clients;
//static AI_session g_AIs[MAX_AI_NUM];