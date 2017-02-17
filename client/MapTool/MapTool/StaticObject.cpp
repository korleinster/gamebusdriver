#include "stdafx.h"
#include "StaticObject.h"
#include "StaticMesh.h"
#include "Texture.h"
#include "Info.h"
#include "VIBuffer.h"
#include "RcTex.h"
#include "ShaderMgr.h"
#include "Camera.h"
#include "ObjMgr.h"
#include "Shader.h"
#include "Device.h"
#include "Camera.h"
#include "ResourcesMgr.h"


CStaticObject::CStaticObject()
{
	m_pBuffer = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pTexture = NULL;
	m_pVerTex = NULL;
}


CStaticObject::~CStaticObject()
{
	Release();
}

HRESULT CStaticObject::Initialize(const TCHAR* pMeshKey, D3DXVECTOR3 vPos)
{
	_tcscpy_s(m_tcKey, pMeshKey);
	_tcscpy_s(m_tcMeshKey, pMeshKey);

	_tcscat_s(m_tcKey, L".png");



	if (FAILED(AddComponent(pMeshKey, m_tcKey)))
		return E_FAIL;

	m_pInfo->m_vPos = vPos;

	m_pInfo->m_vScale = D3DXVECTOR3(0.05f, 0.05f, 0.05f);

	//m_pInfo->m_fAngle[ANGLE_X] = /*D3DX_PI / 2 * -1.f;*/D3DXToRadian(-90);
	//m_pInfo->m_vScale = D3DXVECTOR3(0.01f, 0.01f, 0.01f);

	return S_OK;
}

int CStaticObject::Update(void)
{
	D3DXVec3TransformNormal(&m_pInfo->m_vDir, &g_vLook, &m_pInfo->m_matWorld);
	CObj::Update();

	return 0;
}

void CStaticObject::Render(void)
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

CStaticObject * CStaticObject::Create(const TCHAR* pMeshKey, D3DXVECTOR3 vPos)
{
	CStaticObject* pObj = new CStaticObject;
	if (FAILED(pObj->Initialize(pMeshKey,vPos)))
		::Safe_Delete(pObj);

	return pObj;
}

void CStaticObject::Release(void)
{
	Safe_Release(m_pBuffer);
	Safe_Release(m_pInfo);
	Safe_Release(m_pTexture);
}

HRESULT CStaticObject::AddComponent(const TCHAR* pMeshKey, const TCHAR* pTextureKey)
{
	CComponent* pComponent = NULL;

	//TransForm
	pComponent = m_pInfo = CInfo::Create(g_vLook);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Transform", pComponent));

	//StaticMesh
	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, pMeshKey);
	m_pBuffer = dynamic_cast<CStaticMesh*>(pComponent);
	NULL_CHECK_RETURN(m_pBuffer, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Mesh", pComponent));

	//Texture
	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, pTextureKey);
	m_pTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(m_pTexture, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Texture", pComponent));

	m_pVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"VS");
	m_pPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"PS");


	return S_OK;
}