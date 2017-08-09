#include "stdafx.h"
#include "Effect.h"
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
#include "DynamicMesh.h"
#include "Input.h"
#include "TimeMgr.h"
#include "RenderMgr.h"
#include "ResourcesMgr.h"
#include "AnimationMgr.h"

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

CEffect::CEffect()
{
	m_pBuffer = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pTexture = NULL;
	m_pVerTex = NULL;


	m_pSceneVertexShaderCB = NULL;
	m_pScenePixelShaderCB = NULL;

	m_fRenderTime = 0.f;
}


CEffect::~CEffect()
{
	CObj::Release();

	/*DWORD ReleasePoint = m_pBuffer->Release();
	if (ReleasePoint  == 1)
	::Safe_Delete(m_pBuffer);*/

	CRenderMgr::GetInstance()->DelRenderGroup(TYPE_NONEALPHA, this);
}

HRESULT CEffect::Initialize(wstring wstMeshKey, wstring wstrTextureKey , D3DXVECTOR3 vPos)
{
	m_pBuffer = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pTexture = NULL;
	m_pVerTex = NULL;

	m_pSceneVertexShaderCB = NULL;
	m_pScenePixelShaderCB = NULL;

	if (FAILED(AddComponent(wstMeshKey, wstrTextureKey)))
		return E_FAIL;

	//m_pInfo->m_vScale = D3DXVECTOR3(0.1f, 0.1f, 0.1f);
	m_pInfo->m_vPos = vPos;
	//m_pInfo->m_vScale = D3DXVECTOR3(10.f, 10.f, 10.f);
	//m_pInfo->m_fAngle[ANGLE_X] = /*D3DX_PI / 2 * -1.f;*/D3DXToRadian(-90);

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

	//cbDesc.ByteWidth = sizeof(D3DXMATRIX);
	//FAILED_CHECK(m_pGrapicDevice->m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pCascadedShadowGenVertexCB));

	return S_OK;
}

int CEffect::Update(void)

{

	D3DXVec3TransformNormal(&m_pInfo->m_vDir, &g_vLook, &m_pInfo->m_matWorld);

	//SetCurrling();

	m_fRenderTime += CTimeMgr::GetInstance()->GetTime();

	if (m_fRenderTime > 5.f)
		m_bDeath = true;

	CObj::Update();

	if (!m_bDeath)
		return 0;

	else
		return 100;




}

void CEffect::Render(void)
{
	if (m_bCurred == false)
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

		dynamic_cast<CDynamicMesh*>(m_pBuffer)->PlayAnimation(0,40.f);
	}
}

void  CEffect::ShadowmapRender(void)
{
}

CEffect * CEffect::Create(wstring wstMeshKey, wstring wstrTextureKey, D3DXVECTOR3 vPos)
{
	CEffect* pObj = new CEffect;
	if (FAILED(pObj->Initialize(wstMeshKey, wstrTextureKey,  vPos)))
		::Safe_Delete(pObj);

	return pObj;
}


HRESULT CEffect::AddComponent(wstring wstMeshKey, wstring wstrTextureKey)
{
	CComponent* pComponent = NULL;

	vecAniName = *(CAnimationMgr::GetInstance()->GetAnimaiton(wstMeshKey));


	//TransForm
	pComponent = m_pInfo = CInfo::Create(g_vLook);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Transform", pComponent));



	//DynamicMesh
	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, wstMeshKey);
	m_pBuffer = dynamic_cast<CDynamicMesh*>(pComponent);
	NULL_CHECK_RETURN(m_pBuffer, E_FAIL);
	//	m_mapComponent.insert(map<wstring, CComponent*>::value_type(L"Mesh", pComponent));

	//Texture
	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, wstrTextureKey);
	m_pTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(m_pTexture, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Texture", pComponent));

	pComponent = m_pVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"RenderSceneVS_ANI");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"VS_Shader", pComponent));
	pComponent = m_pPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"RenderScenePS");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"PS_Shader", pComponent));


	return S_OK;
}

void CEffect::SetCurrling(void)
{
	auto player = CObjMgr::GetInstance()->Get_ObjList(L"Player")->begin();

	D3DXVECTOR3 vPlayerPos = (*player)->GetInfo()->m_vPos;

	if (abs(m_pInfo->m_vPos.x - vPlayerPos.x) > 30.f && abs(m_pInfo->m_vPos.z - vPlayerPos.z) > 30.f)
		m_bCurred = true;
	else
		m_bCurred = false;
}

