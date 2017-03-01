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
#include"../../../../server/serverBoostModel/serverBoostModel/protocol.h"
#include "RenderMgr.h"
#include "AnimationMgr.h"
#include "ResourcesMgr.h"


CPlayer::CPlayer()
{
	m_pBuffer = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pTexture = NULL;
	m_pVerTex = NULL;
	m_pTerrainCol = NULL;
	m_fSeverTime = 0.f;
	//m_eObjDir = OBJDIR_UP;
	m_bSendServer = false;
	m_ePlayerState = PLAYER_IDLE;
	m_bPush = false;

	m_Packet = new Packet[MAX_BUF_SIZE];
}


CPlayer::~CPlayer()
{
	Release();
}

HRESULT CPlayer::Initialize(void)
{
	if (FAILED(AddComponent()))
		return E_FAIL;
	//m_pInfo->m_fAngle[ANGLE_X] = /*D3DX_PI / 2 * -1.f;*/D3DXToRadian(-90);
	//m_ServerInfo.pos.x = m_pInfo->m_vPos.x;
	//m_ServerInfo.pos.y = m_pInfo->m_vPos.z;
	//m_pInfo->m_vScale = D3DXVECTOR3(0.05f, 0.05f, 0.05f);
	m_pInfo->m_vScale = D3DXVECTOR3(5.f, 5.f, 5.f);

	list<CObj*>::iterator iter = CObjMgr::GetInstance()->m_mapObj[L"Terrain"].begin();
	list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->m_mapObj[L"Terrain"].end();

	if (iter != iter_end)
		m_pVerTex = *dynamic_cast<CTerrain*>(*iter)->GetVertex();

	CRenderMgr::GetInstance()->AddRenderGroup(TYPE_NONEALPHA, this);
	
	return S_OK;
}

int CPlayer::Update(void)
{
	m_fSeverTime += CTimeMgr::GetInstance()->GetTime();
	m_pTerrainCol->CollisionTerrain(&m_pInfo->m_vPos, m_pVerTex);
	/*if (dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_bAniEnd == true)
		dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_bAniEnd = false;*/

	if (m_bSendServer == false)
	{
		m_pInfo->m_ServerInfo = *g_client.getPlayerData();
		g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));
		g_client.sendPacket(sizeof(char), CHANGED_DIRECTION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.dir));
		m_bSendServer = true;
	}


	if (m_fSeverTime > 0.5f)
	{
		
		m_fSeverTime = 0.f;
	}

	D3DXVec3TransformNormal(&m_pInfo->m_vDir, &g_vLook, &m_pInfo->m_matWorld);

	//cout << m_pInfo->m_vDir.x << "/" << m_pInfo->m_vDir.y << "/" << m_pInfo->m_vDir.z << endl;

	//cout << "Player pos: " << m_pInfo->m_vPos.x << "/" << m_pInfo->m_vPos.y << "/" << m_pInfo->m_vPos.z << endl;


	KeyInput();
	AniMove();

	cout <<"내 채력:" << m_pInfo->m_ServerInfo.state.hp << endl;

	CObj::Update();

	cout << m_bPush << endl;

	if (m_bPush == false)
		m_ePlayerState = PLAYER_IDLE;


	//if(dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_bAniEnd = false)
	

	//m_ServerInfo.pos.x = m_pInfo->m_vPos.x;
	//m_ServerInfo.pos.y = m_pInfo->m_vPos.z;
	return 0;
}

void CPlayer::Render(void)
{

	ConstantBuffer cb;
	D3DXMatrixTranspose(&cb.matWorld, &m_pInfo->m_matWorld);
	D3DXMatrixTranspose(&cb.matView, &CCamera::GetInstance()->m_matView);
	D3DXMatrixTranspose(&cb.matProjection, &CCamera::GetInstance()->m_matProj);
	m_pGrapicDevice->m_pDeviceContext->UpdateSubresource(m_pBuffer->m_ConstantBuffer, 0, NULL, &cb, 0, 0);

	m_pGrapicDevice->m_pDeviceContext->VSSetShader(m_pVertexShader->m_pVertexShader, NULL, 0);
	m_pGrapicDevice->m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pBuffer->m_ConstantBuffer);
	//////////////////
	m_pGrapicDevice->m_pDeviceContext->PSSetShader(m_pPixelShader->m_pPixelShader, NULL, 0);
	m_pGrapicDevice->m_pDeviceContext->PSSetShaderResources(0, 1, &m_pTexture->m_pTextureRV);
	m_pGrapicDevice->m_pDeviceContext->PSSetSamplers(0, 1, &m_pTexture->m_pSamplerLinear);

	//m_pBuffer->Render();
	dynamic_cast<CDynamicMesh*>(m_pBuffer)->PlayAnimation(m_ePlayerState);

}

CPlayer * CPlayer::Create(void)
{
	CPlayer* pObj = new CPlayer;
	if (FAILED(pObj->Initialize()))
		::Safe_Delete(pObj);

	return pObj;
}

void CPlayer::Release(void)
{
	Safe_Delete(m_pBuffer);
	Safe_Delete(m_pInfo);
	Safe_Delete(m_pTexture);
	Safe_Delete_Array(m_Packet);
}

HRESULT CPlayer::AddComponent(void)
{
	CComponent* pComponent = NULL;

	vecAniName = *(CAnimationMgr::GetInstance()->GetAnimaiton(L"Player"));
	//vecName.push_back("Fall");
	//vecName.push_back("Dead");
	//vecName.push_back("Damage");
	//vecAniName.push_back("Booster");
	//vecName.push_back("Break");

	//TransForm
	pComponent = m_pInfo = CInfo::Create(g_vLook);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Transform", pComponent));


	//DynamicMesh
	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Player_IDLE");
	m_pBuffer = dynamic_cast<CDynamicMesh*>(pComponent);
	NULL_CHECK_RETURN(m_pBuffer, E_FAIL);
	m_mapComponent.insert(map<wstring, CComponent*>::value_type(L"Mesh", pComponent));

	//Texture
	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Texture_Player");
	m_pTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(m_pTexture, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Texture", pComponent));

	m_pVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"VS_ANI");
	m_pPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"PS");;


	return S_OK;
}


void CPlayer::KeyInput()
{
	float		fTime = CTimeMgr::GetInstance()->GetTime();

	//g_client->m_recvbuf
	if (CInput::GetInstance()->GetDIKeyState(DIK_UP) & 0x80)
	{
		m_bPush = true;
		m_pInfo->m_fAngle[ANGLE_Y] = D3DXToRadian(315.f);
		m_pInfo->m_vPos += m_pInfo->m_vDir * 5.f * fTime;
		m_pInfo->m_ServerInfo.pos.x = m_pInfo->m_vPos.x;
		m_pInfo->m_ServerInfo.pos.y = m_pInfo->m_vPos.z;


		m_pInfo->m_ServerInfo.dir = KEYINPUT_UP;
		g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));
		g_client.sendPacket(sizeof(char), CHANGED_DIRECTION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.dir));

		
		if (m_ePlayerState == PLAYER_MOVE)
			return;

		m_ePlayerState = PLAYER_MOVE;
		
		//m_eObjDir = OBJDIR_UP;


		//dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_bAniEnd = false;
		//dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_fAniPlayTimer = 0;
		//m_ePlayerState = PLAYER_MOVE;

		//cout << "상" << endl;
	}
	/*else
		m_bPush = false;*/

	else if (CInput::GetInstance()->GetDIKeyState(DIK_DOWN) & 0x80)
	{
		m_bPush = true;
		m_pInfo->m_fAngle[ANGLE_Y] = D3DXToRadian(135.f);
		m_pInfo->m_vPos += m_pInfo->m_vDir * 5.f * fTime;
		m_pInfo->m_ServerInfo.pos.x = m_pInfo->m_vPos.x;
		m_pInfo->m_ServerInfo.pos.y = m_pInfo->m_vPos.z;


		m_pInfo->m_ServerInfo.dir = KEYINPUT_DOWN;
		g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));
		g_client.sendPacket(sizeof(char), CHANGED_DIRECTION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.dir));

		
		if (m_ePlayerState == PLAYER_MOVE)
			return;


		m_ePlayerState = PLAYER_MOVE;
		
		//m_eObjDir = OBJDIR_UP;


		//dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_bAniEnd = false;
		//dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_fAniPlayTimer = 0;
		//m_ePlayerState = PLAYER_MOVE;

		//cout << "하" << endl;
	}
	/*else
		m_bPush = false;*/
	

	else if (CInput::GetInstance()->GetDIKeyState(DIK_LEFT) & 0x80)
	{
		m_bPush = true;
		m_pInfo->m_fAngle[ANGLE_Y] = D3DXToRadian(225.f);
		m_pInfo->m_vPos += m_pInfo->m_vDir * 5.f * fTime;
		m_pInfo->m_ServerInfo.pos.x = m_pInfo->m_vPos.x;
		m_pInfo->m_ServerInfo.pos.y = m_pInfo->m_vPos.z;


		m_pInfo->m_ServerInfo.dir = KEYINPUT_LEFT;
		g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));
		g_client.sendPacket(sizeof(char), CHANGED_DIRECTION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.dir));

		
		if (m_ePlayerState == PLAYER_MOVE)
			return;


		m_ePlayerState = PLAYER_MOVE;
		
		//m_eObjDir = OBJDIR_UP;


		//dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_bAniEnd = false;
		//dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_fAniPlayTimer = 0;
		//m_ePlayerState = PLAYER_MOVE;

		//cout << "좌" << endl;
	}
	/*else
		m_bPush = false;*/

	else if (CInput::GetInstance()->GetDIKeyState(DIK_RIGHT) & 0x80)
	{
		m_bPush = true;
		m_pInfo->m_fAngle[ANGLE_Y] = D3DXToRadian(45.f);
		m_pInfo->m_vPos += m_pInfo->m_vDir * 5.f * fTime;
		m_pInfo->m_ServerInfo.pos.x = m_pInfo->m_vPos.x;
		m_pInfo->m_ServerInfo.pos.y = m_pInfo->m_vPos.z;


		m_pInfo->m_ServerInfo.dir = KEYINPUT_RIGHT;
		g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));
		g_client.sendPacket(sizeof(char), CHANGED_DIRECTION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.dir));

		if (m_ePlayerState == PLAYER_MOVE)
			return;

		
		m_ePlayerState = PLAYER_MOVE;
		
		//m_eObjDir = OBJDIR_UP;


		//dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_bAniEnd = false;
		//dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_fAniPlayTimer = 0;
		//m_ePlayerState = PLAYER_MOVE;

		//cout << "우" << endl;
	}
	/*else
		m_bPush = false;*/
	
	

	else if (CInput::GetInstance()->GetDIKeyState(DIK_SPACE) & 0x80)
	{
		m_bPush = true;

		g_client.sendPacket(0, KEYINPUT_ATTACK, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.state.hp));

		if (m_ePlayerState == PLAYER_ATT1)
			return;

		

		m_ePlayerState = PLAYER_ATT1;

		//dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_bAniEnd = false;
		//dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_fAniPlayTimer = 0;
	}
	else
	{
		m_bPush = false;
	}
}


Packet* CPlayer::GetPacket()
{
	return m_Packet;
}

void CPlayer::AniMove(void)
{
	//애니매이션에 따라 얼마나 이동할껀지 부분임.
}
