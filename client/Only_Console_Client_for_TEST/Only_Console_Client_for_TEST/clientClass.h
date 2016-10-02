#pragma once

class AsynchronousClientClass
{
public:
	AsynchronousClientClass();
	~AsynchronousClientClass();

	void Init();

	void ConnectingServer();
	void inputServerIP_ReadtxtFile();
	void inputServerIP_cin();

	int recvn();
	void error_display(char *msg, int err_no, int line);
	void error_quit(wchar_t *msg, int err_no);

	SOCKET* getServerSocket();
	void ProcessWinMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void processPacket();

	void sendPacket(const BYTE data_size, const BYTE type, BYTE* data_start_pointer);
	void sendPacket_TEST();

private:
	WSADATA wsadata;
	SOCKET sock;
	int retval{ 0 };

	HWND hWnd;
	CHAR serverIP[32]{ 0 };

	WSABUF wsa_recvbuf;
	WSABUF wsa_sendbuf;

	Packet recvbuf[MAX_BUF_SIZE]{ 0 };
	Packet sendbuf[MAX_BUF_SIZE]{ 0 };
	unsigned int recvbytes{ 0 };
	unsigned int sendbytes{ 0 };
	bool recvdelayed{ 0 };
};