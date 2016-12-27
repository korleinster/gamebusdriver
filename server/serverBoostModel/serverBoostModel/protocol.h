#pragma once

#define SERVERPORT 9000
#define MAX_BUF_SIZE 256
#define MAX_USER 500

// packet[1] operation
#define DISCONNECTED 0
#define CONNECTED 1

// iocp buf operation
#define OP_SERVER_RECV 1
#define OP_SERVER_SEND 2

// process protocol
enum PacketProtocolType {
	TEST = 1,		// 받은 패킷 그대로 돌려주기용. ( 보낸 내용이 그대로 돌아오지 않는다면, 클라나 서버에 문제가 있다는 뜻 )
	DUMMY_CLIENT,	// dummy client 용 프로토콜
	KEYINPUT,		// 클라이언트에서 키 입력을 받았을 경우

};	

using Packet = unsigned char;

// 캐릭터 기본 정보 struct
/// 기본 좌표계
using position = struct Position {
	float x{ 0 };
	float y{ 0 };
};
/// 플레이어 전체 정보
using player_data = struct Player_data {
	unsigned int id{ 0 };
	position pos;
};