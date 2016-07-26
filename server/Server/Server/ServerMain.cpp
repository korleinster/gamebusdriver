#include"protocol.h"
#include<Windows.h>
#include<stdlib.h>

#include<iostream>
#include<thread>
#include<vector>
using namespace std;

/*
	16.07.06
	서버 기본 틀. 클라이언트를 저장하는 clients 가 원래 array 었으나, vector 포인터 형태로 처음 변경해 보았기 때문에, 버그가 발생할 소지가 있다.
	클라이언트에서 정보를 잘 전달하기 위해, 기본 프로토콜을 같이 적용하여 추후 만들 생각 이다.

	16.07.26
	일부 주석 설명 변경 및 process 함수를 if 문에서 switch 문으로 변경
	몇몇 조건 부분을 가독성을 높이기 위해 TRUE == (!ServerShutdwon) 으로 변경
*/

// 서버의 기초가 되는 함수 및 변수 ( 여기서 건드릴 함수 및 변수는 없다 )
void GetServerIpAddress();
void ServerClose();
void error_display(char *, int, int);
void error_quit(wchar_t *, int);
int checkCpuCore();
void Initialize();
void SendPacket(int, Packet *);

HANDLE g_hIocp;
bool ServerShutdown{ false };

void workerThreads();
// ----- 여기까지의 함수 및 변수는 건드리지 말자 !! -----

// 처음 접속한 클라이언트에게 데이터를 주거나, 다른 클라이언트에게 정보를 전송해 주어야 하는 경우, 여기에 코드가 들어가야 한다.
void acceptThread();

// 날아온 정보를 기반으로 데이터를 분석하여 처리하는 함수. ( 클라이언트에서 보낸 모든 통신 데이터는 이 함수를 거친다 )
void ProcessPacket(unsigned int, const Packet[]);

// 접속하는 클라이언트들에게 고유의 번호를 주는 변수
unsigned int playerIndex{ UINT_MAX };

// 클라이언트가 접속하면, 순차적으로 이 벡터에 들어가게 된다. ( playerIndex 가 곧 고유의 클라이언트 id가 되며, id를 통해 바로 직접 접근 가능 )
vector<PLAYER_INFO *> clients;

int main() {

	_wsetlocale(LC_ALL, L"korean");
	GetServerIpAddress();
	Initialize();
	int cpuCore = checkCpuCore();

	vector<thread *> worker_threads;
	worker_threads.reserve(cpuCore);

	for (int i = 0; i < cpuCore; ++i) { worker_threads.push_back(new thread{ workerThreads }); }

	thread accept_thread{ acceptThread };

	while (ServerShutdown) { Sleep(1000); }

	for (auto thread : worker_threads) {
		thread->join();
		delete thread;
	}

	accept_thread.join();

	ServerClose();
}

// 서버의 IP를 알아내는 함수
void GetServerIpAddress() {
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	PHOSTENT hostinfo;
	char hostname[50];
	char ipaddr[50];
	memset(hostname, 0, sizeof(hostname));
	memset(ipaddr, 0, sizeof(ipaddr));

	int nError = gethostname(hostname, sizeof(hostname));
	if (nError == 0)
	{
		hostinfo = gethostbyname(hostname);
		strcpy(ipaddr, inet_ntoa(*reinterpret_cast<struct in_addr*>(hostinfo->h_addr_list[0])));
	}
	WSACleanup();
	printf("This Server's IP address : %s\n", ipaddr);
}

// 메인 서버 전부 종료용 함수
void ServerClose() {
	WSACleanup();
}

// 에러 상황을 보고하지만, 서버를 종료시키지 않는다. 몇 번째 줄에서 에러가 발생했는지 코드 줄 수를 세서 위치를 알아낼 수 있다.
void error_display(char *msg, int err_no, int line) {
	WCHAR *lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[ %s - %d ]", msg, line);
	wprintf(L"에러 %s\n", lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// 에러 상황을 보고하고, 서버를 종료
void error_quit(wchar_t *msg, int err_no) {
	WCHAR *lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(-1);
}

// 멀티쓰레드를 위한 cpu 코어 갯 수 체크 함수 ( 최대 쓰레드 갯수를 리턴 )
int checkCpuCore() {
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	int cpuCore = static_cast<int>(si.dwNumberOfProcessors) * 2;
	printf("CPU Core Count = %d, threads = %d\n", cpuCore / 2, cpuCore);
	return cpuCore;
}

// 소켓도 초기화를 하지만, 기본적인 클라이언트나 다른 맵, 처음 실행되어 메모리에 올라가야할 사항들을 초기화 해주는 함수
void Initialize() {

	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	g_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (g_hIocp == NULL) {
		int err_no = WSAGetLastError();
		error_quit(L"Initialize::CreateIoCompletionPort", err_no);
	}
}

// 클라이언트들 에게 통신을 보내는 함수 ( 보내야 하는 클라이언트 id 와, 패킷을 보내야 하는 버퍼 주소가 필요하다 )
void SendPacket(int id, Packet *packet) {
	// packet[0] = packet size
	// packet[1] = type
	// packet[...] = data

	OVLP_EX *over = new OVLP_EX;
	memset(over, 0, sizeof(OVLP_EX));
	over->operation = OP_SERVER_SEND;
	over->wsabuf.buf = reinterpret_cast<char *>(over->iocp_buffer);
	over->wsabuf.len = packet[0];
	memcpy(over->iocp_buffer, packet, packet[0]);

	DWORD flags{ 0 };
	int retval = WSASend(clients[id]->s, &over->wsabuf, 1, NULL, flags, &over->original_overlap, NULL);
	if (SOCKET_ERROR == retval) {
		int err_no = WSAGetLastError();
		if (ERROR_IO_PENDING != err_no) {
			error_display("SendPacket::WSASend", err_no, __LINE__);
			while (true);
		}
	}
}

void workerThreads() {
	while (TRUE == (!ServerShutdown)) {
		DWORD key;
		DWORD iosize;
		OVLP_EX *my_overlap;

		BOOL result = GetQueuedCompletionStatus(g_hIocp, &iosize, &key, reinterpret_cast<LPOVERLAPPED *>(&my_overlap), INFINITE);

		// 클라이언트가 접속을 끊었을 경우
		if (FALSE == result || 0 == iosize) {
			if (FALSE == result) {
				int err_no = WSAGetLastError();
				error_display("WorkerThreadStart::GetQueuedCompletionStatus", err_no, __LINE__);
			}
			
			closesocket(clients[key]->s);
			clients[key]->connected = false;

			/* view list 에서 빼주자 */
			/* 모든 클라이언트에게, 현재 클라이언트가 끊겼다고 알려주자 */

			/*Packet p[11];
			p[0] = 11;
			p[1] = DISCONNECTED;
			*((int *)(&p[2])) = key;*/

			//for (int i = 0; i < clients.size(); ++i) {
			//	if (false == clients[i]->connected) { continue; }
			//	//if (i == playerIndex) { continue; }

			//	SendPacket(i, p);
			//}
			continue;
		}
		else if (OP_SERVER_RECV == my_overlap->operation) {
			// 클라이언트로 부터 데이터를 받았을 경우
			Packet *buf_ptr = clients[key]->recv_overlap.iocp_buffer;
			int remained = iosize;
			while (0 < remained) {
				if (0 == clients[key]->packet_size) { clients[key]->packet_size = buf_ptr[0]; }

				int required = clients[key]->packet_size - clients[key]->previous_size;

				if (remained >= required) {
					memcpy(clients[key]->packet_buff + clients[key]->previous_size, buf_ptr, required);

					// 아래 함수에서 패킷을 처리하게 된다.
					ProcessPacket(key, clients[key]->packet_buff);

					buf_ptr += required;
					remained -= required;

					clients[key]->packet_size = 0;
					clients[key]->previous_size = 0;
				}
				else {
					memcpy(clients[key]->packet_buff + clients[key]->previous_size, buf_ptr, remained);
					buf_ptr += remained;
					clients[key]->previous_size += remained;
					remained = 0;
				}
			}
			DWORD flags = 0;
			int retval = WSARecv(clients[key]->s, &clients[key]->recv_overlap.wsabuf, 1, NULL, &flags, &clients[key]->recv_overlap.original_overlap, NULL);
			if (SOCKET_ERROR == retval) {
				int err_no = WSAGetLastError();
				if (ERROR_IO_PENDING != err_no) {
					error_display("WorkerThreadStart::WSARecv", err_no, __LINE__);
				}
				continue;
			}
		}
		else if (OP_SERVER_SEND == my_overlap->operation) {
			// 서버에서 메세지를 보냈으면, 메모리를 해제해 준다.
			delete my_overlap;
		}
		else {
			cout << "Unknown IOCP event !!\n";
			exit(-1);
		}
	}
}

void acceptThread() {
	int retval{ 0 };

	// socket() - IPv4 ( AF_INET )
	SOCKET listen_sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listen_sock == INVALID_SOCKET) {
		int err_no = WSAGetLastError();
		error_quit(L"socket()", err_no);
	};

	// bind()
	struct sockaddr_in serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = ::bind(listen_sock, reinterpret_cast<struct sockaddr *>(&serveraddr), sizeof(serveraddr));
	if (retval == SOCKET_ERROR) {
		int err_no = WSAGetLastError();
		error_quit(L"socket()", err_no);
	}

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) {
		int err_no = WSAGetLastError();
		error_quit(L"socket()", err_no);
	}
	
	while (TRUE == (!ServerShutdown)) {
		// accept()
		struct sockaddr_in clientaddr;
		int addrlen = sizeof(clientaddr);
		SOCKET client_sock = WSAAccept(listen_sock, reinterpret_cast<sockaddr *>(&clientaddr), &addrlen, NULL, NULL);
		if (INVALID_SOCKET == client_sock) {
			int err_no = WSAGetLastError();
			error_display("Accept::WSAAccept", err_no, __LINE__);
			while (true);
		}

		/* DB 관련 login 기능이 여기에 추가되어야 한다. 로그인이 번호가 제대로 맞으면 통과, 아니면 클라이언트 연결을 끊는다. 로그인을 하면 DB에서 정보를 가져온다 */

		playerIndex += 1;
		printf("[ No. %u ] Client IP = %s, Port = %d is Connected\n", playerIndex, inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(client_sock), g_hIocp, playerIndex, 0);

		PLAYER_INFO *user = new PLAYER_INFO;

		user->s = client_sock;
		user->connected = true;
		user->id = playerIndex;
		user->packet_size = 0;
		user->previous_size = 0;
		memset(&user->recv_overlap.original_overlap, 0, sizeof(user->recv_overlap.original_overlap));
		user->recv_overlap.operation = OP_SERVER_RECV;
		user->recv_overlap.wsabuf.buf = reinterpret_cast<char*>(&user->recv_overlap.iocp_buffer);
		user->recv_overlap.wsabuf.len = sizeof(user->recv_overlap.iocp_buffer);

		/* 추가로 데이터가 필요한 경우라면, PLAYER_INFO 구조체 내용을 수정하고, 추가로 초기화 값이나 불러오는 내용을 대입하여 넣어주어야 한다. */

		clients.push_back(move(user));

		/* 주변 클라이언트에 대해 뿌릴 정보 뿌리고, 시야 리스트나 처리해야 할 정보들도 함께 넣는다. */		

		// 클라이언트에서 응답오길 기다리기
		DWORD flags{ 0 };
		retval = WSARecv(client_sock, &clients[playerIndex]->recv_overlap.wsabuf, 1, NULL, &flags, &clients[playerIndex]->recv_overlap.original_overlap, NULL);
		if (SOCKET_ERROR == retval) {
			int err_no = WSAGetLastError();
			if (ERROR_IO_PENDING != err_no) {
				error_display("Accept::WSARecv", err_no, __LINE__);
			}
		}
	}
}

// 모든 패킷은 이 함수를 거쳐간다.
void ProcessPacket(unsigned int id, const Packet buf[]) {
	// packet[0] = packet size		> 0번째 자리에는 무조건, 패킷의 크기가 들어가야만 한다.
	// packet[1] = type				> 1번째 자리에는 현재 패킷이 무슨 패킷인지 속성을 정해주는 값이다.
	// packet[...] = data			> 2번째 부터는 속성에 맞는 순대로 처리를 해준다.

	// buf[1] 번째의 속성으로 분류를 한 뒤에, 내부에서 2번째 부터 데이터를 처리하기 시작한다.
	switch (buf[1])
	{
	case 1:
	{

	}
		break;
	default:
	{
		// 클라이언트로 부터 알수 없는 데이터가 왔을 경우, 해킹 방지를 위해 서버를 강제 종료. 해당 클라이언트의 고유 번호와 타입 번호를 알려준다.
		printf("ERROR, Unknown signal -> [ %u ] protocol num = %d\n", id, buf[1]);
		exit(-1);
	}
		break;
	}
}