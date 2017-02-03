#pragma once
#include"stdafx.h"
#include"../../../../server/serverBoostModel/serverBoostModel/protocol.h"
#include"../Server_Code/ClientClass.h"
#include "ObjMgr.h"
#include "Obj.h"
#include "RenderMgr.h"
#include "OtherPlayer.h"
#include "SceneMgr.h"

void AsynchronousClientClass::processPacket(Packet* buf)
{
	switch (buf[1])
	{
	case TEST:
#ifdef _DEBUG
		// 이건 테스트 용이니까 그냥 냅두면 되고 ㅇㅇ
		cout << "Server is Running. TEST Packet Recived Successfully.\n";
#endif
		break;
	case INIT_CLIENT: {
		m_player = *(reinterpret_cast<player_data*>(&buf[2]));
		// 여기는 주체가 되는, 내가 직접 조작을 하는 플레이어의 데이터가 들어오니까
		// 아이템이라던가 위치정보라던가 일단 다 여기로 들어온다잉
	}
		break;
	case INIT_OTHER_CLIENT: {
		/*unordered_map<UINT, player_data>::iterator ptr = m_other_players.find(reinterpret_cast<player_data*>(&buf[2])->id);
		if (m_other_players.end() == ptr) { m_other_players.insert(make_pair(reinterpret_cast<player_data*>(&buf[2])->id, *reinterpret_cast<player_data*>(&buf[2]))); }
		else { ptr->second.pos = *reinterpret_cast<position*>(&buf[2]); }

		InvalidateRect(m_hWnd, NULL, TRUE);*/
		//if (CSceneMgr::GetInstance()->GetScene() != SCENE_LOGO)
		{
			player_data *data = CObjMgr::GetInstance()->Get_PlayerServerData(reinterpret_cast<player_data*>(&buf[2])->id);

			if (nullptr != data) { break; }
			else {
				// 데이터 추가 해줘야 됨 이해 했음?
				// 저기 data 가 만약 end 위치라면, 데이터가 없는데 넣으려고 하는거잖아
				// 그럼 새로운 플레이어가 감지되었다는 뜻이니까
				// 새로 객체 추가해서 저 값을 넣어줘야겠찌? ㅇㅋ?

				CObj* pObj = NULL;
				pObj = COtherPlayer::Create();
				pObj->SetPos(D3DXVECTOR3(m_player.pos.x, 1.f, m_player.pos.y));
				pObj->SetPacketData(reinterpret_cast<player_data*>(&buf[2]));
				CObjMgr::GetInstance()->AddObject(L"OtherPlayer", pObj);
				CRenderMgr::GetInstance()->AddRenderGroup(TYPE_NONEALPHA, pObj);

			}
		}
	}
		break;
	case PLAYER_DISCONNECTED: {
		// 여기도, id 로 찾아서, 그 아이디 찾으면 그 객체는 삭제 해야겠찌? ㅇㅋㄷㅋ?
		player_data *data = CObjMgr::GetInstance()->Get_PlayerServerData(reinterpret_cast<player_data*>(&buf[2])->id);

		list<CObj*>::iterator iter = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->begin();
		list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->end();

		for (iter; iter != iter_end;)
		{
			if ((*iter)->GetPacketData()->id == reinterpret_cast<player_data*>(data)->id)
			{
				CRenderMgr::GetInstance()->DelRenderGroup(TYPE_NONEALPHA, *iter);
				::Safe_Release(*iter);
				CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->erase(iter++);
			}
			else
				++iter;

		}

	}
		break;
	case CHANGED_POSITION: {

		if (CSceneMgr::GetInstance()->GetScene() != SCENE_LOGO)
		{
			player_data *data = CObjMgr::GetInstance()->Get_PlayerServerData(*reinterpret_cast<unsigned int*>(&buf[sizeof(position)+2]));

			if (nullptr != data) { memcpy(&data->pos, &buf[2], sizeof(player_data)); }
			else
				break;
		}
	}
		break;
	case CHANGED_DIRECTION: {

		if (CSceneMgr::GetInstance()->GetScene() != SCENE_LOGO)
		{
			player_data *data = CObjMgr::GetInstance()->Get_PlayerServerData(*reinterpret_cast<unsigned int*>(&buf[sizeof(char)+2]));

			if (nullptr != data) { memcpy(&data->dir, &buf[2], sizeof(char)); }
			else
				break;
		}
	}
		break;
	default:
		break;
	}
}
