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

HRESULT CStaticObject::Initialize(void)
{
	if (FAILED(AddComponent()))
		return E_FAIL;
	m_pInfo->m_fAngle[ANGLE_X] = /*D3DX_PI / 2 * -1.f;*/D3DXToRadian(-90);
	//m_ServerInfo.pos.x = m_pInfo->m_vPos.x;
	//m_ServerInfo.pos.y = m_pInfo->m_vPos.z;
	//m_pInfo->m_vScale = D3DXVECTOR3(0.01f, 0.01f, 0.01f);



	return S_OK;
}

int CStaticObject::Update(void)
{


	D3DXVec3TransformNormal(&m_pInfo->m_vDir, &g_vLook, &m_pInfo->m_matWorld);
	m_pBuffer->CreateRasterizerState2();
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

CStaticObject * CStaticObject::Create(void)
{
	CStaticObject* pObj = new CStaticObject;
	if (FAILED(pObj->Initialize()))
		::Safe_Delete(pObj);

	return pObj;
}

void CStaticObject::Release(void)
{
	Safe_Delete(m_pBuffer);
	Safe_Delete(m_pInfo);
	Safe_Delete(m_pTexture);
}

HRESULT CStaticObject::AddComponent(void)
{
	CComponent* pComponent = NULL;
	char cModelPath[MAX_PATH] = "../Resource/Mesh/town.FBX";


	m_pBuffer = CStaticMesh::Create(cModelPath);
	pComponent = m_pBuffer;
	m_mapComponent.insert(map<wstring, CComponent*>::value_type(L"Buffer", pComponent));

	m_pInfo = CInfo::Create(g_vLook);
	pComponent = m_pInfo;
	if (pComponent == NULL)
		return E_FAIL;
	m_mapComponent.insert(map<wstring, CComponent*>::value_type(L"Info", pComponent));

	m_pTexture = CTexture::Create(L"../Resource/MeshImage/town.png");
	pComponent = m_pTexture;
	if (pComponent == NULL)
		return E_FAIL;
	m_mapComponent.insert(map<wstring, CComponent*>::value_type(L"Texture", pComponent));

	m_pVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"VS");
	m_pPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"PS");


	return S_OK;
}