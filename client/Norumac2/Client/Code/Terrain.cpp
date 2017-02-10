#include "stdafx.h"
#include "Terrain.h"
#include "Info.h"
#include "VIBuffer.h"
#include "RcTerrain.h"
#include "Shader.h"
#include "ShaderMgr.h"
#include "Texture.h"
#include "Camera.h"
#include "Device.h"
#include "Shader.h"

CTerrain::CTerrain()
{
	m_pTerrainBuffer = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pTexture = NULL;
	m_pVerTex = NULL;
	m_pConvertVerTex = NULL;
}

CTerrain::~CTerrain()
{
	Release();
}

CTerrain * CTerrain::Create(void)
{
	CTerrain* pObj = new CTerrain;

	if (FAILED(pObj->Initialize()))
	{
		::Safe_Delete(pObj);
	}

	return pObj;
}

HRESULT CTerrain::Initialize(void)
{
	if (FAILED(AddComponent()))
	{
		MessageBox(NULL, L"System Error", L"Terrain Component Failed", MB_OK);
		return E_FAIL;
	}

	m_pVerTex = new VTXTEX[VERTEXCOUNTX * VERTEXCOUNTZ];

	//m_pGrapicDevice->m_pDeviceContext->GetData
	/*D3D11_MAPPED_SUBRESOURCE MapResource;

	m_pGrapicDevice->m_pDeviceContext->Map(m_pTerrainBuffer->m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MapResource);
	memcpy(m_pVerTex,(VTXTEX*)MapResource.pData, sizeof(VTXTEX) * (VERTEXCOUNTX * VERTEXCOUNTZ));
	m_pGrapicDevice->m_pDeviceContext->Unmap(m_pTerrainBuffer->m_VertexBuffer, 0);*/

	return S_OK;
}

int CTerrain::Update(void)
{

	CObj::Update();
	return 0;
}

void CTerrain::Render(void)
{

	// Initialize the projection matrix
	ConstantBuffer cb;

	//11,12,13,14 = RIGHT = u
	//21,22,23,24 = UP	  = v
	//31,32,33,34 = LOOK  = w
	//41,42,43,44 = Position
	//11 21 31 41
	//12 22 32 42
	//13 23 33 43
	//14 24 34 44

	D3DXMatrixTranspose(&cb.matWorld, &m_pInfo->m_matWorld);
	D3DXMatrixTranspose(&cb.matView, &CCamera::GetInstance()->m_matView);
	D3DXMatrixTranspose(&cb.matProjection, &CCamera::GetInstance()->m_matProj);
	m_pGrapicDevice->m_pDeviceContext->UpdateSubresource(m_pTerrainBuffer->m_ConstantBuffer, 0, NULL, &cb, 0, 0);


	//ÀÌ¿ëÈñ ±³¼ö´ÔÀÇ ·¦ÇÁ·ÎÁ§Æ® ¹ßÃé - ¸Ê/ ¾ð¸Ê -  = Direct 9 Lock / unlock
	//D3D11_MAPPED_SUBRESOURCE MapResource;
	//m_pGrapicDevcie->m_pDeviceContext->Map(m_pRcCol->m_ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MapResource);
	//ConstantBuffer* pConstantBuffer = (ConstantBuffer *)MapResource.pData;
	////pConstantBuffer->matWorld = m_pInfo->m_matWorld;
	////pConstantBuffer->matView = CCamera::GetInstance()->m_matView;
	////pConstantBuffer->matProjection = CCamera::GetInstance()->m_matProj;
	//D3DXMatrixTranspose(&pConstantBuffer->matWorld, &m_pInfo->m_matWorld);
	//D3DXMatrixTranspose(&pConstantBuffer->matView, &CCamera::GetInstance()->m_matView);
	//D3DXMatrixTranspose(&pConstantBuffer->matProjection, &CCamera::GetInstance()->m_matProj);
	//m_pGrapicDevcie->m_pDeviceContext->Unmap(m_pRcCol->m_ConstantBuffer, 0);


	m_pGrapicDevice->m_pDeviceContext->VSSetShader(m_pVertexShader->m_pVertexShader, NULL, 0);
	m_pGrapicDevice->m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pTerrainBuffer->m_ConstantBuffer);
	//////////////////
	m_pGrapicDevice->m_pDeviceContext->PSSetShader(m_pPixelShader->m_pPixelShader, NULL, 0);
	m_pGrapicDevice->m_pDeviceContext->PSSetShaderResources(0, 1, &m_pTexture->m_pTextureRV);
	m_pGrapicDevice->m_pDeviceContext->PSSetSamplers(0, 1, &m_pTexture->m_pSamplerLinear);
	m_pTerrainBuffer->Render();
}

void CTerrain::Release(void)
{
	::Safe_Delete(m_pInfo);
	::Safe_Delete(m_pTerrainBuffer);
	::Safe_Delete(m_pVertexShader);
	::Safe_Delete(m_pPixelShader);
	::Safe_Delete(m_pTexture);
	::Safe_Delete(m_pVerTex);
}

HRESULT CTerrain::AddComponent(void)
{
	/*CComponent* pComponent = NULL;

	m_pInfo = CInfo::Create(g_vLook);
	pComponent = m_pInfo;
	if (pComponent == NULL)
		return E_FAIL;
	m_mapComponent.insert(map<wstring, CComponent*>::value_type(L"Info", pComponent));

	m_pTerrainBuffer = CRcTerrain::Create(VERTEXCOUNTX, VERTEXCOUNTZ, VERTEXINTERVAL);
	pComponent = m_pTerrainBuffer;
	if (pComponent == NULL)
		return E_FAIL;
	m_mapComponent.insert(map<wstring, CComponent*>::value_type(L"Buffer", pComponent));


	m_pTexture = CTexture::Create(L"../Resource/Terrain/Terrain0.png");
	pComponent = m_pTexture;
	if (pComponent == NULL)
		return E_FAIL;
	m_mapComponent.insert(map<wstring, CComponent*>::value_type(L"Texture", pComponent));

	m_pVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"VS");
	m_pPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"PS");;*/


	
	return S_OK;
}
