#pragma once
#include"stdafx.h"
#include"../../../../server/serverBoostModel/serverBoostModel/protocol.h"
#include"../Server_Code/ClientClass.h"
#include "ObjMgr.h"
#include "Obj.h"
#include "RenderMgr.h"
#include "OtherPlayer.h"
#include "SceneMgr.h"

void AsynchronousClientClass::processPacket()
{
	switch (m_recvbuf[1])
	{
	case TEST:
#ifdef _DEBUG
		// 이건 테스트 용이니까 그냥 냅두면 되고 ㅇㅇ
		cout << "Server is Running. TEST Packet Recived Successfully.\n";
#endif
		break;
	case INIT_CLIENT: {
		m_player = *(reinterpret_cast<player_data*>(&m_recvbuf[2]));
		// 여기는 주체가 되는, 내가 직접 조작을 하는 플레이어의 데이터가 들어오니까
		// 아이템이라던가 위치정보라던가 일단 다 여기로 들어온다잉
	}
		break;
	case PLAYER_DISCONNECTED: {
		// 여기도, id 로 찾아서, 그 아이디 찾으면 그 객체는 삭제 해야겠찌? ㅇㅋㄷㅋ?
		player_data *data = CObjMgr::GetInstance()->Get_PlayerServerData(reinterpret_cast<player_data*>(&m_recvbuf[2])->id);

		//CObjMgr::GetInstance()->Get_ObjList()
	}
		break;
	case KEYINPUT: {

		if (CSceneMgr::GetInstance()->GetScene() != SCENE_LOGO)
		{
			player_data *data = CObjMgr::GetInstance()->Get_PlayerServerData(reinterpret_cast<player_data*>(&m_recvbuf[2])->id);

			if (nullptr != data) { memcpy(data, &m_recvbuf[2], m_recvbuf[0]); }
			else {
				// 데이터 추가 해줘야 됨 이해 했음?
				// 저기 data 가 만약 end 위치라면, 데이터가 없는데 넣으려고 하는거잖아
				// 그럼 새로운 플레이어가 감지되었다는 뜻이니까
				// 새로 객체 추가해서 저 값을 넣어줘야겠찌? ㅇㅋ?

				CObj* pObj = NULL;
				pObj = COtherPlayer::Create();
				pObj->SetPos(D3DXVECTOR3(m_player.pos.x, 1.f, m_player.pos.y));
				pObj->SetPacketData(reinterpret_cast<player_data*>(&m_recvbuf[2]));
				CObjMgr::GetInstance()->AddObject(L"OtherPlayer", pObj);
				CRenderMgr::GetInstance()->AddRenderGroup(TYPE_NONEALPHA, pObj);

			}
		}
	}
		break;
	default:
		break;
	}
}
