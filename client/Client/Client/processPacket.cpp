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
#if 0
		cout << "Client was INITED\n";
#endif
		m_player = *(reinterpret_cast<player_data*>(&buf[2]));
	}
		break;
	case PLAYER_DISCONNECTED: {
#if 0
		cout << "Player " << reinterpret_cast<player_data*>(&buf[2])->id << " is Disconnected\n";
#endif // _DEBUG
		unordered_map<UINT, player_data>::iterator ptr = m_other_players.find(reinterpret_cast<player_data*>(&buf[2])->id);

		if (m_other_players.end() != ptr) { m_other_players.erase(ptr->first); }

		InvalidateRect(m_hWnd, NULL, TRUE);
	}
		break;
	case CHANGED_POSITION: {
#if 0
		cout << "CHANGED_POSITION , player id : " << reinterpret_cast<player_data*>(&buf[2])->id << endl;
#endif // _DEBUG
		unordered_map<UINT, player_data>::iterator ptr = m_other_players.find(reinterpret_cast<player_data*>(&buf[2])->id);

		if (m_other_players.end() == ptr) { m_other_players.insert(make_pair(reinterpret_cast<player_data*>(&buf[2])->id, *(reinterpret_cast<player_data*>(&buf[2])))); }
		else { ptr->second.pos = reinterpret_cast<player_data*>(&buf[2])->pos; }

		InvalidateRect(m_hWnd, NULL, TRUE);
	}
		break;
	default:
		break;
	}
}
