#pragma once
#pragma comment(lib, "ws2_32")

#include<WinSock2.h>
#include<Windows.h>

#include<iostream>
using namespace std;

// 서버와 연동되는 protocol.h - 수정 시 서버에도 바로 적용된다.
#include"../../../../server/serverBoostModel/serverBoostModel/protocol.h"


#define DUMMY_MAX_SIZE 500

void error_display(char *msg, int err_no, int line);
void error_quit(wchar_t *msg, int err_no);

void init_sock(SOCKET s[]);
void server_stress_test(SOCKET s[]);

int main() {

	// 통신 관련 변수
	WSADATA m_wsadata;

	// init Winsock
	if (WSAStartup(MAKEWORD(2, 2), &m_wsadata) != 0) {
		int err_no = WSAGetLastError();
		error_quit(L"WSAStartup ERROR", err_no);
	}
	
	SOCKET dummy_sock[DUMMY_MAX_SIZE] = { 0 };
	init_sock(dummy_sock);

	server_stress_test(dummy_sock);
}

void init_sock(SOCKET s[]) {
	for (int i = 0; i < DUMMY_MAX_SIZE; ++i)
	{
		s[i] = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
		if (INVALID_SOCKET == s[i]) {
			int err_no = WSAGetLastError();
			error_quit(L"socket()", err_no);
		}
	}

	// non-blocking 으로 변경
	//ULONG flags = 1;
	//for (int i = 0; i < DUMMY_MAX_SIZE; ++i)
	//{
	//	// ioctlsocket 소켓 설정 변경 ( 소켓, 소켓의 입출력모드, 기본 non-block 설정 값 );
	//	ioctlsocket(s[i], FIONBIO, &flags);
	//}

	// connect
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serveraddr.sin_port = htons(SERVERPORT);

	int return_value{ 0 };
	for (int i = 0; i < DUMMY_MAX_SIZE; ++i)
	{
		return_value = WSAConnect(s[i], (SOCKADDR *)&serveraddr, sizeof(serveraddr), NULL, NULL, NULL, NULL);
		if (SOCKET_ERROR == return_value) {
			// 비동기 소켓이라 그냥 리턴, 검사 해주어야 함
			if (WSAGetLastError() != WSAEWOULDBLOCK) {
				int err_no = WSAGetLastError();
				error_quit(L"connect()", err_no);
			}
		}
	}
}

void server_stress_test(SOCKET s[]) {
	int return_value{ 0 };
	unsigned char recv_buf[MAX_BUF_SIZE]{ 0 };
	unsigned char send_buf[MAX_BUF_SIZE]{ 0 };

	send_buf[0] = 2;
	send_buf[1] = DUMMY_CLIENT;

	unsigned long long looped{ 0 };

	while (true)
	{
		for (int i = 0; i < DUMMY_MAX_SIZE; ++i)
		{
			return_value = send(s[i], reinterpret_cast<char*>(&send_buf), send_buf[0], 0);
			if (SOCKET_ERROR == return_value) {
				int err_no = WSAGetLastError();
				error_quit(L"ioctlsocket()", err_no);
			}
		}

		for (int i = 0; i < DUMMY_MAX_SIZE; ++i)
		{
			return_value = recv(s[i], reinterpret_cast<char*>(&recv_buf), send_buf[0], 0);
			if (SOCKET_ERROR == return_value) {
				int err_no = WSAGetLastError();
				error_quit(L"ioctlsocket()", err_no);
			}
		}
		++looped;
		cout << "Looped All Dummy Cliented ( " << DUMMY_MAX_SIZE << " clients ) , Looped times : " << looped << "\n";
	}
}

void error_display(char *msg, int err_no, int line)
{
	WCHAR *lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[ %s - %d ]", msg, line);
	wprintf(L"에러 %s\n", lpMsgBuf);
	LocalFree(lpMsgBuf);
}

void error_quit(wchar_t *msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(-1);
}