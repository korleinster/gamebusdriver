#pragma once

using BUFS = struct PACKET_INFO
{
	Packet buf[MAX_BUF_SIZE] = { 0 };
	Packet buf_recv[MAX_BUF_SIZE] = { 0 };
	Packet buf_send[MAX_BUF_SIZE] = { 0 };

	int size_prev = { 0 };
	int size_curr = { 0 };

	WSABUF wsabuf_recv = { 0 };
	WSABUF wsabuf_send = { 0 };
};

class network_class
{
	static network_class* me;

	HWND m_hwnd = { 0 };
	HINSTANCE m_hist = { 0 };

	wchar_t m_server_ip[32] = { L"127.0.0.1" };
	bool m_b_debug_mode = { false };

	BUFS m_buf;
	SOCKET m_sock;
	int m_retval;
	player_class *m_player = { nullptr };

	static INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);

	void server_to_connect();

	void err_quit(wchar_t *msg, int err_no);
	void err_display(char *msg, int err_no, int line, char *func, int id);

public:
	network_class();
	~network_class();

	void init(const HWND&, const HINSTANCE&, void*);
	void process_win_msg(LPARAM);

	template<typename T>
	bool send_packet(T *buf);
	bool send_packet(BYTE data_size, BYTE type, void *buf);
};

