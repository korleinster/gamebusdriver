#pragma once
#include"stdafx.h"
#include"../../../../server/serverBoostModel/serverBoostModel/protocol.h"
#include"../Server_Code/ClientClass.h"

void AsynchronousClientClass::processPacket()
{
	switch (m_recvbuf[1])
	{
	case TEST:
#ifdef _DEBUG
		cout << "Server is Running. TEST Packet Recived Successfully.\n";
#endif
		break;
	case INIT_CLIENT: {
		m_player = *(reinterpret_cast<player_data*>(&m_recvbuf[2]));
	}
		break;
	case PLAYER_DISCONNECTED: {
		unordered_map<UINT, player_data>::iterator ptr = m_other_players.find(reinterpret_cast<player_data*>(&m_recvbuf[2])->id);

		if (m_other_players.end() != ptr) { m_other_players.erase(ptr->first); }

		InvalidateRect(m_hWnd, NULL, TRUE);
	}
							  break;
	case KEYINPUT: {
#ifdef _DEBUG
		cout << "KEYINPUT , player id : " << reinterpret_cast<player_data*>(&m_recvbuf[2])->id << endl;
#endif // _DEBUG
		unordered_map<UINT, player_data>::iterator ptr = m_other_players.find(reinterpret_cast<player_data*>(&m_recvbuf[2])->id);

		if (m_other_players.end() == ptr) { m_other_players.insert(make_pair(reinterpret_cast<player_data*>(&m_recvbuf[2])->id, *(reinterpret_cast<player_data*>(&m_recvbuf[2])))); }
		else { ptr->second.pos = reinterpret_cast<player_data*>(&m_recvbuf[2])->pos; }

		InvalidateRect(m_hWnd, NULL, TRUE);
	}
		break;
	default:
		break;
	}
}
