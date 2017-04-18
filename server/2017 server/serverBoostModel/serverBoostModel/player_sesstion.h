#pragma once

using boost::asio::ip::tcp;


class player_session //: public std::enable_shared_from_this<player_session>
{
public:
	player_session(tcp::socket s, const unsigned int& index) : m_socket(std::move(s)), m_id(index) {};
	~player_session() {};

	void Init();

	inline bool* get_hp_adding() { return &is_hp_adding; }
	inline bool set_hp(int hp) { m_player_data.state.hp = hp; return true; }
	inline int get_maxhp() { return m_player_data.state.maxhp; }

	inline unsigned int get_id() { return m_id; }
	inline bool get_current_connect_state() { return m_connect_state; }
	inline bool set_connect_state(bool b) { m_connect_state = b; return b; }
	inline player_data* get_player_data() { return &m_player_data; }
	void send_packet(Packet *packet);
	bool check_login();

	mutex player_lock;
private:
	// Function
	void m_recv_packet();
	void m_process_packet(Packet buf[]);

	// 통신용 변수
	tcp::socket		m_socket;

	// 플레이어 상태 변수
	bool m_connect_state{ true };
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
	sub_status m_sub_status;

	// 현재 플레이어의 view list
	/// 고민 1. 기본 unorderd_set 자료형을 쓸 것인가 ?
	unordered_set<unsigned int> m_view_list;
	/// 고민 2. 따로 멀티 쓰레드 전용 자료구조를 만들어 쓸 것인가 ? CAS... 5000을 위해서는 써야 할것이다...
};