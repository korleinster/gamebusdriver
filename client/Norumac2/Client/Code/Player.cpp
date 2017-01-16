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


CPlayer::CPlayer()
{
	m_pBuffer = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pTexture = NULL;
	m_pVerTex = NULL;
	m_pTerrainCol = NULL;
	m_fSeverTime = 0.f;
	test = false;

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
	m_pInfo->m_vScale = D3DXVECTOR3(0.01f, 0.01f, 0.01f);

	
	
	return S_OK;
}

int CPlayer::Update(void)
{
	m_fSeverTime += CTimeMgr::GetInstance()->GetTime();
	/*if (m_pBuffer->m_bAniEnd == true)
		m_pBuffer->m_bAniEnd = false;*/

	if (test == false)
	{
		m_pInfo->m_ServerInfo = *g_client.getPlayerData();
		test = true;
	}


	if (m_fSeverTime > 0.5f)
	{
		m_pInfo->m_ServerInfo.pos.x = m_pInfo->m_vPos.x;
		m_pInfo->m_ServerInfo.pos.y = m_pInfo->m_vPos.z;
		m_pInfo->m_ServerInfo.dir.x = m_pInfo->m_vDir.x;
		m_pInfo->m_ServerInfo.dir.y = m_pInfo->m_vDir.y;
		m_pInfo->m_ServerInfo.dir.z = m_pInfo->m_vDir.z;
		g_client.sendPacket(sizeof(player_data), CHANGED_POSITION, reinterpret_cast<BYTE*>(&m_pInfo->m_ServerInfo));

		m_fSeverTime = 0.f;
	}

	D3DXVec3TransformNormal(&m_pInfo->m_vDir, &g_vLook, &m_pInfo->m_matWorld);

	//cout << m_pInfo->m_vDir.x << "/" << m_pInfo->m_vDir.y << "/" << m_pInfo->m_vDir.z << endl;

	KeyInput();
	CObj::Update();



	

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
	dynamic_cast<CDynamicMesh*>(m_pBuffer)->PlayAnimation(0);

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
	char cModelPath[MAX_PATH] = "../Resource/";

	vecAniName.push_back("Bird");
	//vecName.push_back("Fall");
	//vecName.push_back("Dead");
	//vecName.push_back("Damage");
	//vecAniName.push_back("Booster");
	//vecName.push_back("Break");

	m_pBuffer = CDynamicMesh::Create(cModelPath, vecAniName);
	pComponent = m_pBuffer;
	m_mapComponent.insert(map<wstring, CComponent*>::value_type(L"Buffer", pComponent));

	m_pInfo = CInfo::Create(g_vLook);
	pComponent = m_pInfo;
	if (pComponent == NULL)
		return E_FAIL;
	m_mapComponent.insert(map<wstring, CComponent*>::value_type(L"Info", pComponent));

	m_pTexture = CTexture::Create(L"../Resource/bird.png");
	pComponent = m_pTexture;
	if (pComponent == NULL)
		return E_FAIL;
	m_mapComponent.insert(map<wstring, CComponent*>::value_type(L"Texture", pComponent));

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
		m_pInfo->m_vPos += m_pInfo->m_vDir * 100.f * fTime;
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_DOWN) & 0x80)
	{
		m_pInfo->m_vPos -= m_pInfo->m_vDir * 100.f * fTime;
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_LEFT) & 0x80)
	{
		m_pInfo->m_fAngle[ANGLE_Y] -= D3DXToRadian(90.f * fTime);
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_RIGHT) & 0x80)
	{
		m_pInfo->m_fAngle[ANGLE_Y] += D3DXToRadian(90.f * fTime);
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_PGUP) & 0x80)
	{
		m_pInfo->m_fAngle[ANGLE_Z] += D3DXToRadian(90.f * fTime);
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_PGDN) & 0x80)
	{
		m_pInfo->m_fAngle[ANGLE_Z] -= D3DXToRadian(90.f * fTime);
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_HOME) & 0x80)
	{
		m_pInfo->m_fAngle[ANGLE_X] += D3DXToRadian(90.f * fTime);
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_END) & 0x80)
	{
		m_pInfo->m_fAngle[ANGLE_X] -= D3DXToRadian(90.f * fTime);
	}
}


Packet* CPlayer::GetPacket()
{
	return m_Packet;
}
