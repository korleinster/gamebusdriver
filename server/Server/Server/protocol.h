#pragma once
#pragma comment(lib, "ws2_32")
#include<WinSock2.h>
#include<queue>

#define SERVERPORT 9000
#define MAX_BUF_SIZE 256
#define MAX_USER 500

// packet[1] operation
#define DISCONNECTED 0

// iocp buf operation
#define OP_SERVER_RECV 1
#define OP_SERVER_SEND 2

// process protocol
#define TEST 1			// 받은 패킷 그대로 돌려주기용. ( 보낸 내용이 그대로 돌아오지 않는다면, 클라나 서버에 문제가 있다는 뜻 )

using Packet = unsigned char;

// 오류 검사용 함수라 건드리지 않아도 된다
// 에러 상황을 보고하지만, 서버를 종료시키지 않는다. 몇 번째 줄에서 에러가 발생했는지 코드 줄 수를 세서 위치를 알아낼 수 있다.
// error_display( "화면에 출력할 메세지", 에러 번호, __LINE__ )
void error_display(char *msg, int err_no, int line) {
	WCHAR *lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[ %s - %d ]", msg, line);
	wprintf(L"에러 %s\n", lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// 에러 상황을 보고하고, 서버를 종료
// error_quit( L"화면에 출력할 메세지", 에러 번호, __LINE__ ) - 아예 프로그램이 종료 된다.
void error_quit(wchar_t *msg, int err_no) {
	WCHAR *lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(-1);
}

// 보낸 패킷 할당받은거 메모리 해제용 ( 메세지를 받을 경우 맨 앞에 있는 해당 주소메모리 해제 후, 큐에서 빼준다 ) - < 벡터로 buf_recv 배열과 합칠 예정 >
queue<Packet*> free_memory_queue;

// 클라이언트 전용 받기 & 보내기 버퍼
Packet buf_send[MAX_BUF_SIZE] = { 0 };
Packet buf_recv[MAX_BUF_SIZE] = { 0 };

// 클라이언트에서 데이터를 보내고자 할 때, 아래 클래스를 사용하면 편하다.
class SendPacket {
private:
	Packet *buf;	// 실제 데이터 전송을 할 버퍼 공간 ( 전체 사이즈 + 타입 + 실제 데이터 등등... )
	BYTE size = 0;	// 버퍼의 제일 앞 칸에 들어가야 한다 ( buf 배열에 입력 될 때는 +2 가 더해진다 )
	BYTE type = 0;	// 패킷 사이즈 다음 칸에 타입이 정해진다

public:
	SendPacket() {};
	~SendPacket() { /*delete[] buf;*/ };

	// 현재 데이터 크기, 서버에서 처리할 때 분류할 타입, 서버에 보낼 데이터 복사할 원본 데이터 포인터 위치
	SendPacket(const BYTE data_size, const BYTE type, BYTE* data_start_pointer) : size(data_size), type(type) {
		
		// 실제 최대 버퍼 사이즈 보다 데이터 길이가 커지면 안된다.
		if (MAX_BUF_SIZE < (data_size + 2)) {
			// 아래와 같은 에러가 발생하게 된다면, 버퍼 사이즈를 건들이기 보다 실제 데이터 크기를 압축해 줄여 보낼 수 있도록 하자
			printf("[ code LINE %d ] [ code FUNCTION %s ] SendPacket class ERROR :: data size overed MAX_BUF_SIZE\n", __LINE__, __FUNCTION__);
		}
		else {
			// 혹시 모를 만약의 경우에 대비해 일단 할당으로...
			//buf = new Packet[MAX_BUF_SIZE];

			// 할당 후 비동기 작동시 문제발생할 확률이 높아서 일반 공용 전역 버퍼를 활용한다.
			buf = buf_send;

			// 패킷 안의 실제 내용 생성
			buf[0] = size + 2;
			buf[1] = type;
			memcpy(&buf[2], data_start_pointer, size);
		}
	}

	void Send(const SOCKET s) {
		if (0 >= size) { printf("[ code LINE %d ] SendPacket class ERROR :: there is no data size", __LINE__); }
		if (2 >= buf[0]) { printf("[ code LINE %d ] SendPacket class ERROR :: there is no data size", __LINE__); }

		int retval = send(s, reinterpret_cast<char*>(&buf), buf[0], 0);
		if (retval == SOCKET_ERROR) {
			// 비동기 소켓이라 그냥 리턴, 검사 해주어야 함
			if (WSAGetLastError() != WSAEWOULDBLOCK) {
				int err_no = WSAGetLastError();
				error_quit(L"connect()", err_no);
			}
		}
	}

	// 서버가 살아있는지, 응답은 하는지에 대한 기본 테스트 함수. 3 Bytes 를 보내며, 기본 타입은 TEST, 데이터는 1을 보낸다.
	void Send_default_test(const SOCKET s) {
		buf[0] = 3;
		buf[1] = TEST;
		buf[2] = 1;		// true

		int retval = send(s, reinterpret_cast<char*>(&buf), buf[0], 0);
		if (retval == SOCKET_ERROR) {
			// 비동기 소켓이라 그냥 리턴, 검사 해주어야 함
			if (WSAGetLastError() != WSAEWOULDBLOCK) {
				int err_no = WSAGetLastError();
				error_quit(L"connect()", err_no);
			}
		}
	}

	Packet* getPacketBuf() { return buf; }
};