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
		if (false == am_i_inited) { m_player = *(reinterpret_cast<player_data*>(&buf[2])); am_i_inited = true; }
		else {
			unordered_map<UINT, player_data>::iterator ptr = m_other_players.find(reinterpret_cast<player_data*>(&buf[2])->id);

			if (m_other_players.end() == ptr) { m_other_players.insert(make_pair(reinterpret_cast<player_data*>(&buf[2])->id, *(reinterpret_cast<player_data*>(&buf[2])))); }
			else { ptr->second.pos = reinterpret_cast<player_data*>(&buf[2])->pos; }

			sendPacket(sizeof(player_data), KEYINPUT, reinterpret_cast<BYTE*>(&m_player));
			InvalidateRect(m_hWnd, NULL, TRUE);
			
		}
	}
		break;
	case PLAYER_DISCONNECTED: {
		unordered_map<UINT, player_data>::iterator ptr = m_other_players.find(reinterpret_cast<player_data*>(&buf[2])->id);

		if (m_other_players.end() != ptr) { m_other_players.erase(ptr->first); }

		InvalidateRect(m_hWnd, NULL, TRUE);
	}
		break;
	case KEYINPUT: {
#ifdef _DEBUG
		cout << "KEYINPUT , player id : " << reinterpret_cast<player_data*>(&buf[2])->id << endl;
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
