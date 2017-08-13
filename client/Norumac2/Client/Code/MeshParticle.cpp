#include "stdafx.h"
#include "MeshParticle.h"
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
#include "RenderMgr.h"
#include "ResourcesMgr.h"
#include "LightMgr.h"
#include "Player.h"
#include "TimeMgr.h"

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

CMeshParticle::CMeshParticle()
{
	m_pBuffer = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pTexture = NULL;
	m_pVerTex = NULL;
	m_pSceneVertexShaderCB = NULL;
	m_pScenePixelShaderCB = NULL;
	m_fRotSpeed = 0.f;
	m_fMoveSpeed = 15.f;
}


CMeshParticle::~CMeshParticle()
{
	map<wstring, CComponent*>::iterator iter = m_mapComponent.begin();
	map<wstring, CComponent*>::iterator iter_end = m_mapComponent.end();
	for (iter; iter != iter_end; ++iter)
	{
		::Safe_Delete(iter->second);
	}
	m_mapComponent.clear();

	CRenderMgr::GetInstance()->DelRenderGroup(TYPE_NONEALPHA, this);
}

HRESULT CMeshParticle::Initialize(const TCHAR* pMeshKey,D3DXVECTOR3 vPos, PARTICLECOLOR ePColor)
{

	if (FAILED(AddComponent(pMeshKey, ePColor)))
		return E_FAIL;
	m_pInfo->m_fAngle[ANGLE_X] = D3DXToRadian(-90);

	CRenderMgr::GetInstance()->AddRenderGroup(TYPE_NONEALPHA, this);

	m_pInfo->m_vScale = D3DXVECTOR3(0.1f, 0.1f, 0.1f);
	m_vOriginPos = vPos;
	m_pInfo->m_vPos = vPos;



	

	


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

int CMeshParticle::Update(void)
{


	float fTime = CTimeMgr::GetInstance()->GetTime();
	m_pInfo->m_vDir.y -= 0.01f;
	m_pInfo->m_vPos += m_pInfo->m_vDir * m_fMoveSpeed * fTime;


	if ((abs(m_pInfo->m_vPos.x - m_vOriginPos.x) > 2.7f) ||
		(abs(m_pInfo->m_vPos.z - m_vOriginPos.z) > 2.7f) ||
		(abs(m_pInfo->m_vPos.y - m_vOriginPos.y) > 2.7f))
		return 100;


	//D3DXVec3TransformNormal(&m_pInfo->m_vDir, &g_vLook, &m_pInfo->m_matWorld);
	CObj::Update();

	return 0;
}

void CMeshParticle::Render(void)
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

		m_pBuffer->Render();
	}
}

void  CMeshParticle::ShadowmapRender(void)
{
	if (m_bCurred == false)
	{
		D3D11_MAPPED_SUBRESOURCE MappedResource;
		FAILED_CHECK_RETURN(m_pGrapicDevice->m_pDeviceContext->Map(m_pCascadedShadowGenVertexCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource), );
		D3DXMATRIX* pVSPerObject = (D3DXMATRIX*)MappedResource.pData;
		D3DXMatrixTranspose(pVSPerObject, &m_pInfo->m_matWorld);
		m_pGrapicDevice->m_pDeviceContext->Unmap(m_pCascadedShadowGenVertexCB, 0);
		m_pGrapicDevice->m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pCascadedShadowGenVertexCB);

		m_pBuffer->Render();
	}
}

CMeshParticle * CMeshParticle::Create(const TCHAR* pMeshKey,D3DXVECTOR3 vPos, PARTICLECOLOR ePColor)
{
	CMeshParticle* pObj = new CMeshParticle;
	if (FAILED(pObj->Initialize(pMeshKey,vPos,ePColor)))
		::Safe_Delete(pObj);

	return pObj;
}

HRESULT CMeshParticle::AddComponent(const TCHAR* pMeshKey, PARTICLECOLOR ePColor)
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

	//Texture

	if(ePColor == PC_WHITE)
		pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Texture_White");
	else if(ePColor == PC_RED)
		pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Texture_Red");
	else if (ePColor == PC_YELLOW)
		pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Texture_Yellow");
	else if (ePColor == PC_BLUE)
		pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Texture_Blue");
	else if (ePColor == PC_BLACK)
		pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Texture_Black");
	else if (ePColor == PC_PURPLE)
		pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Texture_Purple");
	else if (ePColor == PC_GREEN)
		pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Texture_Green");
	else if (ePColor == PC_ORANGE)
		pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Texture_Orange");

	m_pTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(m_pTexture, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Texture", pComponent));

	pComponent = m_pVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"RenderSceneVS");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"VS_Shader", pComponent));
	pComponent = m_pPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"RenderScenePS");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"PS_Shader", pComponent));

	return S_OK;
}

void CMeshParticle::SetCurrling(void)
{
	auto player = CObjMgr::GetInstance()->Get_ObjList(L"Player")->begin();

	D3DXVECTOR3 vPlayerPos = (*player)->GetInfo()->m_vPos;

	if (abs(m_pInfo->m_vPos.x - vPlayerPos.x) > 25.f || abs(m_pInfo->m_vPos.z - vPlayerPos.z) > 25.f)
	{
		m_bCurred = true;
		//cout << "컬링됨" << endl;
	}
	else
	{
		m_bCurred = false;
		//cout << "컬링해제" << endl;
	}



}