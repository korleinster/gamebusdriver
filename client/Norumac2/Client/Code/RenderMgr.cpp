#include "stdafx.h"
#include "RenderMgr.h"
#include "Obj.h"
#include "Scene.h"
#include "Device.h"
#include "TargetMgr.h"
#include "LightMgr.h"
#include "MultiRenderTarget.h"
#include "TimeMgr.h"
#include "Input.h"
#include "camera.h"
#include "Shader.h"
#include "ShaderMgr.h"
#include "RcTex.h"
#include "ResourcesMgr.h"

#pragma pack(push,1)
struct CB_LINEAR_DEPTH
{
	D3DXVECTOR4 PerspectiveValues;
};
#pragma pack(pop)

IMPLEMENT_SINGLETON(CRenderMgr)

CRenderMgr::CRenderMgr()
	:m_fTime(0.f)
	, m_dwCount(0)
	, m_pDevice(CDevice::GetInstance())
	, m_pTargetMgr(CTargetMgr::GetInstance())
	, m_pLightMgr(CLightMgr::GetInstance())
	, m_bRenderGBuffer(false)
	, m_bDefferdOn(true)
	, m_fTimeCheck(0.f)
	, m_pBorderLineVS(nullptr)
	, m_pBorderLinePS(nullptr)
	, m_pRcTex(nullptr)
	, m_pSamplerState(nullptr)
	, m_pLinearDepthCB(nullptr)
	, m_fSobelValue(0.f)
{
	ZeroMemory(m_szFps, sizeof(TCHAR) * 128);

	m_pTargetMgr->Initialize();
	m_pLightMgr->Initialize();

	m_fDirColor[RGB_RED] = 0.8f;
	m_fDirColor[RGB_GREEN] = 0.8f;
	m_fDirColor[RGB_BLUE] = 0.8f;

	m_vDirLight = D3DXVECTOR3(-0.2f, -1.0f, -0.0f);

	m_pLightMgr->SetAmbient(D3DXVECTOR3(0.4f, 0.5f, 0.4f), D3DXVECTOR3(0.4f, 0.5f, 0.4f));
	m_pLightMgr->SetDirectional(
		D3DXVECTOR3(m_vDirLight.x, m_vDirLight.y, m_vDirLight.z),
		D3DXVECTOR3(m_fDirColor[RGB_RED], m_fDirColor[RGB_GREEN], m_fDirColor[RGB_BLUE]));

	m_pBorderLineVS = CShaderMgr::GetInstance()->Clone_Shader(L"BorderLineVS");
	m_pBorderLinePS = CShaderMgr::GetInstance()->Clone_Shader(L"BorderLinePS");

	m_pRcTex = dynamic_cast<CVIBuffer*>(CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STATIC, L"Buffer_RcTex"));

	//Create Sampler State
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	CDevice::GetInstance()->m_pDevice->CreateSamplerState(&sampDesc, &m_pSamplerState);

	D3D11_BUFFER_DESC cbDesc;
	ZeroMemory(&cbDesc, sizeof(cbDesc));
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.ByteWidth = sizeof(CB_LINEAR_DEPTH);
	CDevice::GetInstance()->m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pLinearDepthCB);
}


CRenderMgr::~CRenderMgr()
{
	m_pTargetMgr->DestroyInstance();
	m_pLightMgr->DestroyInstance();

	Safe_Delete(m_pBorderLineVS);
	Safe_Delete(m_pBorderLinePS);
	Safe_Delete(m_pRcTex);

	Safe_Release(m_pSamplerState);
	Safe_Release(m_pLinearDepthCB);
}

void CRenderMgr::SetCurrentScene(CScene * pScene)
{
}

void CRenderMgr::AddRenderGroup(RENDERGROUP eRednerID, CObj* pObj)
{
	NULL_CHECK_RETURN(pObj, );

	m_RenderGroup[eRednerID].push_back(pObj);
}

void CRenderMgr::DelRenderGroup(RENDERGROUP eRednerID, CObj* pObj)
{
	list<CObj*>::iterator	iter = m_RenderGroup[eRednerID].begin();
	list<CObj*>::iterator	iter_end = m_RenderGroup[eRednerID].end();

	for (; iter != iter_end; )
	{
		if ((*iter) == pObj)
			iter = m_RenderGroup[eRednerID].erase(iter);		
		else
			++iter;

	}
}

HRESULT CRenderMgr::InitScene(void)
{
	return S_OK;
}

void CRenderMgr::Render(const float & fTime)
{
	// 키세팅
	Input(fTime);

	if (m_bDefferdOn)
	{
		// 디렉셔널 갱신
		m_pLightMgr->SetDirectional(
			D3DXVECTOR3(m_vDirLight.x, m_vDirLight.y, m_vDirLight.z),
			D3DXVECTOR3(m_fDirColor[RGB_RED], m_fDirColor[RGB_GREEN], m_fDirColor[RGB_BLUE]));

		// 조명 클리어
		//m_pLightMgr->ClearLights();
		//// 점조명 갱신
		/*for (int i = 0; i < 100; ++i)
		{
			if (i % 3 == 0)
				m_pLightMgr->AddPointLight(D3DXVECTOR3(0.f, 20.f, 0.f + i * 5), 20.f, D3DXVECTOR3(0.0f, 0.0f, i));
			else if (i % 3 == 1)
				m_pLightMgr->AddPointLight(D3DXVECTOR3(0.f, 0.f + i * 5, 0.f), 20.f, D3DXVECTOR3(0.0f, i, 0.0f));
			else if (i % 3 == 2)
				m_pLightMgr->AddPointLight(D3DXVECTOR3(0.f + i * 5, 20.f, 0.f), 20.f, D3DXVECTOR3(i, 0.0f, 0.0f));
		}*/
	}		

	ID3D11DepthStencilState* pPrevDepthState;
	UINT nPrevStencil;
	m_pDevice->m_pDeviceContext->OMGetDepthStencilState(&pPrevDepthState, &nPrevStencil);

	// Set render resources
	//m_pDevice->m_pDeviceContext->PSSetSamplers(0, 1, &g_pSampLinear);

	m_pTargetMgr->GetGBuffer()->Begin_MRT(m_pDevice->m_pDeviceContext);

	// GBuffer	
	if (m_bDefferdOn)
	{		
		Render_Priority();
		Render_NoneAlpha();		
		
	}		

	m_pTargetMgr->GetGBuffer()->End_MRT(m_pDevice->m_pDeviceContext);

	Render_BorderLine();

	// Set the render target and do the lighting
	m_pDevice->m_pDeviceContext->OMSetRenderTargets(1, &m_pDevice->m_pRenderTargetView, m_pTargetMgr->GetGBuffer()->GetDepthReadOnlyDSV());

	m_pTargetMgr->GetGBuffer()->PrepareForUnpack(m_pDevice->m_pDeviceContext);
	m_pLightMgr->DoLighting(m_pDevice->m_pDeviceContext, m_pTargetMgr->GetGBuffer());

	if (m_bRenderGBuffer)
	{
		//m_pLightMgr->DoDebugLightVolume(m_pDevice->m_pDeviceContext);
		m_pTargetMgr->RenderGBuffer(m_pDevice->m_pDeviceContext);
	}

	m_pDevice->m_pDeviceContext->OMSetRenderTargets(1, &m_pDevice->m_pRenderTargetView, m_pTargetMgr->GetGBuffer()->GetDepthDSV());

	m_pDevice->m_pDeviceContext->OMSetDepthStencilState(pPrevDepthState, nPrevStencil);
	Safe_Release(pPrevDepthState);

	if(!m_bDefferdOn)
	{ 
		Render_Priority();
		Render_NoneAlpha();
	}		

	Render_Alpha();

	if(!m_bRenderGBuffer)
		Render_UI();

	Render_FPS(fTime);
}

void CRenderMgr::Render_Priority(void)
{
	//m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	//m_pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

	list<CObj*>::iterator	iter = m_RenderGroup[TYPE_PRIORITY].begin();
	list<CObj*>::iterator	iter_end = m_RenderGroup[TYPE_PRIORITY].end();

	for (; iter != iter_end;)
	{
		if ((*iter) == NULL)
			iter = m_RenderGroup[TYPE_PRIORITY].erase(iter);
		else
		{
			(*iter)->Render();
			++iter;
		}
	}

	//m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	//m_pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
}

void CRenderMgr::Render_NoneAlpha(void)
{
	list<CObj*>::iterator	iter = m_RenderGroup[TYPE_NONEALPHA].begin();
	list<CObj*>::iterator	iter_end = m_RenderGroup[TYPE_NONEALPHA].end();

	for (; iter != iter_end;)
	{
		if ((*iter) == NULL)
			iter = m_RenderGroup[TYPE_NONEALPHA].erase(iter);
		else
		{
			(*iter)->Render();
			++iter;
		}
	}
}

bool Compare_ViewZ(CObj* pSour, CObj* pDest)
{
	return pSour->GetViewZ() > pDest->GetViewZ();
}

void CRenderMgr::Render_Alpha(void)
{
	m_RenderGroup[TYPE_ALPHA].sort(Compare_ViewZ);

	// 	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	// 	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	// 	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	// 
	// 	m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	// 	m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	// 	m_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

	list<CObj*>::iterator	iter = m_RenderGroup[TYPE_ALPHA].begin();
	list<CObj*>::iterator	iter_end = m_RenderGroup[TYPE_ALPHA].end();

	for (; iter != iter_end; iter)
	{
		if ((*iter) == NULL)
			iter = m_RenderGroup[TYPE_ALPHA].erase(iter);
		else
		{
			(*iter)->Render();
			++iter;
		}
	}
}

void CRenderMgr::Render_UI(void)
{
	//m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	//m_pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	list<CObj*>::iterator	iter = m_RenderGroup[TYPE_UI].begin();
	list<CObj*>::iterator	iter_end = m_RenderGroup[TYPE_UI].end();

		for (; iter != iter_end; iter)
		{
			if ((*iter) == NULL)
				m_RenderGroup[TYPE_UI].erase(iter);
			else
			{
				(*iter)->Render();
				++iter;
			}
		}
	//m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	//m_pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
}

void CRenderMgr::Render_FPS(const float & fTime)
{
	m_fTime += fTime;
	++m_dwCount;

	if (m_fTime >= 1.f)
	{
		wsprintf(m_szFps, L"FPS : %d", m_dwCount);
		m_fTime = 0.f;
		m_dwCount = 0;
	}
	::SetWindowText(g_hWnd, m_szFps);
}

void CRenderMgr::Render_BorderLine(void)
{
	ID3D11DeviceContext* pd3dImmediateContext = m_pDevice->m_pDeviceContext;

	// Fill the GBuffer unpack constant buffer
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	// 상수 버퍼의 내용을 쓸 수 있도록 잠금
	pd3dImmediateContext->Map(m_pLinearDepthCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	// 상수 버퍼의 데이터에 대한 포인터를 가져옴
	CB_LINEAR_DEPTH* pGBufferUnpackCB = (CB_LINEAR_DEPTH*)MappedResource.pData;
	const D3DXMATRIX* pProj = CCamera::GetInstance()->GetProjMatrix();

	// 상수 버퍼에 값을 복사
	pGBufferUnpackCB->PerspectiveValues.x = m_fSobelValue;
	pGBufferUnpackCB->PerspectiveValues.y = 1.0f / pProj->m[1][1];
	pGBufferUnpackCB->PerspectiveValues.z = pProj->m[3][2];
	pGBufferUnpackCB->PerspectiveValues.w = -pProj->m[2][2];

	// 상수 버퍼의 잠금을 품
	pd3dImmediateContext->Unmap(m_pLinearDepthCB, 0);

	// 픽셀 셰이더의 상수 버퍼를 바뀐 값으로 바꿈
	pd3dImmediateContext->PSSetConstantBuffers(0, 1, &m_pLinearDepthCB);

	m_pTargetMgr->GetBorderMRT()->Begin_MRT(pd3dImmediateContext);

	pd3dImmediateContext->IASetInputLayout(m_pBorderLineVS->m_pVertexLayout);

	pd3dImmediateContext->VSSetShader(m_pBorderLineVS->m_pVertexShader, NULL, 0);

	pd3dImmediateContext->PSSetShader(m_pBorderLinePS->m_pPixelShader, NULL, 0);
	ID3D11ShaderResourceView* pDepthSRV = m_pTargetMgr->GetGBuffer()->GetDepthView();
	pd3dImmediateContext->PSSetShaderResources(0, 1, &pDepthSRV);
	pd3dImmediateContext->PSSetSamplers(0, 1, &m_pSamplerState);

	m_pRcTex->Render();

	m_pTargetMgr->GetBorderMRT()->End_MRT(pd3dImmediateContext);
}

void CRenderMgr::Release(void)
{
}

void CRenderMgr::ListClear(void)
{
	for (UINT i = 0; i < TYPE_END; ++i)
	{
		m_RenderGroup[i].clear();
	}
}

void CRenderMgr::Input(float fTime)
{
	m_fTimeCheck += fTime;

	if (m_fTimeCheck >= 0.5f)
	{
		if (CInput::GetInstance()->GetDIKeyState(DIK_TAB) & 0x80)
		{
			if (m_bRenderGBuffer)
				m_bRenderGBuffer = false;
			else
				m_bRenderGBuffer = true;
			m_fTimeCheck = 0.f;
		}
	}

	if (m_fTimeCheck >= 0.5f)
	{
		if (CInput::GetInstance()->GetDIKeyState(DIK_0) & 0x80)
		{
			if (m_bDefferdOn)
				m_bDefferdOn = false;
			else
				m_bDefferdOn = true;
			m_fTimeCheck = 0.f;
		}
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_Z) & 0x80)
	{
		m_fSobelValue += 0.0001f;
		cout << "SobelValue : " << m_fSobelValue << endl;
	}

	if (CInput::GetInstance()->GetDIKeyState(DIK_X) & 0x80)
	{
		m_fSobelValue -= 0.0001f;
		cout << "SobelValue : " << m_fSobelValue << endl;
	}

	// For.Red
	if (CInput::GetInstance()->GetDIKeyState(DIK_R) & 0x80)
	{
		if (CInput::GetInstance()->GetDIKeyState(DIK_UP) & 0x80)
		{
			if (1.0f <= m_fDirColor[RGB_RED])
				m_fDirColor[RGB_RED] = 1.0f;
			else
				m_fDirColor[RGB_RED] += 0.1f * fTime;

			cout << "Red : " << m_fDirColor[RGB_RED] << "\t" << "Green : " << m_fDirColor[RGB_GREEN] << "\t" << "Blue : " << m_fDirColor[RGB_BLUE] << endl;
		}
		if (CInput::GetInstance()->GetDIKeyState(DIK_DOWN) & 0x80)
		{
			if (0.0f >= m_fDirColor[RGB_RED])
				m_fDirColor[RGB_RED] = 0.0f;
			else
				m_fDirColor[RGB_RED] -= 0.1f * fTime;

			cout << "Red : " << m_fDirColor[RGB_RED] << "\t" << "Green : " << m_fDirColor[RGB_GREEN] << "\t" << "Blue : " << m_fDirColor[RGB_BLUE] << endl;
		}
	}

	// For.Green
	if (CInput::GetInstance()->GetDIKeyState(DIK_G) & 0x80)
	{
		if (CInput::GetInstance()->GetDIKeyState(DIK_UP) & 0x80)
		{
			if (1.0f <= m_fDirColor[RGB_GREEN])
				m_fDirColor[RGB_GREEN] = 1.0f;
			else
				m_fDirColor[RGB_GREEN] += 0.1f * fTime;

			cout << "Red : " << m_fDirColor[RGB_RED] << "\t" << "Green : " << m_fDirColor[RGB_GREEN] << "\t" << "Blue : " << m_fDirColor[RGB_BLUE] << endl;
		}
		if (CInput::GetInstance()->GetDIKeyState(DIK_DOWN) & 0x80)
		{
			if (0.0f >= m_fDirColor[RGB_GREEN])
				m_fDirColor[RGB_GREEN] = 0.0f;
			else
				m_fDirColor[RGB_GREEN] -= 0.1f * fTime;

			cout << "Red : " << m_fDirColor[RGB_RED] << "\t" << "Green : " << m_fDirColor[RGB_GREEN] << "\t" << "Blue : " << m_fDirColor[RGB_BLUE] << endl;
		}
	}

	// For.Blue
	if (CInput::GetInstance()->GetDIKeyState(DIK_B) & 0x80)
	{
		if (CInput::GetInstance()->GetDIKeyState(DIK_UP) & 0x80)
		{
			if (1.0f <= m_fDirColor[RGB_BLUE])
				m_fDirColor[RGB_BLUE] = 1.0f;
			else
				m_fDirColor[RGB_BLUE] += 0.1f * fTime;

			cout << "Red : " << m_fDirColor[RGB_RED] << "\t" << "Green : " << m_fDirColor[RGB_GREEN] << "\t" << "Blue : " << m_fDirColor[RGB_BLUE] << endl;
		}
		if (CInput::GetInstance()->GetDIKeyState(DIK_DOWN) & 0x80)
		{
			if (0.0f >= m_fDirColor[RGB_BLUE])
				m_fDirColor[RGB_BLUE] = 0.0f;
			else
				m_fDirColor[RGB_BLUE] -= 0.1f * fTime;

			cout << "Red : " << m_fDirColor[RGB_RED] << "\t" << "Green : " << m_fDirColor[RGB_GREEN] << "\t" << "Blue : " << m_fDirColor[RGB_BLUE] << endl;
		}
	}

	if (CInput::GetInstance()->GetDIMouseState(CInput::DIM_RBUTTON))
	{

		float fDX = (float)CInput::GetInstance()->GetDIMouseMove(CInput::DIM_X);
		float fDY = (float)CInput::GetInstance()->GetDIMouseMove(CInput::DIM_Y);

		D3DXMATRIX viewInvMat;
		D3DXMatrixInverse(&viewInvMat, NULL, CCamera::GetInstance()->GetViewMatrix());
		D3DXVECTOR3 vRight = D3DXVECTOR3(viewInvMat._11, viewInvMat._12, viewInvMat._13);
		D3DXVECTOR3 vUp = D3DXVECTOR3(viewInvMat._21, viewInvMat._22, viewInvMat._23);
		D3DXVECTOR3 vForward = D3DXVECTOR3(viewInvMat._31, viewInvMat._32, viewInvMat._33);

		m_vDirLight -= vRight * fDX * fTime;
		m_vDirLight -= vUp * fDY * fTime;
		m_vDirLight += vForward * fDY * fTime;

		D3DXVec3Normalize(&m_vDirLight, &m_vDirLight);

		cout << "m_vDirLight.x : " << m_vDirLight.x << " " << "m_vDirLight.y : " << m_vDirLight.y << " " << "m_vDirLight.z : " << m_vDirLight.z << endl;
	}
}