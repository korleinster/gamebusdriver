#pragma once
#pragma comment(lib, "ws2_32")
#include<WinSock2.h>

#define SERVERPORT 9000
#define MAX_BUF_SIZE 256
#define MAX_USER 500

// packet[1] operation
#define DISCONNECTED 0

// iocp buf operation
#define OP_SERVER_RECV 1
#define OP_SERVER_SEND 2

using Packet = unsigned char;

using OVLP_EX = struct Overlap_ex {
	OVERLAPPED original_overlap;
	int operation;
	WSABUF wsabuf;
	Packet iocp_buffer[MAX_BUF_SIZE];
};

using PLAYER_INFO = struct Client_INFO {
	SOCKET s;
	unsigned int id;
	bool connected;
	OVLP_EX recv_overlap;
	int packet_size;
	int previous_size;
	Packet packet_buff[MAX_BUF_SIZE];
};

// 클라이언트에서 데이터를 보내고자 할 때, 아래 클래스를 사용하면 편하다.
class SendPacket {
private:
	Packet *buf;	// 실제 데이터 전송을 할 버퍼 공간 ( 전체 사이즈 + 타입 + 실제 데이터 등등... )
	BYTE size = 0;						// 버퍼의 제일 앞 칸에 들어가야 한다 ( buf 배열에 입력 될 때는 +2 가 더해진다 )
	BYTE type = 0;						// 패킷 사이즈 다음 칸에 타입이 정해진다

public:
	SendPacket() {};
	~SendPacket() { delete[] buf; };

	// 현재 데이터 크기, 서버에서 처리할 때 분류할 타입, 서버에 보낼 데이터 복사할 원본 데이터 포인터 위치
	SendPacket(const BYTE data_size, const BYTE type, BYTE* data_start_pointer) : size(data_size), type(type) {
		
		// 실제 최대 버퍼 사이즈 보다 데이터 길이가 커지면 안된다.
		if (MAX_BUF_SIZE < (data_size + 2)) {
			// 아래와 같은 에러가 발생하게 된다면, 버퍼 사이즈를 건들이기 보다 실제 데이터 크기를 압축해 줄여 보낼 수 있도록 하자
			printf("[ code LINE %d ] [ code FUNCTION %s ] SendPacket class ERROR :: data size overed MAX_BUF_SIZE\n", __LINE__, __FUNCTION__);
		}
		else {
			// 혹시 모를 만약의 경우에 대비해 일단 할당으로...
			buf = new Packet[MAX_BUF_SIZE];

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
};