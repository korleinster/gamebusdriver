// boost asio 설치시 참고 내역 --->	http://cattool.tistory.com/94
// C/C++, 일반, 추가 포함 디렉토리 파일 위치	..\boost_1_63_0\boost_1_63_0;				%(AdditionalIncludeDirectories)
// 링커, 일반, 추가 라이브러리 디렉토리			..\boost_1_63_0\boost_1_63_0\stage\lib;		%(AdditionalLibraryDirectories)

#pragma once
#include<boost\asio.hpp>

// 시야리스트 관리용 STL ( 따로 CAS 를 위해 자료구조를 만들 어야 하므로 고민해 보자 )
#include <unordered_set>
#include <mutex>
#include <queue>

static boost::asio::io_service g_io_service;

using boost::asio::ip::tcp;

// DB 접근용 class
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <tchar.h>

// Multithread
#include <mutex>

void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);

#define TRYODBC(h, ht, x)   {   RETCODE rc = x;\
                                if (rc != SQL_SUCCESS)	{ HandleDiagnosticRecord (h, ht, rc); } \
                                if (rc == SQL_ERROR)	{ fwprintf(stderr, L"Error in " L#x L"\n"); }} //\
                                    //goto Exit;  \
                                }  \
                            }
#define SQLSERVERADDR	L"Driver={ODBC Driver 13 for SQL Server};Server=tcp:mygame.database.windows.net,1433;Database=myDatabase;Uid=uzchowall@mygame;Pwd={rlagudwns1!};Encrypt=yes;TrustServerCertificate=no;Connection Timeout=30;"

// 데이터 추가
/// L"INSERT INTO dbo.user_data ( ID, Password, Nickname, PlayerLevel, AdminLevel ) VALUES ( '2013180056', 'ghdtmdvlf', N'홍승필', 1, 0 )"
// 데이터 참조 ( http://blog.naver.com/khjkhj2804/220725944795 )
/// if (SQLExecDirect(hstmt, (SQLWCHAR*)L"SELECT ID, Password, Nickname, PlayerLevel, PositionX, PositionY FROM dbo.user_data", SQL_NTS) != SQL_ERROR)
/// L"SELECT RTRIM(ID), RTRIM(Password), RTRIM(Nickname), RTRIM(PlayerLevel), RTRIM(Admin) FROM dbo.user_data"

class DB {
public:
	DB() { /*Init();*/ };
	~DB() { Release(); };

	void SQLcmd(SQLWCHAR* str);
	bool DB_Login(wchar_t* id, wchar_t* pw);

	void Init();
private:
	void Release();

	SQLHENV     hEnv = NULL;
	SQLHDBC     hDbc = NULL;
	SQLHSTMT    hStmt = NULL;
};

// Player Session class ---------------------------------------------------------------------------------------------------------------
#define MAX_HP 100

class player_session //: public std::enable_shared_from_this<player_session>
{
public:
	player_session(tcp::socket s, const unsigned int& index) : m_socket(std::move(s)), m_id(index) {};
	~player_session() {};
	
	void Init();

	bool* get_hp_adding() { return &is_hp_adding; }
	unsigned int get_id() { return m_id; }
	bool get_current_connect_state() { return m_connect_state; }
	player_data* get_player_data() { return &m_player_data; }
	void send_packet(Packet *packet);
	bool check_login();
private:
	// Function
	void m_recv_packet();
	void m_process_packet(Packet buf[]);

	// 통신용 변수
	tcp::socket		m_socket;

	// 플레이어 상태 변수
	bool m_connect_state{ false };
	unsigned int m_id{ 0 };
	bool is_hp_adding{ false };

	// 버퍼 변수
	Packet m_recv_buf[MAX_BUF_SIZE]{ 0 };
	Packet m_data_buf[MAX_BUF_SIZE]{ 0 };
	unsigned int m_packet_size_current{ 0 };
	unsigned int m_packet_size_previous{ 0 };

	// DB 접속 id 와 pw
	wchar_t m_login_id[MAX_BUF_SIZE / 4]{ 0 };
	wchar_t m_login_pw[MAX_BUF_SIZE / 4]{ 0 };
	
	// 플레이어 캐릭터 관련 정보
	player_data m_player_data;

	// 현재 플레이어의 view list
	/// 고민 1. 기본 unorderd_set 자료형을 쓸 것인가 ?
	unordered_set<unsigned int> m_view_list;
	unordered_set<unsigned int> m_view_list_AI;
	/// 고민 2. 따로 멀티 쓰레드 전용 자료구조를 만들어 쓸 것인가 ? CAS... 5000을 위해서는 써야 할것이다...
};

// AI Session class ---------------------------------------------------------------------------------------------------------------
#define MAX_AI_NUM 5000

enum AI_Operator
{
	AI_STAY = 1,
	AI_ATTACK,
};

class AI_session
{
public:
	AI_session() {};
	~AI_session() {};

	bool is_hp_full() { return(m_player_data.state.hp > (MAX_HP - 1)); }
	void change_HP(int add_hp_size) { m_player_data.state.hp += add_hp_size; }
private:
	bool is_wake{ false };

	short m_operate_state{ AI_STAY };
	player_data m_player_data;
};

// Boost Asio class ---------------------------------------------------------------------------------------------------------------
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

// Timer class ---------------------------------------------------------------------------------------------------------------

enum time_queue_event
{
	HP_ADD = 1,

};

/*
	unsigned int obj_id;
	unsigned int wakeup_time;
	int	event_id;
	bool is_ai{ false };
*/
using event_type = struct Event_type
{
	unsigned int obj_id;
	unsigned int wakeup_time;
	int event_id;
	bool is_ai{ false };
};

//auto wake_time_cmp = [](const event_type lhs, const event_type rhs) -> const bool { return (lhs.wakeup_time > rhs.wakeup_time); };
class wake_time_cmp { public: bool operator() (const event_type *lhs, const event_type *rhs) const { return (lhs->wakeup_time > rhs->wakeup_time); }};

class TimerQueue
{
public:
	TimerQueue() {};
	~TimerQueue() {};

	void TimerThread();
	void add_event(const unsigned int& id, const int& sec, time_queue_event type, bool is_ai);
private:
	void processPacket(event_type *p);

	// lock
	mutex time_lock;

	// timer thread queue
	priority_queue < event_type*, vector<event_type*>, wake_time_cmp/*decltype(wake_time_cmp)*/ > timer_queue;
};


// 플레이어가 담긴 변수
static mutex g_clients_lock;
static vector<player_session*> g_clients;
static AI_session g_AIs[MAX_AI_NUM];

// DB 통신용 변수
static DB database;
static TimerQueue time_queue;