#pragma once
#include"stdafx.h"
#include"../../../../server/serverBoostModel/serverBoostModel/protocol.h"
#include"../Server_Code/ClientClass.h"
#include "ObjMgr.h"
#include "Obj.h"
#include "RenderMgr.h"
#include "OtherPlayer.h"
#include "SceneMgr.h"
#include "Player.h"

void AsynchronousClientClass::processPacket(Packet* buf)
{
	switch (buf[1])
	{
	case CHANGED_POSITION: {

		if (CSceneMgr::GetInstance()->GetScene() != SCENE_LOGO)
		{
			player_data *data = CObjMgr::GetInstance()->Get_PlayerServerData(*reinterpret_cast<unsigned int*>(&buf[sizeof(position)+2]));

			if (nullptr != data) 
			{ 
				memcpy(&data->pos, &buf[2], sizeof(position));

				list<CObj*>::iterator iter = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->begin();
				list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->end();

				for (iter; iter != iter_end;)
				{
					if ((*iter)->GetPacketData()->id == reinterpret_cast<player_data*>(data)->id)
					{
						if (((COtherPlayer*)(*iter))->GetAniState() == PLAYER_IDLE)
							((COtherPlayer*)(*iter))->SetAniState(PLAYER_MOVE);
						break;
					}
					else
						++iter;

				}
			}
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

	case KEYINPUT_ATTACK: {
		if (CSceneMgr::GetInstance()->GetScene() != SCENE_LOGO)
		{
			// 먼저 어떤 놈이 칼질을 했는지 아이디 확인 후, 그 녀석 애니메이션 편집 ( 내 아이디가 아니면 일단 애니메이션 작동 시켜야 됨 )
			/// 만약 이 위치에 내 고유 id 가 들어있다면, 이미 키를 누른 시점에서 애니메이션을 재생했기 때문에, 그냥 if 문을 넘어간다.
			if (m_player.id != *(reinterpret_cast<UINT*>(&buf[sizeof(int) + sizeof(int) + 2]))) {

				player_data *data = CObjMgr::GetInstance()->Get_PlayerServerData(*reinterpret_cast<unsigned int*>(&buf[sizeof(int) + sizeof(int) + 2]));

				list<CObj*>::iterator iter = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->begin();
				list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->end();

				for (iter; iter != iter_end;)
				{
					if ((*iter)->GetPacketData()->id == reinterpret_cast<player_data*>(data)->id)
					{
						if(((COtherPlayer*)(*iter))->GetAniState()==PLAYER_IDLE)
							((COtherPlayer*)(*iter))->SetAniState(PLAYER_ATT1);
						break;
					}
					else
						++iter;

				}

				// *(reinterpret_cast<UINT*>(&buf[sizeof(int) + sizeof(int) + 2])) 이 번호 클라이언트의 애니메이션을 작동시켜야 한다.
				/// 통신을 하던 도중, 추후 시야에서 사라져, 데이터에서 제거한 상황이 있을 수 있기때문에, 벌써 사라져 없는 클라이언트에 대한 예외 처리도 해주어야 한다.
				/// 추후 더 효과적으로 하기 위해서는, 애초에 카메라 밖에 위치한 클라이언트에 대해선 처리를 안하는게 가장 좋겠지...?
				
			}

			// 내가 피해를 입은 것이라면, 내 hp 를 깎고 break;
			/// -> 정확히는 hp 전체 총량 값이다. 헷갈리지 않도록 유의
			/// 다시말하면, 이전 hp 값과, 패킷으로 날아들어온 hp 값을 차이를 비교해서 데미지가 얼마 들어갔는지 표시해주어야 한다.
			/// 원하면, 깎이는 데미지가 얼마인지만 패킷으로 보내 줄 수도 있음.
			if (m_player.id == *(reinterpret_cast<UINT*>(&buf[sizeof(int) + 2]))) {
				m_player.state.hp = *(reinterpret_cast<int*>(&buf[2]));
				list<CObj*>::iterator iter = CObjMgr::GetInstance()->Get_ObjList(L"Player")->begin();
				(*iter)->SetPacketData(&m_player);

				break;
			}

			// 내가 아니라면 다른애 hp 깎기
			player_data *data = CObjMgr::GetInstance()->Get_PlayerServerData(*reinterpret_cast<unsigned int*>(&buf[sizeof(int) + 2]));
			data->state.hp = *(reinterpret_cast<int*>(&buf[2]));			

		}
	}
		 break;


	default:	// 잘 안쓰이는 패킷들

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
		default:
			break;
		}


				// default break;
		break;
	}
}
