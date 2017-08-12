#include "stdafx.h"
#include "Player.h"
#include "StaticMesh.h"
#include "Texture.h"
#include "Info.h"
#include "VIBuffer.h"
#include "RcTex.h"
#include "ShaderMgr.h"
#include "Camera.h"
#include "ObjMgr.h"
#include "Shader.h"
#include "Terrain.h"
#include "TerrainCol.h"
#include "Device.h"
#include "Camera.h"
#include "DynamicMesh.h"
#include "Input.h"
#include "TimeMgr.h"
#include "RenderMgr.h"
#include "AnimationMgr.h"
#include "ResourcesMgr.h"
#include "OtherPlayer.h"
#include "NaviMgr.h"
#include "NPC.h"
#include "QuestUI.h"

#pragma pack(push,1)
struct CB_VS_PER_OBJECT
{
	D3DXMATRIX m_mWorldViewProjection;
	D3DXMATRIX m_mWorld;
};

struct CB_PS_PER_OBJECT
{
	float m_fSpecExp;
	float m_fSpecIntensity;
	D3DXVECTOR3 m_vEyePosition;
	float pad[3];
};
#pragma pack(pop)

CPlayer::CPlayer()
{
	m_pBuffer = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pShadowAniVertexShader = NULL;
	m_pShadowNonAniVertexShader = NULL;
	m_pTexture = NULL;
	m_pVerTex = NULL;
	m_pTerrainCol = NULL;
	m_fSeverTime = 0.f;
	//m_eObjDir = OBJDIR_UP;
	m_bSendServer = false;
	m_ePlayerState = PLAYER_IDLE;
	m_bPush = false;
	m_pSceneVertexShaderCB = NULL;
	m_pScenePixelShaderCB = NULL;
	m_bMoving = false;
	m_fPotionTime = 0.f;
	m_bPotionCool = false;
	m_bTpCool = false;
	m_fTpTime = 0.f;
	m_dwCellNum = 0;
	m_bStart = true;
	m_fComboTime = 0.f;
	ZeroMemory(&m_bCombo, sizeof(bool) * 2);
	m_bMoveSend = false;
	m_fSkillMoveTime = 0.f;
	m_bSkillUsed = false;
	g_fChatCool = 0.f;
	m_fKeyCool = 5.f;
	m_eQuestState = QUEST_NOT;
	m_iQuestStateMount = 0;

	m_Packet = new Packet[MAX_BUF_SIZE];
}


CPlayer::~CPlayer()
{
	CObj::Release();
	::Safe_Delete_Array(m_Packet);	
}

HRESULT CPlayer::Initialize(void)
{
	if (FAILED(AddComponent()))
		return E_FAIL;
	//m_pInfo->m_fAngle[ANGLE_X] = /*D3DX_PI / 2 * -1.f;*/D3DXToRadian(-90);
	//m_ServerInfo.pos.x = m_pInfo->m_vPos.x;
	//m_ServerInfo.pos.y = m_pInfo->m_vPos.z;
	//m_pInfo->m_vScale = D3DXVECTOR3(0.05f, 0.05f, 0.05f);
	m_pInfo->m_vScale = D3DXVECTOR3(1.f, 1.f, 1.f);
	m_fSpeed = 4.7;

	list<CObj*>::iterator iter = CObjMgr::GetInstance()->m_mapObj[L"Terrain"].begin();
	list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->m_mapObj[L"Terrain"].end();

	if (iter != iter_end)
		m_pVerTex = *dynamic_cast<CTerrain*>(*iter)->GetVertex();

	CRenderMgr::GetInstance()->AddRenderGroup(TYPE_NONEALPHA, this);
	m_dwTime = GetTickCount();

	D3D11_BUFFER_DESC cbDesc;
	ZeroMemory(&cbDesc, sizeof(cbDesc));
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	cbDesc.ByteWidth = sizeof(CB_VS_PER_OBJECT);
	FAILED_CHECK(m_pGrapicDevice->m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pSceneVertexShaderCB));

	cbDesc.ByteWidth = sizeof(CB_PS_PER_OBJECT);
	FAILED_CHECK(m_pGrapicDevice->m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pScenePixelShaderCB));

	cbDesc.ByteWidth = sizeof(D3DXMATRIX);
	FAILED_CHECK(m_pGrapicDevice->m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pCascadedShadowGenVertexCB));

	return S_OK;
}

int CPlayer::Update(void)
{
	//cout << "플레이어 좌표 ( " << m_pInfo->m_vPos.x << " , " << m_pInfo->m_vPos.y << " , " << m_pInfo->m_vPos.z << " )" << endl;
	if (m_bStart)
	{
		SetNaviIndex(CNaviMgr::GetInstance()->GetCellIndex(&m_pInfo->m_vPos));
		m_bStart = false;
	}
	if (m_bSendServer == false)
	{
		m_pInfo->m_ServerInfo = *g_client.getPlayerData();
		g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));
		g_client.sendPacket(sizeof(char), CHANGED_DIRECTION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.dir));
		m_bSendServer = true;
	}


	//m_pInfo->m_fAngle[1] += 0.1f;

	TimeSetter();


	if (CNaviMgr::GetInstance()->GetCellType(m_dwCellNum) == TYPE_TERRAIN)
		m_pTerrainCol->CollisionTerrain(&m_pInfo->m_vPos, m_pVerTex);




	D3DXVec3TransformNormal(&m_pInfo->m_vDir, &g_vLook, &m_pInfo->m_matWorld);


	if (m_bDeath == false)
	{
		if (CInput::GetInstance()->GetDIKeyState(DIK_RETURN) & 0x80)
		{
			if (g_bChatMode == false && g_fChatCool > 0.2f)
			{
				g_bChatMode = true;
				g_fChatCool = 0.f;
				m_ePlayerState = PLAYER_IDLE;
				m_bSkillUsed = false;
				m_fSkillMoveTime = 0.f;
			}
			else if (g_bChatMode == true && g_fChatCool > 0.2f && g_bChatEnd == true)
			{
				g_bChatMode = false;
				g_fChatCool = 0.f;
			}
		}

		if (g_bChatMode == false)
			KeyInput();

		AniMove();
		CObj::Update();

		//cout << m_bPush << endl;


		if (dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_bAniEnd != false && m_bMoving == false)
		{
			if (m_bPush == false && m_bMoving == false)
			{
				m_ePlayerState = PLAYER_IDLE;
				m_bSkillUsed = false;
				m_fSkillMoveTime = 0.f;
			}
		}

		if (m_pInfo->m_ServerInfo.state.hp <= 0)
		{
			m_bDeath = true;
			m_ePlayerState = PLAYER_DEAD;
			dynamic_cast<CDynamicMesh*>(m_pBuffer)->ResetPlayTimer();

		}
	}
	else if (m_bDeath == true)
	{
		if (m_pInfo->m_ServerInfo.state.hp > 0)
		{
			m_bDeath = false;
			m_ePlayerState = PLAYER_IDLE;
		}

	}

	//if (m_bPush == false)
	//{
	//	if (m_ePlayerState == PLAYER_MOVE)
	//		m_ePlayerState = PLAYER_IDLE;
	//	else if (m_ePlayerState != PLAYER_MOVE)
	//	{
	//		if (dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_bAniEnd != false)
	//			m_ePlayerState = PLAYER_IDLE;
	//	}
	//}

	/*if (m_bPush == false && m_ePlayerState != PLAYER_MOVE)
		m_ePlayerState = PLAYER_IDLE;*/

	if (CInput::GetInstance()->GetDIKeyState(DIK_LBRACKET) & 0x80)// "["키 
	{
		m_fSpeed = 4.7;
	}
	if (CInput::GetInstance()->GetDIKeyState(DIK_RBRACKET) & 0x80)// "]"키
	{
		m_fSpeed = 12.0;
	}



	return 0;
}

void CPlayer::Render(void)
{
	// Get the projection & view matrix from the camera class
	D3DXMATRIX mView = *(CCamera::GetInstance()->GetViewMatrix());
	D3DXMATRIX mProj = *(CCamera::GetInstance()->GetProjMatrix());
	D3DXMATRIX mWorldViewProjection = m_pInfo->m_matWorld * mView * mProj;

	// Set the constant buffers
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	FAILED_CHECK_RETURN(m_pGrapicDevice->m_pDeviceContext->Map(m_pSceneVertexShaderCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource), );
	CB_VS_PER_OBJECT* pVSPerObject = (CB_VS_PER_OBJECT*)MappedResource.pData;
	D3DXMatrixTranspose(&pVSPerObject->m_mWorldViewProjection, &mWorldViewProjection);
	D3DXMatrixTranspose(&pVSPerObject->m_mWorld, &m_pInfo->m_matWorld);
	m_pGrapicDevice->m_pDeviceContext->Unmap(m_pSceneVertexShaderCB, 0);
	m_pGrapicDevice->m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pSceneVertexShaderCB);

	FAILED_CHECK_RETURN(m_pGrapicDevice->m_pDeviceContext->Map(m_pScenePixelShaderCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource), );
	CB_PS_PER_OBJECT* pPSPerObject = (CB_PS_PER_OBJECT*)MappedResource.pData;
	pPSPerObject->m_vEyePosition = CCamera::GetInstance()->m_vEye;
	pPSPerObject->m_fSpecExp = 250.0f;
	pPSPerObject->m_fSpecIntensity = 0.25f;
	m_pGrapicDevice->m_pDeviceContext->Unmap(m_pScenePixelShaderCB, 0);
	m_pGrapicDevice->m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pScenePixelShaderCB);

	// Set the vertex layout
	m_pGrapicDevice->m_pDeviceContext->IASetInputLayout(m_pVertexShader->m_pVertexLayout);

	// Set the shaders
	m_pGrapicDevice->m_pDeviceContext->VSSetShader(m_pVertexShader->m_pVertexShader, NULL, 0);
	m_pGrapicDevice->m_pDeviceContext->PSSetShader(m_pPixelShader->m_pPixelShader, NULL, 0);

	m_pGrapicDevice->m_pDeviceContext->PSSetShaderResources(0, 1, &m_pTexture->m_pTextureRV);
	m_pGrapicDevice->m_pDeviceContext->PSSetSamplers(0, 1, &m_pTexture->m_pSamplerLinear);


	if (m_bDeath == false)
		dynamic_cast<CDynamicMesh*>(m_pBuffer)->PlayAnimation(m_ePlayerState, 40.f);
	else if (m_bDeath == true)
		dynamic_cast<CDynamicMesh*>(m_pBuffer)->PlayAnimationOnce(PLAYER_DEAD, 40.f);
}

void  CPlayer::ShadowmapRender(void)
{
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	FAILED_CHECK_RETURN(m_pGrapicDevice->m_pDeviceContext->Map(m_pCascadedShadowGenVertexCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource), );
	D3DXMATRIX* pVSPerObject = (D3DXMATRIX*)MappedResource.pData;
	D3DXMatrixTranspose(pVSPerObject, &m_pInfo->m_matWorld);
	m_pGrapicDevice->m_pDeviceContext->Unmap(m_pCascadedShadowGenVertexCB, 0);
	m_pGrapicDevice->m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pCascadedShadowGenVertexCB);

	m_pGrapicDevice->m_pDeviceContext->IASetInputLayout(m_pShadowAniVertexShader->m_pVertexLayout);
	m_pGrapicDevice->m_pDeviceContext->VSSetShader(m_pShadowAniVertexShader->m_pVertexShader, NULL, 0);

	if (m_bDeath == false)
		dynamic_cast<CDynamicMesh*>(m_pBuffer)->PlayAnimation(m_ePlayerState,40.f);
	else if (m_bDeath == true)
		dynamic_cast<CDynamicMesh*>(m_pBuffer)->PlayAnimationOnce(PLAYER_DEAD, 40.f);

	m_pGrapicDevice->m_pDeviceContext->IASetInputLayout(m_pShadowNonAniVertexShader->m_pVertexLayout);
	m_pGrapicDevice->m_pDeviceContext->VSSetShader(m_pShadowNonAniVertexShader->m_pVertexShader, NULL, 0);
}

CPlayer * CPlayer::Create(void)
{
	CPlayer* pObj = new CPlayer;
	if (FAILED(pObj->Initialize()))
		::Safe_Delete(pObj);

	return pObj;
}


HRESULT CPlayer::AddComponent(void)
{
	CComponent* pComponent = NULL;

	vecAniName = *(CAnimationMgr::GetInstance()->GetAnimaiton(L"Player"));

	//TransForm
	pComponent = m_pInfo = CInfo::Create(g_vLook);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Transform", pComponent));


	//DynamicMesh
	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Player");
	m_pBuffer = dynamic_cast<CDynamicMesh*>(pComponent);
	NULL_CHECK_RETURN(m_pBuffer, E_FAIL);
	//m_mapComponent.insert(map<wstring, CComponent*>::value_type(L"Mesh", pComponent));

	//Texture
	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Texture_Player");
	m_pTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(m_pTexture, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Texture", pComponent));

	pComponent = m_pVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"RenderSceneVS_ANI");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"VS_Shader", pComponent));
	pComponent = m_pPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"RenderScenePS");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"PS_Shader", pComponent));

	pComponent = m_pShadowAniVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"PointShandowGenVS_ANI");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"VS_Shader_ShadowAni", pComponent));
	pComponent = m_pShadowNonAniVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"PointShadowGenVS");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"VS_Shader_Shadow", pComponent));


	return S_OK;
}


void CPlayer::KeyInput()
{
	float		fTime = CTimeMgr::GetInstance()->GetTime();

	//g_client->m_recvbuf

	if (CInput::GetInstance()->GetDIKeyState(DIK_UP) & 0x80)
	{
		m_bPush = true;
		m_bMoveSend = true;
		if (CInput::GetInstance()->GetDIKeyState(DIK_LEFT) & 0x80)
		{
			m_pInfo->m_fAngle[ANGLE_Y] = D3DXToRadian(90.f);
			m_dwCellNum = CNaviMgr::GetInstance()->MoveOnNaviMesh(&m_pInfo->m_vPos, &(m_pInfo->m_vDir * m_fSpeed * fTime), m_dwCellNum);
			m_pInfo->m_ServerInfo.pos.x = m_pInfo->m_vPos.x;
			m_pInfo->m_ServerInfo.pos.y = m_pInfo->m_vPos.z;

			if (m_pInfo->m_ServerInfo.dir != (KEYINPUT_UP | KEYINPUT_LEFT)) {
				m_pInfo->m_ServerInfo.dir = (KEYINPUT_UP | KEYINPUT_LEFT);
				//g_client.sendPacket(sizeof(char), CHANGED_DIRECTION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.dir));
			}
			//g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));
		}
		else if (CInput::GetInstance()->GetDIKeyState(DIK_RIGHT) & 0x80)
		{
			m_pInfo->m_fAngle[ANGLE_Y] = D3DXToRadian(180.f);
			m_dwCellNum = CNaviMgr::GetInstance()->MoveOnNaviMesh(&m_pInfo->m_vPos, &(m_pInfo->m_vDir * m_fSpeed * fTime), m_dwCellNum);
			m_pInfo->m_ServerInfo.pos.x = m_pInfo->m_vPos.x;
			m_pInfo->m_ServerInfo.pos.y = m_pInfo->m_vPos.z;

			if (m_pInfo->m_ServerInfo.dir != (KEYINPUT_UP | KEYINPUT_RIGHT)) {
				m_pInfo->m_ServerInfo.dir = (KEYINPUT_UP | KEYINPUT_RIGHT);
				//g_client.sendPacket(sizeof(char), CHANGED_DIRECTION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.dir));
			}
			//g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));
		}
		else
		{
			m_pInfo->m_fAngle[ANGLE_Y] = D3DXToRadian(135.f);
			m_dwCellNum = CNaviMgr::GetInstance()->MoveOnNaviMesh(&m_pInfo->m_vPos, &(m_pInfo->m_vDir * m_fSpeed * fTime), m_dwCellNum);
			m_pInfo->m_ServerInfo.pos.x = m_pInfo->m_vPos.x;
			m_pInfo->m_ServerInfo.pos.y = m_pInfo->m_vPos.z;

			if (m_pInfo->m_ServerInfo.dir != KEYINPUT_UP) {
				m_pInfo->m_ServerInfo.dir = KEYINPUT_UP;
				//g_client.sendPacket(sizeof(char), CHANGED_DIRECTION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.dir));
			}
			//g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));
		}


		if (m_ePlayerState == PLAYER_MOVE)
			return;

		m_ePlayerState = PLAYER_MOVE;

		dynamic_cast<CDynamicMesh*>(m_pBuffer)->ResetPlayTimer();

		//cout << "상" << endl;
		cout << "플레이어 좌표 ( " << m_pInfo->m_vPos.x << " , " << m_pInfo->m_vPos.z << " )\n";
	}
	else
	{
		m_bPush = false;
		m_bMoving = false;
		if (m_bMoveSend == true)
		{
			g_client.sendPacket(sizeof(char), CHANGED_DIRECTION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.dir));
			g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));
			m_bMoveSend = false;
		}
		if (m_dwTime + 120 < GetTickCount())
		{
			m_dwTime = GetTickCount();

			if (CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer") != NULL)
			{
				list<CObj*>::iterator iter = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->begin();
				list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->end();

				for (iter; iter != iter_end; ++iter)
				{
					((COtherPlayer*)(*iter))->m_bKey = false;
				}
			}
		}
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_DOWN) & 0x80)
	{
		m_bPush = true;
		m_bMoving = true;
		m_bMoveSend = true;

		if (CInput::GetInstance()->GetDIKeyState(DIK_LEFT) & 0x80)
		{
			m_pInfo->m_fAngle[ANGLE_Y] = D3DXToRadian(0.f);
			m_dwCellNum = CNaviMgr::GetInstance()->MoveOnNaviMesh(&m_pInfo->m_vPos, &(m_pInfo->m_vDir * m_fSpeed * fTime), m_dwCellNum);
			m_pInfo->m_ServerInfo.pos.x = m_pInfo->m_vPos.x;
			m_pInfo->m_ServerInfo.pos.y = m_pInfo->m_vPos.z;

			if (m_pInfo->m_ServerInfo.dir != (KEYINPUT_DOWN | KEYINPUT_LEFT)) {
				m_pInfo->m_ServerInfo.dir = (KEYINPUT_DOWN | KEYINPUT_LEFT);
				//g_client.sendPacket(sizeof(char), CHANGED_DIRECTION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.dir));
			}
			//g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));
		}
		else if (CInput::GetInstance()->GetDIKeyState(DIK_RIGHT) & 0x80)
		{
			m_pInfo->m_fAngle[ANGLE_Y] = D3DXToRadian(270.f);
			m_dwCellNum = CNaviMgr::GetInstance()->MoveOnNaviMesh(&m_pInfo->m_vPos, &(m_pInfo->m_vDir * m_fSpeed * fTime), m_dwCellNum);
			m_pInfo->m_ServerInfo.pos.x = m_pInfo->m_vPos.x;
			m_pInfo->m_ServerInfo.pos.y = m_pInfo->m_vPos.z;

			if (m_pInfo->m_ServerInfo.dir != (KEYINPUT_DOWN | KEYINPUT_RIGHT)) {
				m_pInfo->m_ServerInfo.dir = (KEYINPUT_DOWN | KEYINPUT_RIGHT);
				//g_client.sendPacket(sizeof(char), CHANGED_DIRECTION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.dir));
			}
			//g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));
		}
		else
		{
			m_pInfo->m_fAngle[ANGLE_Y] = D3DXToRadian(315.f);
			m_dwCellNum = CNaviMgr::GetInstance()->MoveOnNaviMesh(&m_pInfo->m_vPos, &(m_pInfo->m_vDir * m_fSpeed * fTime), m_dwCellNum);
			m_pInfo->m_ServerInfo.pos.x = m_pInfo->m_vPos.x;
			m_pInfo->m_ServerInfo.pos.y = m_pInfo->m_vPos.z;


			if (m_pInfo->m_ServerInfo.dir != KEYINPUT_DOWN) {
				m_pInfo->m_ServerInfo.dir = KEYINPUT_DOWN;
				//g_client.sendPacket(sizeof(char), CHANGED_DIRECTION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.dir));
			}
			//g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));
		}


		if (m_ePlayerState == PLAYER_MOVE)
			return;


		m_ePlayerState = PLAYER_MOVE;

		dynamic_cast<CDynamicMesh*>(m_pBuffer)->ResetPlayTimer();

		//cout << "하" << endl;
		cout << "플레이어 좌표 ( " << m_pInfo->m_vPos.x << " , " << m_pInfo->m_vPos.z << " )\n";
	}
	else
	{
		m_bPush = false;
		m_bMoving = false;
		if (m_bMoveSend == true)
		{
			g_client.sendPacket(sizeof(char), CHANGED_DIRECTION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.dir));
			g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));
			m_bMoveSend = false;
		}
		if (m_dwTime + 120 < GetTickCount())
		{
			m_dwTime = GetTickCount();

			if (CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer") != NULL)
			{
				list<CObj*>::iterator iter = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->begin();
				list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->end();

				for (iter; iter != iter_end; ++iter)
				{
					((COtherPlayer*)(*iter))->m_bKey = false;
				}
			}
		}
	}


	if (CInput::GetInstance()->GetDIKeyState(DIK_LEFT) & 0x80)
	{
		m_bPush = true;
		m_bMoving = true;
		m_bMoveSend = true;
		m_pInfo->m_fAngle[ANGLE_Y] = D3DXToRadian(45.f);
		m_dwCellNum = CNaviMgr::GetInstance()->MoveOnNaviMesh(&m_pInfo->m_vPos, &(m_pInfo->m_vDir * m_fSpeed * fTime), m_dwCellNum);
		m_pInfo->m_ServerInfo.pos.x = m_pInfo->m_vPos.x;
		m_pInfo->m_ServerInfo.pos.y = m_pInfo->m_vPos.z;


		if (m_pInfo->m_ServerInfo.dir != KEYINPUT_LEFT) {
			m_pInfo->m_ServerInfo.dir = KEYINPUT_LEFT;
			//g_client.sendPacket(sizeof(char), CHANGED_DIRECTION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.dir));
		}
		//g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));


		if (m_ePlayerState == PLAYER_MOVE)
			return;


		m_ePlayerState = PLAYER_MOVE;

		dynamic_cast<CDynamicMesh*>(m_pBuffer)->ResetPlayTimer();

		//cout << "좌" << endl;
		cout << "플레이어 좌표 ( " << m_pInfo->m_vPos.x << " , " << m_pInfo->m_vPos.z << " )\n";
	}
	else
	{
		m_bPush = false;
		m_bMoving = false;
		if (m_bMoveSend == true)
		{
			g_client.sendPacket(sizeof(char), CHANGED_DIRECTION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.dir));
			g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));
			m_bMoveSend = false;
		}
		if (m_dwTime + 120 < GetTickCount())
		{
			m_dwTime = GetTickCount();

			if (CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer") != NULL)
			{
				list<CObj*>::iterator iter = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->begin();
				list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->end();

				for (iter; iter != iter_end; ++iter)
				{
					((COtherPlayer*)(*iter))->m_bKey = false;
				}
			}
		}
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_RIGHT) & 0x80)
	{
		m_bPush = true;
		m_bMoving = true;
		m_bMoveSend = true;
		m_pInfo->m_fAngle[ANGLE_Y] = D3DXToRadian(225.f);
		CNaviMgr::GetInstance()->MoveOnNaviMesh(&m_pInfo->m_vPos, &(m_pInfo->m_vDir * m_fSpeed * fTime), m_dwCellNum);
		m_pInfo->m_ServerInfo.pos.x = m_pInfo->m_vPos.x;
		m_pInfo->m_ServerInfo.pos.y = m_pInfo->m_vPos.z;


		if (m_pInfo->m_ServerInfo.dir != KEYINPUT_RIGHT) {
			m_pInfo->m_ServerInfo.dir = KEYINPUT_RIGHT;
			//g_client.sendPacket(sizeof(char), CHANGED_DIRECTION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.dir));
		}
		//g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));

		if (m_ePlayerState == PLAYER_MOVE)
			return;


		m_ePlayerState = PLAYER_MOVE;

		dynamic_cast<CDynamicMesh*>(m_pBuffer)->ResetPlayTimer();

		//cout << "우" << endl;
		cout << "플레이어 좌표 ( " << m_pInfo->m_vPos.x << " , " << m_pInfo->m_vPos.z << " )\n";
	}
	else
	{
		m_bPush = false;
		m_bMoving = false;
		if (m_bMoveSend == true)
		{
			g_client.sendPacket(sizeof(char), CHANGED_DIRECTION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.dir));
			g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));
			m_bMoveSend = false;
		}

		if (m_dwTime + 120 < GetTickCount())
		{
			m_dwTime = GetTickCount();

			if (CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer") != NULL)
			{
				list<CObj*>::iterator iter = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->begin();
				list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->end();

				for (iter; iter != iter_end; ++iter)
				{
					((COtherPlayer*)(*iter))->m_bKey = false;
				}
			}
		}
	}


	if ((CInput::GetInstance()->GetDIKeyState(DIK_SPACE) & 0x80) && (m_fKeyCool > 0.7f))
	{
		m_bPush = true;
		int iSeverAtt;

		/*if (m_ePlayerState == PLAYER_ATT1 || m_ePlayerState == PLAYER_ATT2 || m_ePlayerState == PLAYER_ATT3)
			return;*/

		if (m_bCombo[0] == false && m_bCombo[1] == false)
		{
			if (m_ePlayerState == PLAYER_ATT1)
				return;
			m_bCombo[0] = true;
			m_fComboTime = 0.f;
			m_fKeyCool = 0.f;
			m_ePlayerState = PLAYER_ATT1;
			//cout << "콤보1단계 활성" << endl;
			iSeverAtt = COMBO1;
		}
		else if (m_bCombo[0] == true && m_bCombo[1] == false)
		{
			m_bCombo[1] = true;
			//m_bCombo[0] = false;
			m_fComboTime = 0.f;
			m_fKeyCool = 0.f;
			m_ePlayerState = PLAYER_ATT2;
			//cout << "콤보2단계 활성" << endl;
			iSeverAtt = COMBO2;
		}
		else if (m_bCombo[0] == true && m_bCombo[1] == true)
		{
			if (m_bCombo[0] == false && m_bCombo[1] == false)
				__debugbreak();

			m_bCombo[0] = false;
			m_bCombo[1] = false;
			//cout << "콤보 해제	1:" << boolalpha << m_bCombo[0] << "	2:" << boolalpha << m_bCombo[1] << endl;
			m_fComboTime = 0.f;
			m_fKeyCool = 0.f;
			m_ePlayerState = PLAYER_ATT3;
			iSeverAtt = COMBO3;
		}

		g_client.sendPacket(sizeof(int), KEYINPUT_ATTACK, reinterpret_cast<BYTE*>(&iSeverAtt));

		dynamic_cast<CDynamicMesh*>(m_pBuffer)->ResetPlayTimer();
	}
	else
	{
		m_bPush = false;
		if (m_dwTime + 120 < GetTickCount())
		{
			m_dwTime = GetTickCount();

			if (CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer") != NULL)
			{
				list<CObj*>::iterator iter = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->begin();
				list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->end();

				for (iter; iter != iter_end; ++iter)
				{
					((COtherPlayer*)(*iter))->m_bKey = false;
				}
			}
		}
	}
	/*else
	{
		m_bPush = false;

		if (m_dwTime + 120 < GetTickCount())
		{
			m_dwTime = GetTickCount();

			if (CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer") != NULL)
			{
				list<CObj*>::iterator iter = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->begin();
				list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->end();

				for (iter; iter != iter_end; ++iter)
				{
					((COtherPlayer*)(*iter))->m_bKey = false;
				}
			}
		}
	}*/


	if ((CInput::GetInstance()->GetDIKeyState(DIK_Q) & 0x80) && m_pInfo->m_ServerInfo.state.gauge > 50)
	{
		m_bPush = true;
		int iSeverAtt;

		if (m_ePlayerState == PLAYER_SKILL1)
			return;

		m_ePlayerState = PLAYER_SKILL1;
		iSeverAtt = SKILL1;
		m_bSkillUsed = true;

		g_client.sendPacket(sizeof(int), KEYINPUT_ATTACK, reinterpret_cast<BYTE*>(&iSeverAtt));

		dynamic_cast<CDynamicMesh*>(m_pBuffer)->ResetPlayTimer();
	}
	else
	{
		m_bPush = false;
		if (m_dwTime + 120 < GetTickCount())
		{
			m_dwTime = GetTickCount();

			if (CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer") != NULL)
			{
				list<CObj*>::iterator iter = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->begin();
				list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->end();

				for (iter; iter != iter_end; ++iter)
				{
					((COtherPlayer*)(*iter))->m_bKey = false;
				}
			}
		}
	}

	if ((CInput::GetInstance()->GetDIKeyState(DIK_W) & 0x80) && m_pInfo->m_ServerInfo.state.gauge > 50)
	{
		m_bPush = true;
		int iSeverAtt;

		if (m_ePlayerState == PLAYER_SKILL2)
			return;

		m_ePlayerState = PLAYER_SKILL2;
		iSeverAtt = SKILL2;
		m_bSkillUsed = true;

		g_client.sendPacket(sizeof(int), KEYINPUT_ATTACK, reinterpret_cast<BYTE*>(&iSeverAtt));

		dynamic_cast<CDynamicMesh*>(m_pBuffer)->ResetPlayTimer();
	}
	else
	{
		m_bPush = false;
		if (m_dwTime + 120 < GetTickCount())
		{
			m_dwTime = GetTickCount();

			if (CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer") != NULL)
			{
				list<CObj*>::iterator iter = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->begin();
				list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->Get_ObjList(L"OtherPlayer")->end();

				for (iter; iter != iter_end; ++iter)
				{
					((COtherPlayer*)(*iter))->m_bKey = false;
				}
			}
		}
	}


	if (CInput::GetInstance()->GetDIKeyState(DIK_1) & 0x80)
	{
		if (m_bPotionCool == true)
		{
			//cout << "포션 쿨타임중" << endl;
			return;
		}
		g_client.sendPacket(sizeof(nullptr), KEYINPUT_POTION, 0);
		m_bPotionCool = true;
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_2) & 0x80)
	{
		if (m_bTpCool == true)
		{
			return;
		}

		m_pInfo->m_vPos = D3DXVECTOR3(155.f, 0.f, 400.f);
		//m_pInfo->m_vPos = D3DXVECTOR3(0.f, 0.f, 0.f);
		m_pInfo->m_ServerInfo.pos.x = m_pInfo->m_vPos.x;
		m_pInfo->m_ServerInfo.pos.y = m_pInfo->m_vPos.z;
		SetNaviIndex(CNaviMgr::GetInstance()->GetCellIndex(&m_pInfo->m_vPos));
		g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));
		m_bTpCool = true;
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_3) & 0x80)
	{
		if (m_bTpCool == true)
		{
			return;
		}
		m_pInfo->m_vPos = D3DXVECTOR3(330.f, 0.f, 408.f);
		m_pInfo->m_ServerInfo.pos.x = m_pInfo->m_vPos.x;
		m_pInfo->m_ServerInfo.pos.y = m_pInfo->m_vPos.z;
		SetNaviIndex(CNaviMgr::GetInstance()->GetCellIndex(&m_pInfo->m_vPos));
		g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));
		m_bTpCool = true;
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_4) & 0x80)
	{
		if (m_bTpCool == true)
		{
			return;
		}
		m_pInfo->m_vPos = D3DXVECTOR3(260.f, 0.f, 300.f);
		m_pInfo->m_ServerInfo.pos.x = m_pInfo->m_vPos.x;
		m_pInfo->m_ServerInfo.pos.y = m_pInfo->m_vPos.z;
		SetNaviIndex(CNaviMgr::GetInstance()->GetCellIndex(&m_pInfo->m_vPos));
		g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));
		m_bTpCool = true;
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_T) & 0x80)
	{
		CQuestUI* pQuestUI = dynamic_cast<CQuestUI*>(*(CObjMgr::GetInstance()->Get_ObjList(L"QuestUI")->begin()));
		if (m_eQuestState == QUEST_NOT)
		{
			list<CObj*>::iterator iter = CObjMgr::GetInstance()->Get_ObjList(L"NPC")->begin();
			list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->Get_ObjList(L"NPC")->end();

			for (iter; iter != iter_end; ++iter)
			{
				if (dynamic_cast<CNpc*>(*iter)->m_eNpcType == NPC_SLIME)
				{
					if ((abs(m_pInfo->m_vPos.x - (*iter)->GetInfo()->m_vPos.x) < 5.f && abs(m_pInfo->m_vPos.z - (*iter)->GetInfo()->m_vPos.z) < 5.f) && (false == m_bQuestFlag))
					{
						m_bQuestFlag = true;
						m_eQuestState = QUEST_SLIME;
						pQuestUI->m_bRender = true;
						g_client.sendPacket(0, QUEST_START, nullptr);
						cout << "슬라임 퀘스트 받아짐" << endl;
					}
				}
			}

		}

		else if (m_eQuestState == QUEST_SLIME && m_iQuestStateMount == MAX_AI_SLIME)
		{
			list<CObj*>::iterator iter = CObjMgr::GetInstance()->Get_ObjList(L"NPC")->begin();
			list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->Get_ObjList(L"NPC")->end();

			for (iter; iter != iter_end; ++iter)
			{
				if (dynamic_cast<CNpc*>(*iter)->m_eNpcType == NPC_GOBLIN)
				{
					if ((abs(m_pInfo->m_vPos.x - (*iter)->GetInfo()->m_vPos.x)  < 5.f && abs(m_pInfo->m_vPos.z - (*iter)->GetInfo()->m_vPos.z) < 5.f) && (true == m_bQuestFlag))
					{
						m_bQuestFlag = false;
						m_eQuestState = QUEST_GOBLIN;
						g_client.sendPacket(0, QUEST_START, nullptr);
						pQuestUI->m_bRender = true;
						cout << "고블린 퀘스트 받아짐" << endl;
					}
				}
			}

		}

		else if (m_eQuestState == QUEST_GOBLIN && m_iQuestStateMount == MAX_AI_GOBLIN)
		{
			list<CObj*>::iterator iter = CObjMgr::GetInstance()->Get_ObjList(L"NPC")->begin();
			list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->Get_ObjList(L"NPC")->end();

			for (iter; iter != iter_end; ++iter)
			{
				if (dynamic_cast<CNpc*>(*iter)->m_eNpcType == NPC_BOSS)
				{
					if ((abs(m_pInfo->m_vPos.x - (*iter)->GetInfo()->m_vPos.x) < 5.f && abs(m_pInfo->m_vPos.z - (*iter)->GetInfo()->m_vPos.z) < 5.f) && (false == m_bQuestFlag))
					{
						m_bQuestFlag = true;
						m_eQuestState = QUEST_BOSS;
						g_client.sendPacket(0, QUEST_START, nullptr);
						pQuestUI->m_bRender = true;
						cout << "보스 퀘스트 받아짐" << endl;
					}
				}
			}
		}
	}

}

void CPlayer::TimeSetter(void)
{

	m_fSeverTime += CTimeMgr::GetInstance()->GetTime();
	g_fChatCool += CTimeMgr::GetInstance()->GetTime();
	m_fKeyCool += CTimeMgr::GetInstance()->GetTime();
	if (m_bPotionCool == true)
		m_fPotionTime += CTimeMgr::GetInstance()->GetTime();
	if (m_bTpCool == true)
		m_fTpTime += CTimeMgr::GetInstance()->GetTime();
	if (m_bCombo[0] == true || m_bCombo[1] == true || m_bCombo[2] == true)
	{
		m_fComboTime += CTimeMgr::GetInstance()->GetTime();
	}
	if (m_bSkillUsed == true)
		m_fSkillMoveTime += CTimeMgr::GetInstance()->GetTime();


	if (m_fSeverTime > 0.1f && m_bMoveSend)
	{
		g_client.sendPacket(sizeof(char), CHANGED_DIRECTION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.dir));
		g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));
		m_fSeverTime = 0.f;
	}

	if (m_fPotionTime > 5.f)
	{
		m_fPotionTime = 0.f;
		if (m_bPotionCool == true)
		{
			m_bPotionCool = false;
		}
	}

	if (m_fTpTime > 5.f)
	{
		m_fTpTime = 0.f;
		if (m_bTpCool == true)
		{
			m_bTpCool = false;
		}
	}

	if (m_fComboTime > 2.8f && m_bCombo[0] == true)
	{
		m_bCombo[0] = false;
		m_bCombo[1] = false;
		//cout << "콤보1단계 초기화" << endl;
	}

	if (m_fComboTime > 2.8f && m_bCombo[1] == true)
	{
		m_bCombo[0] = false;
		m_bCombo[1] = false;
		//cout << "콤보2단계 초기화" << endl;
	}
}


Packet* CPlayer::GetPacket()
{
	return m_Packet;
}

void CPlayer::AniMove(void)
{
	//애니매이션에 따라 얼마나 이동할껀지 부분임.

	float		fTime = CTimeMgr::GetInstance()->GetTime();

	/*if(m_bSkillUsed ==true)
		cout << m_fSkillMoveTime << endl;*/

		/*if (m_ePlayerState == PLAYER_SKILL1)
		{
			if (m_fSkillMoveTime > 0.6f && m_fSkillMoveTime < 1.2f)
			{
				CNaviMgr::GetInstance()->MoveOnNaviMesh(&m_pInfo->m_vPos, &(m_pInfo->m_vDir * m_fSpeed * fTime), m_dwCellNum);
				g_client.sendPacket(sizeof(char), CHANGED_DIRECTION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.dir));
				g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));
			}
		}*/
}
