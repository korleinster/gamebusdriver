#include "stdafx.h"
#include "Flower.h"
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
#include "RenderMgr.h"
#include "ResourcesMgr.h"


CFlower::CFlower()
{
	m_pBuffer = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pTexture = NULL;
	m_pVerTex = NULL;
	m_pTerrainCol = NULL;
}


CFlower::~CFlower()
{
	Release();
}

HRESULT CFlower::Initialize(void)
{
	HRESULT		hr = NULL;

	if (FAILED(AddComponent()))
		return E_FAIL;

	list<CObj*>::iterator iter = CObjMgr::GetInstance()->m_mapObj[L"Terrain"].begin();
	list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->m_mapObj[L"Terrain"].end();

	if (iter != iter_end)
		m_pVerTex = *dynamic_cast<CTerrain*>(*iter)->GetVertex();

	CRenderMgr::GetInstance()->AddRenderGroup(TYPE_NONEALPHA, this);

	return S_OK;
}

int CFlower::Update(void)
{
	//m_pTerrainCol->CollisionTerrain(&m_pInfo->m_vPos, m_pVerTex);


	D3DXMATRIX		matBill;
	D3DXMatrixIdentity(&matBill);

	matBill = CCamera::GetInstance()->m_matView;
	ZeroMemory(&matBill.m[3][0], sizeof(D3DXVECTOR3));

	D3DXMatrixInverse(&matBill, NULL, &matBill);

	CObj::Update();
	m_pInfo->m_matWorld = matBill * m_pInfo->m_matWorld;

	//cout << m_pInfo->m_vPos.x << "/" << m_pInfo->m_vPos.y << "/" << m_pInfo->m_vPos.z << endl;
	return 0;
}

void CFlower::Render(void)
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
	m_pBuffer->Render();
}

CFlower * CFlower::Create(void)
{
	CFlower* pFlower = new CFlower;

	if (FAILED(pFlower->Initialize()))
	{
		::Safe_Delete(pFlower);
	}

	return pFlower;
}

void CFlower::Release(void)
{
	::Safe_Release(m_pInfo);
	::Safe_Release(m_pBuffer);
	::Safe_Release(m_pVertexShader);
	::Safe_Release(m_pPixelShader);
	::Safe_Release(m_pTexture);
}

HRESULT CFlower::AddComponent(void)
{
	CComponent* pComponent = NULL;

	pComponent = m_pInfo = CInfo::Create(g_vLook);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Transform", pComponent));

	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STATIC, L"Buffer_RcTex");
	m_pBuffer = dynamic_cast<CVIBuffer*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Buffer", pComponent));


	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Texture_Flower");
	m_pTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_pTerrainCol = CTerrainCol::Create();
	pComponent = m_pTerrainCol;
	if (pComponent == NULL)
		return E_FAIL;
	m_mapComponent.insert(map<wstring, CComponent*>::value_type(L"TerrainCol ", pComponent));

	m_pVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"VS");
	m_pPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"PS");;


	return S_OK;
}
