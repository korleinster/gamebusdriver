// boost asio 설치시 참고 내역 --->	http://cattool.tistory.com/94
// C/C++, 일반, 추가 포함 디렉토리 파일 위치	..\boost_1_63_0\boost_1_63_0;				%(AdditionalIncludeDirectories)
// 링커, 일반, 추가 라이브러리 디렉토리			..\boost_1_63_0\boost_1_63_0\stage\lib;		%(AdditionalLibraryDirectories)

#pragma once
#include<boost\asio.hpp>

// 시야리스트 관리용 STL ( 따로 CAS 를 위해 자료구조를 만들 어야 하므로 고민해 보자 )
#include <unordered_set>
#include <mutex>

static boost::asio::io_service g_io_service;

using boost::asio::ip::tcp;

class player_session //: public std::enable_shared_from_this<player_session>
{
public:
	player_session(tcp::socket s, const unsigned int& index) : m_socket(std::move(s)), m_id(index) {};
	~player_session() {};
	
	void Init();

	unsigned int get_id() { return m_id; }
	bool get_current_connect_state() { return m_connect_state; }
	player_data* get_player_data() { return &m_player_data; }
	void send_packet(Packet *packet);
private:
	// Function
	void m_recv_packet();
	void m_process_packet(Packet buf[]);
	void m_passive_hp_adding();

	// 통신용 변수
	tcp::socket		m_socket;

	// 플레이어 상태 변수
	bool m_connect_state{ false };
	unsigned int m_id{ 0 };

	// 버퍼 변수
	Packet m_recv_buf[MAX_BUF_SIZE]{ 0 };
	Packet m_data_buf[MAX_BUF_SIZE]{ 0 };
	unsigned int m_packet_size_current{ 0 };
	unsigned int m_packet_size_previous{ 0 };

	// hp passive 를 위한 bool 변수
	bool m_is_hp_can_add{ false };

	// 플레이어 캐릭터 관련 정보
	player_data m_player_data;

	// 현재 플레이어의 view list
	/// 고민 1. 기본 unorderd_set 자료형을 쓸 것인가 ?
	unordered_set<unsigned int> m_view_list;
	/// 고민 2. 따로 멀티 쓰레드 전용 자료구조를 만들어 쓸 것인가 ? CAS... 5000을 위해서는 써야 할것이다...
};

class boostAsioServer
{
public:
	boostAsioServer();
	~boostAsioServer();
	
private:
	// Function
	void getMyServerIP();
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

// 플레이어가 담긴 변수
static mutex g_clients_lock;
static vector<player_session*> g_clients;