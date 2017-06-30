#include "stdafx.h"

network_class* network_class::me = nullptr;

INT_PTR CALLBACK network_class::AboutDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_COMMAND: {

		switch (LOWORD(wParam))
		{
		case IDOK: {
			GetDlgItemText(hDlg, IDC_IP, me->m_server_ip, sizeof(me->m_server_ip));

			EndDialog(hDlg, IDOK);
			return TRUE;
		}
		case IDCANCEL: {
			EndDialog(hDlg, IDCANCEL);
			PostQuitMessage(0);
			return TRUE;
		}
		default:
			break;
		}

		break;
	}
	case WM_INITDIALOG: {

		SetDlgItemText(hDlg, IDC_IP, me->m_server_ip);

		return TRUE;
	}
	default:
		break;
	}

	return FALSE;
}

void network_class::server_to_connect()
{
	WSADATA wsa;

	// init Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		err_quit(L"WSAStartup ERROR", WSAGetLastError());
	}

	m_sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	if (INVALID_SOCKET == m_sock) {	err_quit(L"socket()", WSAGetLastError()); }

	// wchar_t to char
	char server_ip[32] = { 0 };
	WideCharToMultiByte(CP_ACP, 0, m_server_ip, -1, server_ip, 32, 0, 0);

	// connect
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(server_ip);
	serveraddr.sin_port = htons(9000);

	m_retval = WSAConnect(m_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr), NULL, NULL, NULL, NULL);
	if (SOCKET_ERROR == m_retval) {
		// 비동기 소켓이라 그냥 리턴, 검사 해주어야 함
		int err_no = WSAGetLastError();
		if (err_no != WSAEWOULDBLOCK) {
			err_quit(L"connect()", err_no);
		}
	}

	m_retval = WSAAsyncSelect(m_sock, m_hwnd, WM_SOCKET, FD_READ | FD_CLOSE);
	if (SOCKET_ERROR == m_retval) {	err_quit(L"ioctlsocket()", WSAGetLastError()); }

	m_buf.wsabuf_recv.buf = reinterpret_cast<CHAR*>(m_buf.buf_recv);
	m_buf.wsabuf_recv.len = sizeof(m_buf.buf_recv);
	m_buf.wsabuf_send.buf = reinterpret_cast<CHAR*>(m_buf.buf_send);
	m_buf.wsabuf_send.len = sizeof(m_buf.buf_send);

	if (true == m_b_debug_mode) { printf("Connect Server Complete\n"); }
}

void network_class::err_quit(wchar_t * msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(-1);
}

void network_class::err_display(char * msg, int err_no, int line, char * func, int id)
{
	// not using id in client
	if (false == m_b_debug_mode) { return; }

	WCHAR *lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	printf("%s [ %s - %d ] ", func, msg, line);
	wprintf(L"에러 %s\n", lpMsgBuf);
	LocalFree(lpMsgBuf);
}

void network_class::init(const HWND &hw, const HINSTANCE &hi, void *player_class_ptr) {
	me = this;
#ifdef _DEBUG
	m_b_debug_mode = true;
#else
	m_b_debug_mode = false;
#endif // _DEBUG
	if (true == m_b_debug_mode) {
		AllocConsole();
		freopen("conout$", "wt", stdout);
	}

	m_hwnd = hw;
	m_hist = hi;
	m_player = reinterpret_cast<player_class*>(player_class_ptr);

	DialogBox(m_hist, MAKEINTRESOURCE(IDD_DIALOG1), m_hwnd, AboutDlgProc);

	server_to_connect();
}

network_class::network_class() {}

network_class::~network_class()
{
	WSACleanup();

	if (true == m_b_debug_mode) {
		FreeConsole();
		fclose(stdout);
	}
}

void network_class::process_win_msg(LPARAM lparam)
{
	// 오류 발생 여부 확인
	if (true == WSAGETSELECTERROR(lparam)) {
		err_display("ProcessWinMessage::", WSAGETSELECTERROR(lparam), __LINE__, __FUNCTION__, NULL);
		return;
	}

	switch (WSAGETSELECTEVENT(lparam))
	{
	case FD_READ: {	// 데이터 수신

		DWORD ioByteSize{ 0 };
		DWORD ioFlag{ 0 };

		m_retval = WSARecv(m_sock, &m_buf.wsabuf_recv, 1, &ioByteSize, &ioFlag, NULL, NULL);
		if (SOCKET_ERROR == m_retval) {
			err_display("FD_READ::", WSAGetLastError(), __LINE__, __FUNCTION__, NULL);
			return;
		}

		Packet *buf_ptr = m_buf.buf_recv;

		int remained = ioByteSize;
		while (0 < remained) {
			if (0 == m_buf.size_curr) { m_buf.size_curr = buf_ptr[0]; }
			int required = m_buf.size_curr - m_buf.size_prev;
			if (remained >= required) {
				memcpy(m_buf.buf + m_buf.size_prev, buf_ptr, required);

				m_player->process_packet(m_buf.buf);

				buf_ptr += required;
				remained -= required;
				m_buf.size_curr = 0;
				m_buf.size_prev = 0;
			}
			else {
				memcpy(m_buf.buf + m_buf.size_prev, buf_ptr, remained);
				buf_ptr += remained;
				m_buf.size_prev += remained;
				remained = 0;
			}
		}/*
		int remained = ioByteSize;
		while (0 < remained) {
			if (0 == m_buf.size_curr) { m_buf.size_curr = buf_ptr[0]; }
			int required = m_buf.size_curr - m_buf.size_prev;
			if (remained >= required) {
				memcpy(m_buf.buf + m_buf.size_prev, buf_ptr, required);

				m_player->process_packet(m_buf.buf);

				buf_ptr += required;
				remained -= required;
				m_buf.size_curr = 0;
				m_buf.size_prev = 0;
			}
			else {
				memcpy(m_buf.buf + m_buf.size_prev, buf_ptr, remained);
				buf_ptr += remained;
				m_buf.size_prev += remained;
				remained = 0;
			}
		}*/
	}
	default:
		break;
	}
}

bool network_class::send_packet(BYTE data_size, BYTE type, void *buf)
{
	if (MAX_BUF_SIZE < data_size + 2) {
		if (true == m_b_debug_mode) { printf("Send Packet data size was over than Max Packet Size\n"); }
		return false;
	}

	m_buf.buf_send[0] = data_size + 2;
	m_buf.buf_send[1] = type;
	if (nullptr != buf) { memcpy(&m_buf.buf_send[2], buf, m_buf.buf_send[0]); }

	m_buf.wsabuf_send.len = m_buf.buf_send[0];
	DWORD ioByteSize;
	m_retval = WSASend(m_sock, &m_buf.wsabuf_send, 1, &ioByteSize, 0, NULL, NULL);
	if (SOCKET_ERROR == m_retval) {
		// 비동기 소켓이라 그냥 리턴, 검사 해주어야 함
		int err_no = WSAGetLastError();
		if (err_no != WSAEWOULDBLOCK) {
			err_quit(L"sendPacket()", err_no);
		}
	}

	return true;
}

template<typename T>
bool network_class::send_packet(T *buf)
{
	if (nullptr == buf) { return false; }
	if (MAX_BUF_SIZE < buf[0]) {
		if (true == m_b_debug_mode) { printf("Send Packet data size was over than Max Packet Size\n"); }
		return false;
	}

	memcpy(&m_buf.buf_send[0], buf, buf[0]);

	m_buf.wsabuf_send.len = m_buf.buf_send[0];
	DWORD ioByteSize;
	m_retval = WSASend(m_sock, &m_buf.wsabuf_send, 1, &ioByteSize, 0, NULL, NULL);
	if (SOCKET_ERROR == m_retval) {
		// 비동기 소켓이라 그냥 리턴, 검사 해주어야 함
		int err_no = WSAGetLastError();
		if (err_no != WSAEWOULDBLOCK) {
			err_quit(L"sendPacket()", err_no);
		}
	}

	return true;
}