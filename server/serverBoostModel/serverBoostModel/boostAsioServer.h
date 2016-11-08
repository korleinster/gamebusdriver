#pragma once
#include<boost\bind.hpp>
#include<boost\asio.hpp>

class PLAYER_INFO
{
public:
	PLAYER_INFO(boost::asio::io_service& io, const unsigned int id) : m_player_socket(new boost::asio::ip::tcp::socket(io)), m_id(id) {};
	~PLAYER_INFO() { delete m_player_socket; };

	// 복사 및 이동 생성 불가
	PLAYER_INFO(const PLAYER_INFO&) = delete;
	PLAYER_INFO& operator=(const PLAYER_INFO&) = delete;

	// get 시리즈
	boost::asio::ip::tcp::socket* getSocket() { return m_player_socket; }
	Packet* getRecvBuf() { return m_recvBuf; }
	unsigned int getId() { return m_id; }
	bool getConnection() { return m_connect; }

	// set 시리즈
	void setConnection(const bool& state) { m_connect = state; }

	// packet send recv
	void packet_recv_from_client();
	void packet_send_for_client(Packet *packet_ptr, size_t length);
	void Send_Packet(Packet *packet_ptr, unsigned int id);

private:
	boost::asio::ip::tcp::socket* m_player_socket;

	unsigned int m_id;
	bool m_connect{ true };

	Packet m_recvBuf[MAX_BUF_SIZE]{ 0 };
};

class boostAsioServer
{
public:
	boostAsioServer();
	~boostAsioServer();
	
private:
	// Function
	void Init();
	void getMyServerIP();
	void CheckThisCPUcoreCount();

	void makeWorkerThreads_and_AcceptThread();
	void acceptThread();
	void workerThread();

	//void handle_accept(PLAYER_INFO*, const boost::system::error_code& error);

	// member 변수
	boost::asio::io_service m_io_service;

	boost::asio::ip::tcp::resolver				*m_resolver{ nullptr };
	boost::asio::ip::tcp::resolver::query		*m_query{ nullptr };
	boost::asio::ip::tcp::resolver::iterator	m_resolver_iterator;

	//boost::asio::ip::tcp::socket	*m_socket{ nullptr };
	boost::asio::ip::tcp::acceptor	*m_acceptor{ nullptr };
	boost::asio::ip::tcp::endpoint	*m_endpoint{ nullptr };
	//boost::asio::io_service::strand *m_strand{ nullptr };


	unsigned int m_playerIndex{ UINT_MAX };
	vector<PLAYER_INFO *> m_clients;

	bool m_ServerShutdown{ false };
	int m_cpuCore{ 0 };
	vector<thread*> m_worker_threads;
};