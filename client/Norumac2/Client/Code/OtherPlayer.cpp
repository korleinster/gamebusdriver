#include "stdafx.h"
#include "OtherPlayer.h"
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
#include "ResourcesMgr.h"
#include "AnimationMgr.h"



COtherPlayer::COtherPlayer()
{
	m_pBuffer = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pTexture = NULL;
	m_pVerTex = NULL;
	m_pTerrainCol = NULL;
}


COtherPlayer::~COtherPlayer()
{
	Release();
}

HRESULT COtherPlayer::Initialize(void)
{
	if (FAILED(AddComponent()))
		return E_FAIL;

	m_pInfo->m_vScale = D3DXVECTOR3(0.05f, 0.05f, 0.05f);
	//m_pInfo->m_vScale = D3DXVECTOR3(10.f, 10.f, 10.f);
	//m_pInfo->m_fAngle[ANGLE_X] = /*D3DX_PI / 2 * -1.f;*/D3DXToRadian(-90);

	CRenderMgr::GetInstance()->AddRenderGroup(TYPE_NONEALPHA, this);

	return S_OK;
}

int COtherPlayer::Update(void)

{
	m_fSeverTime += CTimeMgr::GetInstance()->GetTime();
	/*if (m_pBuffer->m_bAniEnd == true)
	m_pBuffer->m_bAniEnd = false;*/
	

	m_pInfo->m_vPos.x = m_pInfo->m_ServerInfo.pos.x;
	m_pInfo->m_vPos.z = m_pInfo->m_ServerInfo.pos.y;
	ChangeDir();
	D3DXVec3TransformNormal(&m_pInfo->m_vDir, &g_vLook, &m_pInfo->m_matWorld);


	//cout <<"OtherPlayer pos: " << m_pInfo->m_vPos.x << "/" << m_pInfo->m_vPos.y << "/" << m_pInfo->m_vPos.z << endl;

	CObj::Update();
	return 0;
}

void COtherPlayer::Render(void)
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

COtherPlayer * COtherPlayer::Create(void)
{
	COtherPlayer* pObj = new COtherPlayer;
	if (FAILED(pObj->Initialize()))
		::Safe_Delete(pObj);

	return pObj;
}

void COtherPlayer::Release(void)
{
	Safe_Release(m_pBuffer);
	Safe_Release(m_pTexture);
	Safe_Release(m_pInfo);

}

HRESULT COtherPlayer::AddComponent(void)
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

void COtherPlayer::ChangeDir(void)
{
	if (m_pInfo->m_ServerInfo.dir == KEYINPUT_UP)
	{
		m_pInfo->m_fAngle[ANGLE_Y] = D3DXToRadian(135.f);
	}
	else if (m_pInfo->m_ServerInfo.dir == KEYINPUT_DOWN)
	{
		m_pInfo->m_fAngle[ANGLE_Y] = D3DXToRadian(315.f);
	}
	else if (m_pInfo->m_ServerInfo.dir == KEYINPUT_LEFT)
	{
		m_pInfo->m_fAngle[ANGLE_Y] = D3DXToRadian(45.f);
	}
	else if (m_pInfo->m_ServerInfo.dir == KEYINPUT_RIGHT)
	{
		m_pInfo->m_fAngle[ANGLE_Y] = D3DXToRadian(225.f);
	}
}

