#include "stdafx.h"
#include "Include.h"
#include "UI.h"
#include "DamageFont.h"
#include "Texture.h"
#include "TimeMgr.h"
#include "RenderMgr.h"
#include "Info.h"
#include "VIBuffer.h"
#include "Device.h"
#include "Shader.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "Camera.h"

CDamageFont::CDamageFont()
	: CUI()
	, m_vPos(D3DXVECTOR3(0.f, 0.f, 0.f))
	, m_iDmg(0)
	, m_iCnt(0)
	, m_fAddY(0.f)
	, m_fTime(0.f)
	, m_fScaleRatio(1.f)
	, m_fUpSpeed(0.8f)
	, m_fRightSpeed(0.f)
	, m_fAlpha(1.f)
	, m_fMoveSize(0.f)
	, m_iRendNum(0)
{

}

CDamageFont::~CDamageFont(void)
{
	CObj::Release();
	CRenderMgr::GetInstance()->DelRenderGroup(TYPE_UI, this);//임시인데 어디다 빼야할까
}

HRESULT CDamageFont::Initialize(const D3DXVECTOR3* pPos, const float& fDmg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_fSizeX = 30.0f / 5.f;//2.8f;   
	m_fSizeY = 24.0f / 5.f;//2.8f;

	m_fX = 400.f;
	m_fY = 150.f;

	m_fViewZ = 1;

	m_vPos = *pPos;
	m_vPos.y += 0.1f;
	m_iDmg = int(fDmg);
	m_fDrawRatio = 1.f;
	unsigned int iRand = rand() % 2;
	if (0 == iRand)
		m_fRightSpeed = 0.2f;
	else
		m_fRightSpeed = -0.2f;

	srand(GetTickCount());

	// 50까지 소수
	float fTemp = (float)rand() / RAND_MAX * 1000.f;
	fTemp -= (int)fTemp;
	fTemp *= 50.f;
	m_fPosRandX = fTemp - 25.f;


	float fTemp2 = (float)rand() / RAND_MAX * 100000.f;
	fTemp2 -= (int)fTemp2;
	fTemp2 *= 50.f;
	m_fPosRandY = fTemp2 - 25.f;

	m_vPos.x += (m_fPosRandX * 0.01f);
	m_vPos.y += (m_fPosRandY * 0.002f);

	Compute_Cnt();

	CRenderMgr::GetInstance()->AddRenderGroup(TYPE_UI, this);
	return S_OK;
}

int CDamageFont::Update(void)
{
	m_fTime += CTimeMgr::GetInstance()->GetTime();

	if (m_fTime > 1.3f)
		m_bDeath = true;

	//m_fAlpha = 1.f - (m_fTime / 1.3f);//알파의 강도 조절이 가능하면 생각해보자.

	m_pInfo->m_vPos = m_vPos;

	m_fUpSpeed -= 1.8f * CTimeMgr::GetInstance()->GetTime();

	m_fScaleRatio = 1.0f;



	if (!m_bDeath)
		return 0;

	else
		return 100;



	return 0;
}

void CDamageFont::Render(void)
{

	D3DXMATRIX			matProj, matView2;
	matView2 = CCamera::GetInstance()->m_matView;
	matProj = CCamera::GetInstance()->m_matProj;


	D3DXMATRIX			matWorld2;
	D3DXMatrixIdentity(&matWorld2);

	int iDmg = m_iDmg;
	UINT iCnt = m_iCnt / 10;
	
	while (1)
	{
		int iNum;
		if (0 == iDmg)
			iNum = 0;
		else
			iNum = iDmg / iCnt;

		D3DXMATRIX matScale, matRotX, matRotY, matRotZ, matTrans, matParent;

		D3DXMatrixScaling(&matScale, 0.03f, 0.03f, 0.03f);
		D3DXMatrixRotationX(&matRotX, 0.f);
		D3DXMatrixRotationY(&matRotY, 0.f);
		D3DXMatrixRotationZ(&matRotZ, 0.f);

		D3DXMatrixTranslation(&matTrans, m_fMoveSize, 0.005f, 0.f);
		D3DXMatrixTranslation(&matParent, m_vPos.x,m_vPos.y,m_vPos.z);

		D3DXMATRIX      matBill;
		D3DXMatrixIdentity(&matBill);

		matBill = CCamera::GetInstance()->m_matView;
		ZeroMemory(&matBill.m[3][0], sizeof(D3DXVECTOR3));

		D3DXMatrixInverse(&matBill, 0, &matBill);

		matParent = matBill * matParent;

		matWorld2 = matScale * matRotX * matRotY * matRotZ * matTrans * matParent;

		m_fMoveSize += 0.04f;



		//////////////////////////////////
		ConstantBuffer cb;

		D3DXMatrixTranspose(&cb.matWorld, &m_pInfo->m_matWorld);
		D3DXMatrixTranspose(&cb.matView, &m_matView);
		D3DXMatrixTranspose(&cb.matProjection, &m_matProj);

		m_pGrapicDevice->m_pDeviceContext->UpdateSubresource(m_pBuffer->m_ConstantBuffer, 0, NULL, &cb, 0, 0);

		m_pGrapicDevice->m_pDeviceContext->IASetInputLayout(m_pVertexShader->m_pVertexLayout);

		m_pGrapicDevice->m_pDeviceContext->VSSetShader(m_pVertexShader->m_pVertexShader, NULL, 0);
		m_pGrapicDevice->m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pBuffer->m_ConstantBuffer);

		m_pGrapicDevice->m_pDeviceContext->PSSetShader(m_pPixelShader->m_pPixelShader, NULL, 0);

		m_pGrapicDevice->m_pDeviceContext->PSSetShaderResources(0, 1, &m_pTexture->m_pTextureRV);
		m_pGrapicDevice->m_pDeviceContext->PSSetSamplers(0, 1, &m_pTexture->m_pSamplerLinear);

		m_pBuffer->Render();

		if (1 >= iCnt)
			break;

		iDmg = iDmg - iNum * iCnt;
		iCnt /= 10;
		/////////////////////////////////////////
	}
	m_fMoveSize = 0.f;

}

CDamageFont* CDamageFont::Create(const D3DXVECTOR3* pPos, const float& fDmg)
{
	CDamageFont*		pDmgFont = new CDamageFont();

	if (FAILED(pDmgFont->Initialize(pPos, fDmg)))
	{
		//MSG_BOX("CDamageFont Created Failed");
		::Safe_Delete(pDmgFont);
	}

	return pDmgFont;
}

HRESULT CDamageFont::Add_Component(void)
{
	CComponent*			pComponent = NULL;

	// For.Transform Component
	pComponent = m_pInfo = CInfo::Create(g_vLook);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Transform", pComponent));

	// For.Buffer Component
	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Buffer_RcUI");
	m_pBuffer = dynamic_cast<CVIBuffer*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);

	// For.Texture Component
	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Texture_DamageFont");
	m_pTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(pComponent, E_FAIL);

	// For.Shader Component
	pComponent = m_pVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"VS");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"VS_Shader", pComponent));
	pComponent = m_pPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"PS");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"PS_Shader", pComponent));

	return S_OK;
}



void CDamageFont::Compute_Cnt(void)
{
	int		iDmg = m_iDmg;
	int		iCnt = 1;
	m_iCnt2 = 1;

	while (true)
	{
		if (0 == iDmg / iCnt)
			break;

		++m_iCnt2;
		iCnt *= 10;
	}
	m_iCnt = iCnt;
	--m_iCnt2;
}