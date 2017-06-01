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
	, m_bDead(false)
{

}

CDamageFont::~CDamageFont(void)
{
	CObj::Release();
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

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	D3DXMatrixIdentity(&m_matView);
	m_matView._11 = m_fSizeX;
	m_matView._22 = m_fSizeY;
	m_matView._33 = 1.f;

	// -0.5	-> -400		,	0.5	-> 400
	m_matView._41 = m_fX - (WINCX >> 1);
	m_matView._42 = -m_fY + (WINCY >> 1);



	m_fTime += CTimeMgr::GetInstance()->GetTime();

	if (m_fTime > 1.3f)
		m_bDead = true;

	m_fAlpha = 1.f - (m_fTime / 1.3f);

	m_pInfo->m_vPos = m_vPos;

	m_fUpSpeed -= 1.8f * CTimeMgr::GetInstance()->GetTime();

	m_fScaleRatio = 1.0f;

	return 0;
}

void CDamageFont::Render(void)
{
	//m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	//LPD3DXEFFECT		pEffect = m_pShader->Get_EffectHandle();
	//if (NULL == pEffect)
	//	return;

	////CDamageFont::Set_ConstantTable(pEffect);

	//_matrix			matProj, matView2;
	//m_pDevice->GetTransform(D3DTS_VIEW, &matView2);
	//m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);


	//_matrix			matWorld2;
	//D3DXMatrixIdentity(&matWorld2);

	//// 직교투영을 수행해주는 함수다.
	////D3DXMatrixOrthoLH(&matProj, WINCX, WINCY, 0.f, 1.f);

	////pEffect->SetMatrix("g_matWorld", &matWorld);
	////pEffect->SetMatrix("g_matProj", &matProj);

	//pEffect->SetFloat("g_fAlpha", m_fAlpha);
	//pEffect->SetFloat("g_fPointPower", 1.f);

	//_int	iDmg = m_iDmg;
	//_uint	iCnt = m_iCnt / 10;
	//while (true)
	//{
	//	_int	iNum;
	//	if (0 == iDmg)
	//		iNum = 0;
	//	else
	//		iNum = iDmg / iCnt;

	//	_mymat matScale, matRotX, matRotY, matRotZ, matTrans, matParent;

	//	matScale.Scale(D3DXVECTOR3(0.03f, 0.03f, 0.03f));

	//	matRotX.RotX(D3DXToRadian(0.f));
	//	matRotY.RotY(D3DXToRadian(0.f));
	//	matRotZ.RotZ(D3DXToRadian(0.f));


	//	matTrans.Trans(D3DXVECTOR3(m_fMoveSize, 0.005f, 0.f));

	//	matParent.Trans(m_vPos);


	//	D3DXMATRIX      matBill;
	//	D3DXMatrixIdentity(&matBill);

	//	m_pDevice->GetTransform(D3DTS_VIEW, &matBill);
	//	ZeroMemory(&matBill.m[3][0], sizeof(D3DXVECTOR3));

	//	D3DXMatrixInverse(&matBill, 0, &matBill);

	//	matParent = matBill * matParent;

	//	matWorld2 = matScale * matRotX * matRotY * matRotZ * matTrans * matParent;

	//	m_fMoveSize += 0.04f;

	//	pEffect->SetMatrix("g_matWorld", &matWorld2);
	//	pEffect->SetMatrix("g_matView", &matView2);
	//	pEffect->SetMatrix("g_matProj", &matProj);

	//	//pEffect->SetFloat("g_fDrawRatio", m_fDrawRatio);
	//	m_pTexture->Set_ConstantTable(pEffect, "g_BaseTexture", iNum);
	//	_uint		iPassCnt = 0;

	//	pEffect->Begin(&iPassCnt, 0);
	//	pEffect->BeginPass(0);

	//	m_pBuffer->Render(&m_pInfo->m_matWorld);

	//	pEffect->EndPass();
	//	pEffect->End();

	//	if (1 >= iCnt)
	//		break;

	//	iDmg = iDmg - iNum * iCnt;
	//	iCnt /= 10;

	//}

	//m_fMoveSize = 0.f;
	//m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

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
}

CDamageFont* CDamageFont::Create(const D3DXVECTOR3* pPos, const float& fDmg)
{
	CDamageFont*		pInstance = new CDamageFont();

	if (FAILED(pInstance->Initialize(pPos, fDmg)))
	{
		//MSG_BOX("CDamageFont Created Failed");
		//::safe_release(pInstance);
	}

	return pInstance;
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

