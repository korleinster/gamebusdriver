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
	
	return S_OK;
}

int CPlayer::Update(void)
{
	//m_pInfo->m_fAngle[1] += 0.1f;
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

	//cout << "내 채력:" << m_pInfo->m_ServerInfo.state.hp << endl;

	CObj::Update();

	//cout << m_bPush << endl;

	if (m_bPush == false)
		m_ePlayerState = PLAYER_IDLE;


	//if(dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_bAniEnd = false)

	//m_ServerInfo.pos.x = m_pInfo->m_vPos.x;
	//m_ServerInfo.pos.y = m_pInfo->m_vPos.z;
	return 0;
}

void CPlayer::Render(void)
{

	//ConstantBuffer cb;
	//D3DXMatrixTranspose(&cb.matWorld, &m_pInfo->m_matWorld);
	//D3DXMatrixTranspose(&cb.matView, &CCamera::GetInstance()->m_matView);
	//D3DXMatrixTranspose(&cb.matProjection, &CCamera::GetInstance()->m_matProj);
	//m_pGrapicDevice->m_pDeviceContext->UpdateSubresource(m_pBuffer->m_ConstantBuffer, 0, NULL, &cb, 0, 0);

	//m_pGrapicDevice->m_pDeviceContext->VSSetShader(m_pVertexShader->m_pVertexShader, NULL, 0);
	//m_pGrapicDevice->m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pBuffer->m_ConstantBuffer);
	////////////////////
	//m_pGrapicDevice->m_pDeviceContext->PSSetShader(m_pPixelShader->m_pPixelShader, NULL, 0);
	//m_pGrapicDevice->m_pDeviceContext->PSSetShaderResources(0, 1, &m_pTexture->m_pTextureRV);
	//m_pGrapicDevice->m_pDeviceContext->PSSetSamplers(0, 1, &m_pTexture->m_pSamplerLinear);

	////m_pBuffer->Render();
	//dynamic_cast<CDynamicMesh*>(m_pBuffer)->PlayAnimation(m_ePlayerState);

	//////////////////////////////////////////////////////////////////////////////////

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


	return S_OK;
}


void CPlayer::KeyInput()
{
	float		fTime = CTimeMgr::GetInstance()->GetTime();

	//g_client->m_recvbuf
	if (CInput::GetInstance()->GetDIKeyState(DIK_UP) & 0x80)
	{
		m_bPush = true;
		if (CInput::GetInstance()->GetDIKeyState(DIK_LEFT) & 0x80)
		{
			m_pInfo->m_fAngle[ANGLE_Y] = D3DXToRadian(90.f);
			m_pInfo->m_vPos += m_pInfo->m_vDir * 5.f * fTime;
			m_pInfo->m_ServerInfo.pos.x = m_pInfo->m_vPos.x;
			m_pInfo->m_ServerInfo.pos.y = m_pInfo->m_vPos.z;

			if (m_pInfo->m_ServerInfo.dir != (KEYINPUT_UP | KEYINPUT_LEFT)) {
				m_pInfo->m_ServerInfo.dir = (KEYINPUT_UP | KEYINPUT_LEFT);
				g_client.sendPacket(sizeof(char), CHANGED_DIRECTION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.dir));
			}
			g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));
		}
		else if (CInput::GetInstance()->GetDIKeyState(DIK_RIGHT) & 0x80)
		{
			m_pInfo->m_fAngle[ANGLE_Y] = D3DXToRadian(180.f);
			m_pInfo->m_vPos += m_pInfo->m_vDir * 5.f * fTime;
			m_pInfo->m_ServerInfo.pos.x = m_pInfo->m_vPos.x;
			m_pInfo->m_ServerInfo.pos.y = m_pInfo->m_vPos.z;

			if (m_pInfo->m_ServerInfo.dir != (KEYINPUT_UP | KEYINPUT_RIGHT)) {
				m_pInfo->m_ServerInfo.dir = (KEYINPUT_UP | KEYINPUT_RIGHT);
				g_client.sendPacket(sizeof(char), CHANGED_DIRECTION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.dir));
			}
			g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));
		}
		else
		{
			m_pInfo->m_fAngle[ANGLE_Y] = D3DXToRadian(135.f);
			m_pInfo->m_vPos += m_pInfo->m_vDir * 5.f * fTime;
			m_pInfo->m_ServerInfo.pos.x = m_pInfo->m_vPos.x;
			m_pInfo->m_ServerInfo.pos.y = m_pInfo->m_vPos.z;

			if (m_pInfo->m_ServerInfo.dir != KEYINPUT_UP) {
				m_pInfo->m_ServerInfo.dir = KEYINPUT_UP;
				g_client.sendPacket(sizeof(char), CHANGED_DIRECTION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.dir));
			}
			g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));
		}

		
		if (m_ePlayerState == PLAYER_MOVE)
			return;

		m_ePlayerState = PLAYER_MOVE;
		
		//m_eObjDir = OBJDIR_UP;


		//dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_bAniEnd = false;
		//dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_fAniPlayTimer = 0;
		//m_ePlayerState = PLAYER_MOVE;

		//cout << "상" << endl;
		cout << "플레이어 좌표 ( " << m_pInfo->m_vPos.x << " , " << m_pInfo->m_vPos.z << " )\n";
	}
	/*else
		m_bPush = false;*/

	else if (CInput::GetInstance()->GetDIKeyState(DIK_DOWN) & 0x80)
	{
		m_bPush = true;

		if (CInput::GetInstance()->GetDIKeyState(DIK_LEFT) & 0x80)
		{
			m_pInfo->m_fAngle[ANGLE_Y] = D3DXToRadian(0.f);
			m_pInfo->m_vPos += m_pInfo->m_vDir * 5.f * fTime;
			m_pInfo->m_ServerInfo.pos.x = m_pInfo->m_vPos.x;
			m_pInfo->m_ServerInfo.pos.y = m_pInfo->m_vPos.z;

			if (m_pInfo->m_ServerInfo.dir != (KEYINPUT_DOWN | KEYINPUT_LEFT)) {
				m_pInfo->m_ServerInfo.dir = (KEYINPUT_DOWN | KEYINPUT_LEFT);
				g_client.sendPacket(sizeof(char), CHANGED_DIRECTION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.dir));
			}
			g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));
		}
		else if (CInput::GetInstance()->GetDIKeyState(DIK_RIGHT) & 0x80)
		{
			m_pInfo->m_fAngle[ANGLE_Y] = D3DXToRadian(270.f);
			m_pInfo->m_vPos += m_pInfo->m_vDir * 5.f * fTime;
			m_pInfo->m_ServerInfo.pos.x = m_pInfo->m_vPos.x;
			m_pInfo->m_ServerInfo.pos.y = m_pInfo->m_vPos.z;

			if (m_pInfo->m_ServerInfo.dir != (KEYINPUT_DOWN | KEYINPUT_RIGHT)) {
				m_pInfo->m_ServerInfo.dir = (KEYINPUT_DOWN | KEYINPUT_RIGHT);
				g_client.sendPacket(sizeof(char), CHANGED_DIRECTION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.dir));
			}
			g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));
		}
		else
		{
			m_pInfo->m_fAngle[ANGLE_Y] = D3DXToRadian(315.f);
			m_pInfo->m_vPos += m_pInfo->m_vDir * 5.f * fTime;
			m_pInfo->m_ServerInfo.pos.x = m_pInfo->m_vPos.x;
			m_pInfo->m_ServerInfo.pos.y = m_pInfo->m_vPos.z;


			if (m_pInfo->m_ServerInfo.dir != KEYINPUT_DOWN) {
				m_pInfo->m_ServerInfo.dir = KEYINPUT_DOWN;
				g_client.sendPacket(sizeof(char), CHANGED_DIRECTION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.dir));
			}
			g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));
		}

		
		if (m_ePlayerState == PLAYER_MOVE)
			return;


		m_ePlayerState = PLAYER_MOVE;
		
		//m_eObjDir = OBJDIR_UP;


		//dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_bAniEnd = false;
		//dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_fAniPlayTimer = 0;
		//m_ePlayerState = PLAYER_MOVE;

		//cout << "하" << endl;
		cout << "플레이어 좌표 ( " << m_pInfo->m_vPos.x << " , " << m_pInfo->m_vPos.z << " )\n";
	}
	/*else
		m_bPush = false;*/
	

	else if (CInput::GetInstance()->GetDIKeyState(DIK_LEFT) & 0x80)
	{
		m_bPush = true;
		m_pInfo->m_fAngle[ANGLE_Y] = D3DXToRadian(45.f);
		m_pInfo->m_vPos += m_pInfo->m_vDir * 5.f * fTime;
		m_pInfo->m_ServerInfo.pos.x = m_pInfo->m_vPos.x;
		m_pInfo->m_ServerInfo.pos.y = m_pInfo->m_vPos.z;


		if (m_pInfo->m_ServerInfo.dir != KEYINPUT_LEFT) {
			m_pInfo->m_ServerInfo.dir = KEYINPUT_LEFT;
			g_client.sendPacket(sizeof(char), CHANGED_DIRECTION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.dir));
		}
		g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));

		
		if (m_ePlayerState == PLAYER_MOVE)
			return;


		m_ePlayerState = PLAYER_MOVE;
		
		//m_eObjDir = OBJDIR_UP;


		//dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_bAniEnd = false;
		//dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_fAniPlayTimer = 0;
		//m_ePlayerState = PLAYER_MOVE;

		//cout << "좌" << endl;
		cout << "플레이어 좌표 ( " << m_pInfo->m_vPos.x << " , " << m_pInfo->m_vPos.z << " )\n";
	}
	/*else
		m_bPush = false;*/

	else if (CInput::GetInstance()->GetDIKeyState(DIK_RIGHT) & 0x80)
	{
		m_bPush = true;
		m_pInfo->m_fAngle[ANGLE_Y] = D3DXToRadian(225.f);
		m_pInfo->m_vPos += m_pInfo->m_vDir * 5.f * fTime;
		m_pInfo->m_ServerInfo.pos.x = m_pInfo->m_vPos.x;
		m_pInfo->m_ServerInfo.pos.y = m_pInfo->m_vPos.z;


		if (m_pInfo->m_ServerInfo.dir != KEYINPUT_RIGHT) {
			m_pInfo->m_ServerInfo.dir = KEYINPUT_RIGHT;
			g_client.sendPacket(sizeof(char), CHANGED_DIRECTION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.dir));
		}
		g_client.sendPacket(sizeof(position), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.pos));

		if (m_ePlayerState == PLAYER_MOVE)
			return;

		
		m_ePlayerState = PLAYER_MOVE;
		
		//m_eObjDir = OBJDIR_UP;


		//dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_bAniEnd = false;
		//dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_fAniPlayTimer = 0;
		//m_ePlayerState = PLAYER_MOVE;

		//cout << "우" << endl;
		cout << "플레이어 좌표 ( " << m_pInfo->m_vPos.x << " , " << m_pInfo->m_vPos.z << " )\n";
	}
	/*else
		m_bPush = false;*/
	
	

	else if (CInput::GetInstance()->GetDIKeyState(DIK_SPACE) & 0x80)
	{
		m_bPush = true;

		if (m_ePlayerState == PLAYER_ATT1)
			return;

		g_client.sendPacket(sizeof(int), KEYINPUT_ATTACK, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo.state.hp));

		m_ePlayerState = PLAYER_ATT1;

		dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_bAniEnd = false;
		dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_fAniPlayTimer = 0;
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
}


Packet* CPlayer::GetPacket()
{
	return m_Packet;
}

void CPlayer::AniMove(void)
{
	//애니매이션에 따라 얼마나 이동할껀지 부분임.
}
