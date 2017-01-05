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

	m_pInfo->m_vScale = D3DXVECTOR3(0.01f, 0.01f, 0.01f);
	m_pInfo->m_fAngle[ANGLE_X] = /*D3DX_PI / 2 * -1.f;*/D3DXToRadian(-90);

	return S_OK;
}

int COtherPlayer::Update(void)
{
	/*if (m_pBuffer->m_bAniEnd == true)
	m_pBuffer->m_bAniEnd = false;*/
	
	// 서버에서 좌표를 받아서 다른 플레이어객체들에게 좌표를 줌. ( 어때요 참 쉽죠? )
	//m_pInfo->m_vPos.x = reinterpret_cast<player_data *>(&m_Packet[2])->pos.x;
	//m_pInfo->m_vPos.z = reinterpret_cast<player_data *>(&m_Packet[2])->pos.y;

	D3DXVec3TransformNormal(&m_pInfo->m_vDir, &g_vLook, &m_pInfo->m_matWorld);


	//cout << m_pInfo->m_vDir.x << "/" << m_pInfo->m_vDir.y << "/" << m_pInfo->m_vDir.z << endl;

	CObj::Update();
	return 0;
}

void COtherPlayer::Render(void)
{

		//vector<Animation*>::iterator iter = m_pBuffer->m_vecAni.begin();

		//ConstantBuffer cb;
		//D3DXMatrixTranspose(&cb.matWorld, &m_pInfo->m_matWorld);
		//D3DXMatrixTranspose(&cb.matView, &CCamera::GetInstance()->m_matView);
		//D3DXMatrixTranspose(&cb.matProjection, &CCamera::GetInstance()->m_matProj);

		//m_pBuffer->PlayAnimation(0,m_pVertexShader,m_pPixelShader, &cb, m_pTexture);

		//m_pGrapicDevice->m_pDeviceContext->UpdateSubresource((*iter)->pBoneMatrixBuffer, 0, NULL, &cb, 0, 0);

		//m_pGrapicDevice->m_pDeviceContext->VSSetShader(m_pVertexShader->m_pVertexShader, NULL, 0);
		//m_pGrapicDevice->m_pDeviceContext->VSSetConstantBuffers(0, 1, &(*iter)->pBoneMatrixBuffer);
		////////////////////
		//m_pGrapicDevice->m_pDeviceContext->PSSetShader(m_pPixelShader->m_pPixelShader, NULL, 0);
		//m_pGrapicDevice->m_pDeviceContext->PSSetShaderResources(0, 1, &m_pTexture->m_pTextureRV);
		//m_pGrapicDevice->m_pDeviceContext->PSSetSamplers(0, 1, &m_pTexture->m_pSamplerLinear)

		//m_pBuffer->Render();


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

	m_pBuffer->Render();

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
}

HRESULT COtherPlayer::AddComponent(void)
{
	CComponent* pComponent = NULL;
	char cModelPath[MAX_PATH] = "../Resource/bird.FBX";
	m_pBuffer = CStaticMesh::Create(cModelPath);
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

	m_pVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"VS");
	m_pPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"PS");


	return S_OK;
}

player_data* COtherPlayer::GetPacketData()
{
	return &m_ServerInfo;
}