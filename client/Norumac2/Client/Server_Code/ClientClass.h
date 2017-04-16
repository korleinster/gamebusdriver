#pragma once
#include<WinSock2.h>
#include<unordered_map>

class AsynchronousClientClass
{
public:
	AsynchronousClientClass();
	~AsynchronousClientClass();

	void Init(const HWND&);

	CHAR* get_server_ip_array() { return m_serverIP; }
	wchar_t* get_login_id_array() { return login_id; }
	wchar_t* get_login_pw_array() { return login_pw; }

	void ProcessWinMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void sendPacket(const BYTE data_size, const BYTE type, BYTE* data_start_pointer);
	void sendPacket_TEST();

	unordered_map<UINT, player_data>* getOtherPlayers() { return &m_other_players; }
	player_data* getPlayerData() { return &m_player; }


private:
	// member Function
	void inputServerIP();
	void inputServerIP_ReadtxtFile();
	void inputServerIP_cin();
	void Login_access();

	int recvn();
	void error_display(char *msg, int err_no, int line);
	void error_quit(wchar_t *msg, int err_no);

	SOCKET* getServerSocket();
	void processPacket(Packet*);

	// 윈도우 창 관련 멤버 변수
	WNDCLASS m_wndclass;
	HWND m_hWnd;
	MSG m_msg;

	// 통신 관련 멤버 변수
	WSADATA m_wsadata;
	SOCKET m_sock;
	int m_retval{ 0 };

	CHAR m_serverIP[32]{ 0 };
	wchar_t login_id[MAX_BUF_SIZE / 4]{ L"guest" };
	wchar_t login_pw[MAX_BUF_SIZE / 4]{ L"guest" };

	WSABUF m_wsa_recvbuf;
	WSABUF m_wsa_sendbuf;

	Packet m_recvbuf[MAX_BUF_SIZE]{ 0 };
	Packet m_sendbuf[MAX_BUF_SIZE]{ 0 };
	Packet m_data_buf[MAX_BUF_SIZE]{ 0 };
	unsigned int m_recvbytes{ 0 };
	unsigned int m_packet_size_current{ 0 };
	unsigned int m_packet_size_previous{ 0 };

	// 실제 ingame에 사용할 멤버 변수 데이터
	player_data m_player;
	unordered_map<UINT, player_data> m_other_players;
};