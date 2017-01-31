#include"stdafx.h"

void AsynchronousClientClass::processPacket(Packet *buf)
{
	switch (buf[1])
	{
	case TEST:
#ifdef _DEBUG
		cout << "Server is Running. TEST Packet Recived Successfully.\n";
#endif
		break;
	case INIT_CLIENT: {
		m_player = *(reinterpret_cast<player_data*>(&buf[2]));
	}
		break;
	case INIT_OTHER_CLIENT: {
		unordered_map<UINT, player_data>::iterator ptr = m_other_players.find(reinterpret_cast<player_data*>(&buf[2])->id);
		if (m_other_players.end() == ptr) { m_other_players.insert(make_pair(reinterpret_cast<player_data*>(&buf[2])->id, *reinterpret_cast<player_data*>(&buf[2]))); }
		else { ptr->second.pos = *reinterpret_cast<position*>(&buf[2]); }

		InvalidateRect(m_hWnd, NULL, TRUE);
	}
		break;

	case PLAYER_DISCONNECTED: {
		unordered_map<UINT, player_data>::iterator ptr = m_other_players.find(reinterpret_cast<player_data*>(&buf[2])->id);
		if (m_other_players.end() != ptr) { m_other_players.erase(ptr->first); }
		InvalidateRect(m_hWnd, NULL, TRUE);
	}
		break;

	case CHANGED_POSITION: {
		unordered_map<UINT, player_data>::iterator ptr = m_other_players.find(*reinterpret_cast<UINT*>(&buf[sizeof(position) + 2]));
		if (m_other_players.end() == ptr) {
			player_data temp;
			temp.id = *reinterpret_cast<UINT*>(&buf[sizeof(position) + 2]);
			temp.pos = *reinterpret_cast<position*>(&buf[2]);
			m_other_players.insert(make_pair(temp.id, temp));
		}
		else { ptr->second.pos = *reinterpret_cast<position*>(&buf[2]); }

		InvalidateRect(m_hWnd, NULL, TRUE);
	}
		break;

	case CHANGED_DIRECTION: {
		unordered_map<UINT, player_data>::iterator ptr = m_other_players.find(*(reinterpret_cast<UINT*>(&buf[sizeof(char) + 2])));
		//if(m_other_players.end() != ptr){ ptr->second.dir = *(reinterpret_cast<char*>(&buf[2])); }
		ptr->second.dir = *(reinterpret_cast<char*>(&buf[2]));

		InvalidateRect(m_hWnd, NULL, TRUE);
	}
		break;
	default:
		break;
	}
}
