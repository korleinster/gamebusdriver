#include"stdafx.h"

void AsynchronousClientClass::processPacket(Packet *buf)
{
	switch (buf[1])
	{
	case CHANGED_POSITION: {
		sc_move *p = reinterpret_cast<sc_move*>(buf);

		m_other_players[p->id].pos = p->pos;
		break;
	}

	case CHANGED_DIRECTION: {
		sc_dir *p = reinterpret_cast<sc_dir*>(buf);

		m_other_players[p->id].dir = p->dir;
		break;
	}
	case SERVER_MESSAGE_HP_CHANGED: {
		sc_hp *p = reinterpret_cast<sc_hp*>(buf);
		
		// 나의 hp 가 변경되었다면...
		if (m_player.id == p->id) {
			m_player.state.hp = p->hp;
			break;
		}

		// 내가 아닌 다른 녀석이라면
		m_other_players[p->id].state.hp = p->hp;
		break;
	}

	case KEYINPUT_ATTACK: {

		sc_atk *p = reinterpret_cast<sc_atk*>(buf);

		// 내가 맞았다면.. 나의 hp 를 깎자.
		if (m_player.id == p->under_attack_id) {
			m_player.state.hp = p->hp;
			break;
		}

		// 내가 아닌 딴놈이 맞은거라면, 다른 놈의 hp 를 깎도록 하자.
		m_other_players[p->under_attack_id].state.hp = p->hp;
		if (0 >= p->hp) {
			// 0 이하라면 지워버령
			m_other_players.erase(p->under_attack_id);
		}
		break;
	}

	default:	// 잘 안쓰이는 패킷들

		switch (buf[1])
		{
		case INIT_OTHER_CLIENT: {
			sc_other_init_info *p = reinterpret_cast<sc_other_init_info *>(buf);
			m_other_players[p->playerData.id] = p->playerData;
			break;
		}
		case PLAYER_DISCONNECTED: {
			sc_disconnect *p = reinterpret_cast<sc_disconnect*>(buf);
			m_other_players.erase(p->id);
			break;
		}
		case INIT_CLIENT: {
			sc_client_init_info *p = reinterpret_cast<sc_client_init_info *>(buf);
			m_player = p->player_info;
			break;
		}
		case TEST:
#ifdef _DEBUG
			cout << "Server is Running. TEST Packet Recived Successfully.\n";
#endif
			break;
		default:
			break;
		}

		// default break;
		break;
	}

	InvalidateRect(m_hWnd, NULL, TRUE);
}
