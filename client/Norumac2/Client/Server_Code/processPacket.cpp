#pragma once
#include"stdafx.h"
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
		sc_move *p = reinterpret_cast<sc_move*>(buf);

		if (CSceneMgr::GetInstance()->GetScene() != SCENE_LOGO)
		{
			player_data *data = CObjMgr::GetInstance()->Get_PlayerServerData(p->id);

			if (nullptr != data)
			{
				data->pos = p->pos;

				for (auto iter : *CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer"))
				{
					if (iter->GetPacketData()->id == p->id) {
						if (((COtherPlayer*)iter)->GetAniState() == PLAYER_IDLE) {
							((COtherPlayer*)iter)->m_bKey = true;
							((COtherPlayer*)iter)->SetAniState(PLAYER_MOVE);
						}
						break;
					}
				}
			}
			else { break; }
		}
		break;
	}
	case CHANGED_DIRECTION: {
		sc_dir *p = reinterpret_cast<sc_dir*>(buf);

		if (CSceneMgr::GetInstance()->GetScene() != SCENE_LOGO)
		{
			player_data *data = CObjMgr::GetInstance()->Get_PlayerServerData(p->id);
			if (nullptr != data) { data->dir = p->dir; }
			else { break; }
		}
		break;
	}

	case SERVER_MESSAGE_HP_CHANGED: {
		sc_hp *p = reinterpret_cast<sc_hp*>(buf);

		if (CSceneMgr::GetInstance()->GetScene() != SCENE_LOGO)
		{
			if (m_player.id == p->id) {
				m_player.state.hp = p->hp;
				(*(CObjMgr::GetInstance()->Get_ObjList(L"Player")->begin()))->SetPacketHp(&p->hp);
				break;
			}

			// 내가 아니라면 다른애 hp 깎기
			(CObjMgr::GetInstance()->Get_PlayerServerData(p->id))->state.hp = p->hp;
		}
		break;
	}

	case KEYINPUT_ATTACK: {
		sc_atk *p = reinterpret_cast<sc_atk*>(buf);

		if (CSceneMgr::GetInstance()->GetScene() != SCENE_LOGO)
		{
			// 먼저 어떤 놈이 칼질을 했는지 아이디 확인 후, 그 녀석 애니메이션 편집 ( 내 아이디가 아니면 일단 애니메이션 작동 시켜야 됨 )
			/// 만약 이 위치에 내 고유 id 가 들어있다면, 이미 키를 누른 시점에서 애니메이션을 재생했기 때문에, 그냥 if 문을 넘어간다.
			if (m_player.id != p->attacking_id) {

				for (auto iter : *CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")) {
					if (iter->GetPacketData()->id == p->attacking_id) {
						if ((reinterpret_cast<COtherPlayer*>(iter))->GetAniState() == PLAYER_IDLE)
						{
							(reinterpret_cast<COtherPlayer*>(iter))->m_bKey = true;
							(reinterpret_cast<COtherPlayer*>(iter))->SetAniState(PLAYER_ATT1);
						}
						break;
					}
				}

				// *(reinterpret_cast<UINT*>(&buf[sizeof(int) + sizeof(int) + 2])) 이 번호 클라이언트의 애니메이션을 작동시켜야 한다.
				/// 통신을 하던 도중, 추후 시야에서 사라져, 데이터에서 제거한 상황이 있을 수 있기때문에, 벌써 사라져 없는 클라이언트에 대한 예외 처리도 해주어야 한다.
				/// 추후 더 효과적으로 하기 위해서는, 애초에 카메라 밖에 위치한 클라이언트에 대해선 처리를 안하는게 가장 좋겠지...?

			}

			// 내가 피해를 입은 것이라면, 내 hp 를 깎고 break;
			/// -> 정확히는 hp 전체 총량 값이다. 헷갈리지 않도록 유의
			/// 다시말하면, 이전 hp 값과, 패킷으로 날아들어온 hp 값을 차이를 비교해서 데미지가 얼마 들어갔는지 표시해주어야 한다.
			/// 원하면, 깎이는 데미지가 얼마인지만 패킷으로 보내 줄 수도 있음.
			if (m_player.id == p->under_attack_id) {
				m_player.state.hp = p->hp;
				(*CObjMgr::GetInstance()->Get_ObjList(L"Player")->begin())->SetPacketHp(&p->hp);
				break;
			}

			// 내가 아니라면 다른애 hp 깎기
			CObjMgr::GetInstance()->Get_PlayerServerData(p->under_attack_id)->state.hp = p->hp;

			if (0 >= p->hp)
			{
				list<CObj*>* ListObj = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer");
				list<CObj*>::iterator iter = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->begin();
				list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->end();

				for (iter; iter != iter_end;)
				{
					if ((*iter)->GetPacketData()->id == p->under_attack_id)
					{
						CRenderMgr::GetInstance()->DelRenderGroup(TYPE_NONEALPHA, *iter);
						::Safe_Delete(*iter);
						iter = ListObj->erase(iter);
						break;
					}
				}
			}

		}
		break;
	}

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
			m_player = reinterpret_cast<sc_client_init_info *>(buf)->player_info;
			break;
		}
		case INIT_OTHER_CLIENT: {
			sc_other_init_info *p = reinterpret_cast<sc_other_init_info *>(buf);

			player_data *data = CObjMgr::GetInstance()->Get_PlayerServerData(p->playerData.id);

			if (nullptr != data) { break; }
			else {
				CObj* pObj = COtherPlayer::Create();
				pObj->SetPos(D3DXVECTOR3(m_player.pos.x, 1.f, m_player.pos.y));
				pObj->SetPacketData(&p->playerData);
				CObjMgr::GetInstance()->AddObject(L"OtherPlayer", pObj);
				CRenderMgr::GetInstance()->AddRenderGroup(TYPE_NONEALPHA, pObj);
			}
			break;
		}
		case PLAYER_DISCONNECTED: {
			sc_disconnect *p = reinterpret_cast<sc_disconnect*>(buf);
			
			list<CObj*>::iterator iter = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->begin();
			list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->end();

			for (iter; iter != iter_end;)
			{
				if ((*iter)->GetPacketData()->id == p->id)
				{
					CRenderMgr::GetInstance()->DelRenderGroup(TYPE_NONEALPHA, *iter);
					::Safe_Delete(*iter);
					iter = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->erase(iter);
					break;
				}
			}
			break;
		}
		default:
			break;
		}

		// default break;
		break;
	}
}
