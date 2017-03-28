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
#include "ResourcesMgr.h"
#include "RenderMgr.h"

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
	//D3D11_MAPPED_SUBRESOURCE MapResource;

	m_pVerTex = dynamic_cast<CRcTerrain*>(m_pTerrainBuffer)->Vertex;

	/*m_pGrapicDevice->m_pDeviceContext->Map(m_pTerrainBuffer->m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MapResource);
	memcpy(m_pVerTex, (VTXTEX*)MapResource.pData, sizeof(VTXTEX) * (VERTEXCOUNTX * VERTEXCOUNTZ));
	m_pGrapicDevice->m_pDeviceContext->Unmap(m_pTerrainBuffer->m_VertexBuffer, 0);*/

	CRenderMgr::GetInstance()->AddRenderGroup(TYPE_NONEALPHA, this);

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


	///////////////////////////////////////////////////////////////////////////////////////////////

	//// Get the projection & view matrix from the camera class
	//D3DXMATRIX mView = *(CCamera::GetInstance()->GetViewMatrix());
	//D3DXMATRIX mProj = *(CCamera::GetInstance()->GetProjMatrix());
	//D3DXMATRIX mWorldViewProjection = m_pInfo->m_matWorld * mView * mProj;

	//// Set the constant buffers
	//D3D11_MAPPED_SUBRESOURCE MappedResource;
	//FAILED_CHECK_RETURN(m_pGrapicDevice->m_pDeviceContext->Map(m_pSceneVertexShaderCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource), );
	//CB_VS_PER_OBJECT* pVSPerObject = (CB_VS_PER_OBJECT*)MappedResource.pData;
	//D3DXMatrixTranspose(&pVSPerObject->m_mWorldViewProjection, &mWorldViewProjection);
	//D3DXMatrixTranspose(&pVSPerObject->m_mWorld, &m_pInfo->m_matWorld);
	//m_pGrapicDevice->m_pDeviceContext->Unmap(m_pSceneVertexShaderCB, 0);
	//m_pGrapicDevice->m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pSceneVertexShaderCB);

	//FAILED_CHECK_RETURN(m_pGrapicDevice->m_pDeviceContext->Map(m_pScenePixelShaderCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource), );
	//CB_PS_PER_OBJECT* pPSPerObject = (CB_PS_PER_OBJECT*)MappedResource.pData;
	//pPSPerObject->m_vEyePosition = CCamera::GetInstance()->m_vEye;
	//pPSPerObject->m_fSpecExp = 250.0f;
	//pPSPerObject->m_fSpecIntensity = 0.25f;
	//m_pGrapicDevice->m_pDeviceContext->Unmap(m_pScenePixelShaderCB, 0);
	//m_pGrapicDevice->m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pScenePixelShaderCB);

	//// Set the vertex layout
	//m_pGrapicDevice->m_pDeviceContext->IASetInputLayout(m_pVertexShader->m_pVertexLayout);

	//// Set the shaders
	//m_pGrapicDevice->m_pDeviceContext->VSSetShader(m_pVertexShader->m_pVertexShader, NULL, 0);
	//m_pGrapicDevice->m_pDeviceContext->PSSetShader(m_pPixelShader->m_pPixelShader, NULL, 0);

	//m_pGrapicDevice->m_pDeviceContext->PSSetShaderResources(0, 1, &m_pTexture->m_pTextureRV);
	//m_pGrapicDevice->m_pDeviceContext->PSSetSamplers(0, 1, &m_pTexture->m_pSamplerLinear);

	//m_pTerrainBuffer->Render();
}

void CTerrain::Release(void)
{
	::Safe_Release(m_pInfo);
	::Safe_Release(m_pTerrainBuffer);
	::Safe_Release(m_pVertexShader);
	::Safe_Release(m_pPixelShader);
	::Safe_Release(m_pTexture);
	::Safe_Delete(m_pVerTex);
}

HRESULT CTerrain::AddComponent(void)
{
	CComponent* pComponent = NULL;

	pComponent = m_pInfo = CInfo::Create(g_vLook);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Transform", pComponent));

	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Buffer_RcTerrain");
	m_pTerrainBuffer = dynamic_cast<CVIBuffer*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Buffer", pComponent));


	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Texture_Terrain");
	m_pTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_pVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"RenderSceneVS");
	m_pPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"RenderScenePS");

	return S_OK;
}
