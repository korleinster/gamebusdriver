#include "stdafx.h"
#include "Monster.h"
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

CMonster::CMonster()
{
	m_pBuffer = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pTexture = NULL;
	m_pVerTex = NULL;


	m_pTerrainCol = NULL;
	m_ePlayerState = PLAYER_IDLE;
	m_bKey = false;
	m_pSceneVertexShaderCB = NULL;
	m_pScenePixelShaderCB = NULL;
}


CMonster::~CMonster()
{
	CObj::Release();

	/*DWORD ReleasePoint = m_pBuffer->Release();
	if (ReleasePoint  == 1)
	::Safe_Delete(m_pBuffer);*/
}

HRESULT CMonster::Initialize(void)
{
	m_pBuffer = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pTexture = NULL;
	m_pVerTex = NULL;


	m_pTerrainCol = NULL;
	m_ePlayerState = PLAYER_IDLE;
	m_bKey = false;
	m_pSceneVertexShaderCB = NULL;
	m_pScenePixelShaderCB = NULL;

	if (FAILED(AddComponent()))
		return E_FAIL;

	m_pInfo->m_vScale = D3DXVECTOR3(1.f, 1.f, 1.f);
	//m_pInfo->m_vScale = D3DXVECTOR3(10.f, 10.f, 10.f);
	//m_pInfo->m_fAngle[ANGLE_X] = /*D3DX_PI / 2 * -1.f;*/D3DXToRadian(-90);

	list<CObj*>::iterator iter = CObjMgr::GetInstance()->m_mapObj[L"Terrain"].begin();
	list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->m_mapObj[L"Terrain"].end();

	if (iter != iter_end)
		m_pVerTex = *dynamic_cast<CTerrain*>(*iter)->GetVertex();

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

	return S_OK;
}

int CMonster::Update(void)

{
	m_fSeverTime += CTimeMgr::GetInstance()->GetTime();
	m_pTerrainCol->CollisionTerrain(&m_pInfo->m_vPos, m_pVerTex);
	/*if (m_pBuffer->m_bAniEnd == true)
	m_pBuffer->m_bAniEnd = false;*/

	//m_ePlayerState = PLAYER_IDLE;

	m_pInfo->m_vPos.x = m_pInfo->m_ServerInfo.pos.x;
	m_pInfo->m_vPos.z = m_pInfo->m_ServerInfo.pos.y;
	D3DXVec3TransformNormal(&m_pInfo->m_vDir, &g_vLook, &m_pInfo->m_matWorld);
	if (m_ePlayerState != PLAYER_IDLE)
	{
		//if (dynamic_cast<CDynamicMesh*>(m_pBuffer)->m_bAniEnd == true) // ���Ⱑ �߸����
		if (m_bKey == false)
			m_ePlayerState = PLAYER_IDLE;
	}


	//cout <<"OtherPlayer pos: " << m_pInfo->m_vPos.x << "/" << m_pInfo->m_vPos.y << "/" << m_pInfo->m_vPos.z << endl;

	//if (m_pInfo->m_ServerInfo.state.hp < 100) {	cout << "�ٸ� �÷��̾�" << m_pInfo->m_ServerInfo.id << "���� ü��:" << m_pInfo->m_ServerInfo.state.hp << endl; }

	CObj::Update();




	if (!m_bDeath)
		return 0;

	else
		return 100;
}

void CMonster::Render(void)
{

	//ConstantBuffer cb;
	//D3DXMatrixTranspose(&cb.matWorld, &m_pInfo->m_matWorld);
	//D3DXMatrixTranspose(&cb.matView, &CCamera::GetInstance()->m_matView);
	//D3DXMatrixTranspose(&cb.matProjection, &CCamera::GetInstance()->m_matProj);
	//m_pGrapicDevice->m_pDeviceContext->UpdateSubresource(m_pBuffer->m_ConstantBuffer, 0, NULL, &cb, 0, 0);

	//m_pGrapicDevice->m_pDeviceContext->VSSetShader(m_pVertexShader->m_pVertexShader, NULL, 0);
	//m_pGrapicDevice->m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pBuffer->m_ConstantBuffer);
	////////////////////
	//m_pGrapicDevice->m_pDeviceContext->PSSetShader(m_pPixelShader->m_pPixelShader, NULL, 0);
	//m_pGrapicDevice->m_pDeviceContext->PSSetShaderResources(0, 1, &m_pTexture->m_pTextureRV);
	//m_pGrapicDevice->m_pDeviceContext->PSSetSamplers(0, 1, &m_pTexture->m_pSamplerLinear);

	////m_pBuffer->Render();
	//dynamic_cast<CDynamicMesh*>(m_pBuffer)->PlayAnimation(m_ePlayerState);

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

	dynamic_cast<CDynamicMesh*>(m_pBuffer)->PlayAnimation(m_ePlayerState);
}

CMonster * CMonster::Create(void)
{
	CMonster* pObj = new CMonster;
	if (FAILED(pObj->Initialize()))
		::Safe_Delete(pObj);

	return pObj;
}


HRESULT CMonster::AddComponent(void)
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


	vector<string> vecName;
	vecAniName.push_back("player_normalidle");
	//DynamicMesh
	//m_pBuffer = CDynamicMesh::Create("../Resource", vecAniName);//dynamic_cast<CDynamicMesh*>(pComponent);
	//pComponent = m_pBuffer; //CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Player_IDLE");
	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Player");
	m_pBuffer = dynamic_cast<CDynamicMesh*>(pComponent);
	NULL_CHECK_RETURN(m_pBuffer, E_FAIL);
	//	m_mapComponent.insert(map<wstring, CComponent*>::value_type(L"Mesh", pComponent));

	//Texture
	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Texture_Player");
	m_pTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(m_pTexture, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Texture", pComponent));

	pComponent = m_pVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"RenderSceneVS_ANI");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"VS_Shader", pComponent));
	pComponent = m_pPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"RenderScenePS");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"PS_Shader", pComponent));


	return S_OK;
}

