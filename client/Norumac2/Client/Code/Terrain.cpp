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
#include "Frustum.h"

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
	m_dwTriCnt = 0;
}

CTerrain::~CTerrain()
{
	CObj::Release();
	//Release();
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

	//m_pVerTex = new VTXTEX[VERTEXCOUNTX * VERTEXCOUNTZ];

	//m_pGrapicDevice->m_pDeviceContext->GetData
	//D3D11_MAPPED_SUBRESOURCE MapResource;

	m_pVerTex = dynamic_cast<CRcTerrain*>(m_pTerrainBuffer)->Vertex;

	/*m_pGrapicDevice->m_pDeviceContext->Map(m_pTerrainBuffer->m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MapResource);
	memcpy(m_pVerTex, (VTXTEX*)MapResource.pData, sizeof(VTXTEX) * (VERTEXCOUNTX * VERTEXCOUNTZ));
	m_pGrapicDevice->m_pDeviceContext->Unmap(m_pTerrainBuffer->m_VertexBuffer, 0);*/

	CRenderMgr::GetInstance()->AddRenderGroup(TYPE_NONEALPHA, this);

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

int CTerrain::Update(void)
{

	CObj::Update();
	return 0;
}

void CTerrain::Render(void)
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

	m_pTerrainBuffer->Render();
}

void CTerrain::ShadowmapRender(void)
{
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	FAILED_CHECK_RETURN(m_pGrapicDevice->m_pDeviceContext->Map(m_pCascadedShadowGenVertexCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource), );
	D3DXMATRIX* pVSPerObject = (D3DXMATRIX*)MappedResource.pData;
	D3DXMatrixTranspose(pVSPerObject, &m_pInfo->m_matWorld);
	m_pGrapicDevice->m_pDeviceContext->Unmap(m_pCascadedShadowGenVertexCB, 0);
	m_pGrapicDevice->m_pDeviceContext->VSSetConstantBuffers(1, 1, &m_pCascadedShadowGenVertexCB);

	m_pTerrainBuffer->Render();
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
	//m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Buffer", pComponent));


	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Texture_Terrain");
	m_pTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Texture", pComponent));

	pComponent = m_pVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"RenderSceneVS");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"VS_Shader", pComponent));
	pComponent = m_pPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"RenderScenePS");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"PS_Shader", pComponent));

	return S_OK;
}
