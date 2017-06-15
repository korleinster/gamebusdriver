#pragma once
#include"stdafx.h"
#include"../Server_Code/ClientClass.h"
#include "ObjMgr.h"
#include "Obj.h"
#include "RenderMgr.h"
#include "OtherPlayer.h"
#include "SceneMgr.h"
#include "Player.h"
#include "Monster.h"
#include "DamageFont.h"
#include "Info.h"
#include "MobHpBasic.h"
#include "../Include/MobHpBar.h"

void AsynchronousClientClass::processPacket(Packet* buf)
{

	switch (buf[1])
	{
	case CHANGED_POSITION: {
		sc_move *p = reinterpret_cast<sc_move*>(buf);
		//cout << "moved ID : " << p->id << ", position " << p->pos.x << ", " << p->pos.y << endl;;
		
		if (CSceneMgr::GetInstance()->GetScene() != SCENE_LOGO)
		{
			player_data *data = NULL;

			if (p->id >= MAX_AI_NUM)
				data = CObjMgr::GetInstance()->Get_PlayerServerData(p->id);
			else
				data = CObjMgr::GetInstance()->Get_MonsterServerData(p->id);

			if (nullptr != data)
			{
				data->pos = p->pos;

				if (p->id < MAX_AI_NUM)
				{
					for (auto iter : *CObjMgr::GetInstance()->Get_ObjList(L"Monster"))
					{
						if (iter->GetPacketData()->id == p->id) {
							if (((CMonster*)iter)->GetAniState() == MONSTER_IDLE) {
								((CMonster*)iter)->m_bKey = true;
								((CMonster*)iter)->SetAniState(MONSTER_MOVE);
							}
							break;
						}
					}
				}

				else
				{
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

			}
			else { break; }
		}
		break;
	}
	case CHANGED_DIRECTION: {
		sc_dir *p = reinterpret_cast<sc_dir*>(buf);
		if (CSceneMgr::GetInstance()->GetScene() != SCENE_LOGO)
		{
			player_data *data = nullptr;
			if (p->id >= MAX_AI_NUM)
				data = CObjMgr::GetInstance()->Get_PlayerServerData(p->id);
			else
				data = CObjMgr::GetInstance()->Get_MonsterServerData(p->id);

			if (nullptr != data) { data->dir = p->dir; }
			else { break; }
		}
		break;
	}

	case CHANGED_FEVER: {
		sc_fever *p = reinterpret_cast<sc_fever*>(buf);
		//cout << "내 Fever Gauge 현재 양 = " << p->gauge << "\n";
		m_player.state.gauge = p->gauge;
		(*(CObjMgr::GetInstance()->Get_ObjList(L"Player")->begin()))->SetPacketFever(&p->gauge);

		break;
	}

	case SERVER_MESSAGE_HP_CHANGED: {
		sc_hp *p = reinterpret_cast<sc_hp*>(buf);

		cout << "[ " << p->id << " ] 변경된 HP = " << p->hp << "\n";

		if (CSceneMgr::GetInstance()->GetScene() != SCENE_LOGO)
		{
			if (m_player.id == p->id) {
				m_player.state.hp = p->hp;
				(*(CObjMgr::GetInstance()->Get_ObjList(L"Player")->begin()))->SetPacketHp(&p->hp);
				break;
			}

			// 내가 아니라면 다른애 hp 깎기
			if (NULL != (CObjMgr::GetInstance()->Get_PlayerServerData(p->id))) {
				(CObjMgr::GetInstance()->Get_PlayerServerData(p->id))->state.hp = p->hp;
			}
		}
		break;
	}

	case KEYINPUT_ATTACK: {
		sc_atk *p = reinterpret_cast<sc_atk*>(buf);

		// 갑자기 왜 여기서 다른 플레이어 공격 패킷이 안날아 오는지 알 수가 없다 ( 한번 꼭 확인해 보자 *************************** )
		cout << "[ " << p->attacking_id << " ] -> [ " << p->under_attack_id << " ] 를 공격함. 현재 HP = " << p->hp << "\n";

		if (CSceneMgr::GetInstance()->GetScene() != SCENE_LOGO)
		{
			// 먼저 어떤 놈이 칼질을 했는지 아이디 확인 후, 그 녀석 애니메이션 편집 ( 내 아이디가 아니면 일단 애니메이션 작동 시켜야 됨 )
			/// 만약 이 위치에 내 고유 id 가 들어있다면, 이미 키를 누른 시점에서 애니메이션을 재생했기 때문에, 그냥 if 문을 넘어간다.
			if (m_player.id != p->attacking_id) {

				if (p->attacking_id < MAX_AI_NUM)
				{
					for (auto iter : *CObjMgr::GetInstance()->Get_ObjList(L"Monster"))
					{
						if (iter->GetPacketData()->id == p->attacking_id) {
							if ((reinterpret_cast<CMonster*>(iter))->GetAniState() == MONSTER_IDLE)
							{
								(reinterpret_cast<CMonster*>(iter))->m_bKey = true;
								(reinterpret_cast<CMonster*>(iter))->SetAniState(MONSTER_ATT);
							}
							break;
						}
					}
				}

				else
				{
					for (auto iter : *CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer"))
					{
						if (iter->GetPacketData()->id == p->attacking_id) {
							if ((reinterpret_cast<COtherPlayer*>(iter))->GetAniState() == PLAYER_IDLE)
							{
								(reinterpret_cast<COtherPlayer*>(iter))->m_bKey = true;
								(reinterpret_cast<COtherPlayer*>(iter))->SetAniState(PLAYER_ATT1);
							}
							break;
						}
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

			if (p->under_attack_id >= MAX_AI_NUM) // 플레이어일시
			{
				if (NULL != (CObjMgr::GetInstance()->Get_PlayerServerData(p->under_attack_id))) {
					(CObjMgr::GetInstance()->Get_PlayerServerData(p->under_attack_id))->state.hp = p->hp;

					/*CObj* pObj = CDamageFont::Create();
					pObj->SetPos(D3DXVECTOR3(m_player.pos.x, 1.f, m_player.pos.y));
					pObj->SetPacketData(&p->playerData);
					CObjMgr::GetInstance()->AddObject(L"OtherPlayer", pObj);
					CRenderMgr::GetInstance()->AddRenderGroup(TYPE_NONEALPHA, pObj);*/
				}
			}
			else // 몬스터일시
			{
				if (NULL != (CObjMgr::GetInstance()->Get_MonsterServerData(p->under_attack_id))) {
					(CObjMgr::GetInstance()->Get_MonsterServerData(p->under_attack_id))->state.hp = p->hp;


					//////////////데미지 폰트////////////////////////
					list<CObj*>* ListObj = CObjMgr::GetInstance()->Get_ObjList(L"Monster");
					list<CObj*>::iterator iter = ListObj->begin();
					list<CObj*>::iterator iter_end = ListObj->end();
					D3DXVECTOR3 vPos;

					for (; iter != iter_end; ++iter)
					{
						if ((*iter)->GetPacketData()->id == p->under_attack_id)
						{
							vPos = (*iter)->GetInfo()->m_vPos;
							break;
						}
					}

					CObj* pObj = CDamageFont::Create(&vPos, 105.f); //샘플값 105. 나중에 데미지는 서버에서 받아와서 출력.
					CObjMgr::GetInstance()->AddObject(L"DamageFont", pObj);
					///////////////////////////////////////////////////

					/////////////////체력바///////////////////////////

					int iSize = 0;
					//몹의 체력바가 없으면 만든다.
					if (m_bAttackFirst == false)//최초
					{
						iSize = 0;
						m_bAttackFirst = true;
					}
					else
						iSize = ((CObjMgr::GetInstance()->Get_ObjList(L"MobHpBaisic")->size()));

					if (iSize == 0)
					{
						CObj * pObj = CMobHpBar::Create();
						dynamic_cast<CMobHpBar*>(pObj)->SetRendHp(p->hp, 100); // 최대체력이 생기면 바꿔주자.
						CObjMgr::GetInstance()->AddObject(L"MobHpBar", pObj);

						pObj = CMobHpBasic::Create();
						dynamic_cast<CMobHpBasic*>(pObj)->SetName(L"Monster", p->under_attack_id);
						CObjMgr::GetInstance()->AddObject(L"MobHpBaisic",pObj);			
					}
					//있다면 가져와서 이름과 체력을 갱신하고 랜더타임도 초기화한다.
					else
					{
						CObj* pObj = (*(CObjMgr::GetInstance()->Get_ObjList(L"MobHpBar")->begin()));
						dynamic_cast<CMobHpBar*>(pObj)->SetRendHp(p->hp, 100);
						dynamic_cast<CMobHpBar*>(pObj)->ResetRendTime();

						pObj = (*(CObjMgr::GetInstance()->Get_ObjList(L"MobHpBaisic")->begin()));
						dynamic_cast<CMobHpBasic*>(pObj)->SetName(L"Monster", p->under_attack_id);
						dynamic_cast<CMobHpBasic*>(pObj)->ResetRendTime();
					}
					//////////////////////////////////////////////////

				}
			}

			if (0 >= p->hp)
			{
				if (p->under_attack_id < MAX_AI_NUM)
				{
					list<CObj*>* ListObj = CObjMgr::GetInstance()->Get_ObjList(L"Monster");
					list<CObj*>::iterator iter = ListObj->begin();
					list<CObj*>::iterator iter_end = ListObj->end();

					for (; iter != iter_end; ++iter)
					{
						if ((*iter)->GetPacketData()->id == p->under_attack_id)
						{
							CRenderMgr::GetInstance()->DelRenderGroup(TYPE_NONEALPHA, *iter);
							(*iter)->m_bDeath = true;
							break;
						}
					}


					//몹이 죽으면 체력바도 없에자.
					int iSize = 0;
					iSize = ((CObjMgr::GetInstance()->Get_ObjList(L"MobHpBaisic")->size()));
					if (iSize != 0)
					{
						//직접지우면 뭔일 터질지도 모르니 그냥 랜더시간을 초과시켜서 자연스럽게 없에버리자.
						CObj* pObj = (*(CObjMgr::GetInstance()->Get_ObjList(L"MobHpBar")->begin()));
						dynamic_cast<CMobHpBar*>(pObj)->m_fRendTime = 6.f;

						pObj = (*(CObjMgr::GetInstance()->Get_ObjList(L"MobHpBaisic")->begin()));
						dynamic_cast<CMobHpBasic*>(pObj)->m_fRendTime = 6.f;
					}

				}

				else
				{
					list<CObj*>* ListObj = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer");
					list<CObj*>::iterator iter = ListObj->begin();
					list<CObj*>::iterator iter_end = ListObj->end();

					for (; iter != iter_end; ++iter)
					{
						if ((*iter)->GetPacketData()->id == p->under_attack_id)
						{
							CRenderMgr::GetInstance()->DelRenderGroup(TYPE_NONEALPHA, *iter);
							(*iter)->m_bDeath = true;
							break;
						}
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
			//플레이어 최초 갱신 혹은 죽었다 부활했을시

			m_player = reinterpret_cast<sc_client_init_info *>(buf)->player_info;
			cout << "Init ID : " << m_player.id << "\n";
			(*CObjMgr::GetInstance()->Get_ObjList(L"Player")->begin())->SetPos(D3DXVECTOR3(m_player.pos.x, 0.f, m_player.pos.y));
			(*CObjMgr::GetInstance()->Get_ObjList(L"Player")->begin())->GetPacketData()->state.hp = m_player.state.hp;
			break;
		}
		case INIT_OTHER_CLIENT: {
			//다른플레이어 or NPC or 몬스터 최초갱신 혹은 부활
			sc_other_init_info *p = reinterpret_cast<sc_other_init_info *>(buf);
			cout << "init other ID : " << p->playerData.id << endl;

			player_data *data = CObjMgr::GetInstance()->Get_PlayerServerData(p->playerData.id);

			if (nullptr != data) { break; }
			else {

				if(p->playerData.id < MAX_AI_NUM)
				{ 
					//cout << "몬스터 생성" << endl;

					CObj* pObj = CMonster::Create();
					pObj->SetPos(D3DXVECTOR3(m_player.pos.x, 1.f, m_player.pos.y));
					pObj->SetPacketData(&p->playerData);
					CObjMgr::GetInstance()->AddObject(L"Monster", pObj);
					CRenderMgr::GetInstance()->AddRenderGroup(TYPE_NONEALPHA, pObj);

					//cout << "현재 몬스터의 개수 :" << CObjMgr::GetInstance()->m_mapObj[L"Monster"].size() << endl;
				}
				else
				{
					CObj* pObj = COtherPlayer::Create();
					pObj->SetPos(D3DXVECTOR3(m_player.pos.x, 1.f, m_player.pos.y));
					pObj->SetPacketData(&p->playerData);
					CObjMgr::GetInstance()->AddObject(L"OtherPlayer", pObj);
					CRenderMgr::GetInstance()->AddRenderGroup(TYPE_NONEALPHA, pObj);
				}
			}
			break;
		}
		case PLAYER_DISCONNECTED: {
			sc_disconnect *p = reinterpret_cast<sc_disconnect*>(buf);
			cout << "disconneced ID : " << p->id << endl;

			if (p->id == (*CObjMgr::GetInstance()->Get_ObjList(L"Player")->begin())->GetPacketData()->id) {
				// 플레이어가 죽었을시
				(*CObjMgr::GetInstance()->Get_ObjList(L"Player")->begin())->m_bDeath = true;


				break;
			}

			if (p->id < MAX_AI_NUM)
			{
				list<CObj*>::iterator iter = CObjMgr::GetInstance()->Get_ObjList(L"Monster")->begin();
				list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->Get_ObjList(L"Monster")->end();
				//cout << "몬스터 삭제" << endl;

				for (; iter != iter_end; ++iter)
				{
					//NPC 혹은 몬스터가 죽었을시
					if ((*iter)->GetPacketData()->id == p->id)
					{
						CRenderMgr::GetInstance()->DelRenderGroup(TYPE_NONEALPHA, *iter);
						::Safe_Delete(*iter);
						iter = CObjMgr::GetInstance()->Get_ObjList(L"Monster")->erase(iter);
						break;
					}
				}

				//cout << "현재 몬스터의 개수 :" << CObjMgr::GetInstance()->m_mapObj[L"Monster"].size() << endl;
			}
			
			else
			{
				list<CObj*>::iterator iter = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->begin();
				list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->end();

				for (; iter != iter_end; ++iter)
				{
					//다른플레이어가 죽었을시
					if ((*iter)->GetPacketData()->id == p->id)
					{
						CRenderMgr::GetInstance()->DelRenderGroup(TYPE_NONEALPHA, *iter);
						::Safe_Delete(*iter);
						iter = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->erase(iter);
						break;
					}
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
