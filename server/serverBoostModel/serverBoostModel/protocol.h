#pragma once

#define SERVERPORT 9000
#define MAX_BUF_SIZE 256
#define MAX_USER 500

#define WM_SOCKET (WM_USER + 1)

// packet[1] operation
#define DISCONNECTED 0
#define CONNECTED 1

// iocp buf operation
#define OP_SERVER_RECV 1
#define OP_SERVER_SEND 2

// process protocol
enum PacketProtocolType {
	TEST = 1,		// 받은 패킷 그대로 돌려주기용. ( 보낸 내용이 그대로 돌아오지 않는다면, 클라나 서버에 문제가 있다는 뜻 )
	

	INIT_CLIENT,
	INIT_OTHER_CLIENT,
	PLAYER_DISCONNECTED,
	
	// 캐릭터 좌표 및 방향 관련
	CHANGED_POSITION,
	CHANGED_DIRECTION,

	// 키 입력관련 상호작용
	KEYINPUT_ATTACK,

	// status 값 변경
	CHANGED_HP,
	PASSIVE_HP_ADDED,
};

using Packet = unsigned char;

// 캐릭터 기본 정보 struct
/// 기본 좌표계
using position = struct Position {
	float x{ 100 };
	float y{ 100 };
};
/// 방향 좌표계
#define KEYINPUT_UP		0b00000001
#define KEYINPUT_DOWN	0b00000010
#define KEYINPUT_LEFT	0b00000100
#define KEYINPUT_RIGHT	0b00001000

using status = struct Status {
	int hp{ 100 };
};
/// 플레이어 전체 정보
using player_data = struct Player_data {
	unsigned int	id{ 0 };
	position		pos;
	char			dir{ KEYINPUT_DOWN };
	status			state;
};