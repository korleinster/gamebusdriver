#include "stdafx.h"
#include "Player.h"
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



CPlayer::CPlayer()
{
	m_pBuffer = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pTexture = NULL;
	m_pVerTex = NULL;
	m_pTerrainCol = NULL;
}


CPlayer::~CPlayer()
{
	Release();
}

HRESULT CPlayer::Initialize(void)
{
	if (FAILED(AddComponent()))
		return E_FAIL;

	
	return S_OK;
}

int CPlayer::Update(void)
{
	bool test = false;

	if (test == false)
	{
		D3DXQUATERNION quatRot;
		D3DXQuaternionIdentity(&quatRot);
		D3DXQuaternionRotationYawPitchRoll(&quatRot, 0.f, 0.f, D3DXToRadian(-90.f));
		D3DXMATRIX matOrientation;
		D3DXMatrixIdentity(&matOrientation);
		D3DXMatrixAffineTransformation(&matOrientation, 1.25f, NULL, &quatRot, &CCamera::GetInstance()->m_vEye);

		D3DXMatrixInverse(&m_pInfo->m_matWorld, NULL, &matOrientation);

		test = true;

	}

	D3DXQUATERNION quatRot;
	D3DXQuaternionIdentity(&quatRot);
	D3DXQuaternionRotationYawPitchRoll(&quatRot, -m_pInfo->m_fAngle[ANGLE_Y], 0.f, 0.f);
	D3DXMATRIX matOrientation;
	D3DXMatrixIdentity(&matOrientation);
	D3DXMatrixAffineTransformation(&matOrientation, 1.25f, NULL, &quatRot, &CCamera::GetInstance()->m_vEye);

	D3DXMatrixInverse(&m_pInfo->m_matWorld, NULL, &matOrientation);
	

	/*if (m_pBuffer->m_bAniEnd == true)
		m_pBuffer->m_bAniEnd = false;*/
	m_pInfo->m_fAngle[ANGLE_X] = /*D3DX_PI / 2 * -1.f;*/D3DXToRadian(-90);
	m_pInfo->m_vScale = D3DXVECTOR3(0.01f, 0.01f, 0.01f);
	D3DXVec3TransformNormal(&m_pInfo->m_vDir, &g_vLook, &m_pInfo->m_matWorld);

	cout << m_pInfo->m_vDir.x << "/" << m_pInfo->m_vDir.y << "/" << m_pInfo->m_vDir.z << endl;

	KeyInput();
	CObj::Update();
	return 0;
}

void CPlayer::Render(void)
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

CPlayer * CPlayer::Create(void)
{
	CPlayer* pObj = new CPlayer;
	if (FAILED(pObj->Initialize()))
		::Safe_Delete(pObj);

	return pObj;
}

void CPlayer::Release(void)
{
}

HRESULT CPlayer::AddComponent(void)
{
	CComponent* pComponent = NULL;
	char cModelPath[MAX_PATH] = "../Resource/bird.fbx";

	//////////////임시 다이나믹 ///////////////

	//vecAniName.push_back("bird");

	///////////////////////////////////////////

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


void CPlayer::KeyInput()
{
	float		fTime = CTimeMgr::GetInstance()->GetTime();


	if (CInput::GetInstance()->GetDIKeyState(DIK_UP) & 0x80)
	{
		m_pInfo->m_vPos += m_pInfo->m_vDir * 5.f * fTime;
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_DOWN) & 0x80)
	{
		m_pInfo->m_vPos -= m_pInfo->m_vDir * 5.f * fTime;
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_LEFT) & 0x80)
	{
		m_pInfo->m_fAngle[ANGLE_Y] -= D3DXToRadian(90.f * fTime);
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_RIGHT) & 0x80)
	{
		m_pInfo->m_fAngle[ANGLE_Y] += D3DXToRadian(90.f * fTime);
	}
}
