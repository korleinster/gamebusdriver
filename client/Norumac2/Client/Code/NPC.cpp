#include "stdafx.h"
#include "Npc.h"
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
#include "TerrainCol.h"
#include "Terrain.h"
#include "ChatUI.h"
#include "Player.h"
#include "Font.h"
#include "QuestMark.h"

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

CNpc::CNpc()
{
	m_pBuffer = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pTexture = NULL;
	m_pVerTex = NULL;
	m_pTerrainCol = NULL;


	m_pSceneVertexShaderCB = NULL;
	m_pScenePixelShaderCB = NULL;

	m_eNpcType = NPC_SLIME;
	m_bPlayerIn = false;

	m_bQuestAccept = false;
	m_pQuestMark = NULL;

}


CNpc::~CNpc()
{
	CObj::Release();

	/*DWORD ReleasePoint = m_pBuffer->Release();
	if (ReleasePoint  == 1)
	::Safe_Delete(m_pBuffer);*/
}

HRESULT CNpc::Initialize(wstring wstMeshKey, wstring wstrTextureKey, D3DXVECTOR3 vPos, QUESTNPCTYPE eNpcType)
{
	m_pBuffer = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pTexture = NULL;
	m_pVerTex = NULL;

	m_pSceneVertexShaderCB = NULL;
	m_pScenePixelShaderCB = NULL;

	m_eNpcType = eNpcType;

	if (FAILED(AddComponent(wstMeshKey, wstrTextureKey)))
		return E_FAIL;

	m_pInfo->m_vScale = D3DXVECTOR3(1.f, 1.f, 1.f);
	m_pInfo->m_vPos = vPos;
	//m_pInfo->m_vScale = D3DXVECTOR3(10.f, 10.f, 10.f);
	//m_pInfo->m_fAngle[ANGLE_X] = /*D3DX_PI / 2 * -1.f;*/D3DXToRadian(-90);

	list<CObj*>::iterator iter = CObjMgr::GetInstance()->m_mapObj[L"Terrain"].begin();
	list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->m_mapObj[L"Terrain"].end();

	if (iter != iter_end)
		m_pVerTex = *dynamic_cast<CTerrain*>(*iter)->GetVertex();


	m_pQuestMark = CQuestMark::Create(D3DXVECTOR3(m_pInfo->m_vPos.x, 2.5f, m_pInfo->m_vPos.z));

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

	cbDesc.ByteWidth = sizeof(D3DXMATRIX);
	FAILED_CHECK(m_pGrapicDevice->m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pCascadedShadowGenVertexCB));

	return S_OK;
}

int CNpc::Update(void)

{

	D3DXVec3TransformNormal(&m_pInfo->m_vDir, &g_vLook, &m_pInfo->m_matWorld);

	NpcChat();
	SetCurrling();

	m_pTerrainCol->CollisionTerrain(&m_pInfo->m_vPos, m_pVerTex);


	m_pQuestMark->Update();
	QuestMark();

	CObj::Update();

	if (!m_bDeath)
		return 0;

	else
		return 100;




}

void CNpc::Render(void)
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

		dynamic_cast<CDynamicMesh*>(m_pBuffer)->PlayAnimation(0,20.f);
		m_pQuestMark->Render(m_bQuestAccept);
	}
}

void  CNpc::ShadowmapRender(void)
{
	if (m_bCurred == false)
	{
		D3D11_MAPPED_SUBRESOURCE MappedResource;
		FAILED_CHECK_RETURN(m_pGrapicDevice->m_pDeviceContext->Map(m_pCascadedShadowGenVertexCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource), );
		D3DXMATRIX* pVSPerObject = (D3DXMATRIX*)MappedResource.pData;
		D3DXMatrixTranspose(pVSPerObject, &m_pInfo->m_matWorld);
		m_pGrapicDevice->m_pDeviceContext->Unmap(m_pCascadedShadowGenVertexCB, 0);
		m_pGrapicDevice->m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pCascadedShadowGenVertexCB);

		m_pGrapicDevice->m_pDeviceContext->IASetInputLayout(m_pShadowAniVertexShader->m_pVertexLayout);
		m_pGrapicDevice->m_pDeviceContext->VSSetShader(m_pShadowAniVertexShader->m_pVertexShader, NULL, 0);

		dynamic_cast<CDynamicMesh*>(m_pBuffer)->PlayAnimation(0,20.f);

		m_pGrapicDevice->m_pDeviceContext->IASetInputLayout(m_pShadowNonAniVertexShader->m_pVertexLayout);
		m_pGrapicDevice->m_pDeviceContext->VSSetShader(m_pShadowNonAniVertexShader->m_pVertexShader, NULL, 0);
		m_pQuestMark->Render(m_bQuestAccept);
	}
}

CNpc * CNpc::Create(wstring wstMeshKey, wstring wstrTextureKey, D3DXVECTOR3 vPos, QUESTNPCTYPE eNpcType)
{
	CNpc* pObj = new CNpc;
	if (FAILED(pObj->Initialize(wstMeshKey, wstrTextureKey, vPos, eNpcType)))
		::Safe_Delete(pObj);

	return pObj;
}


HRESULT CNpc::AddComponent(wstring wstMeshKey, wstring wstrTextureKey)
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

	pComponent = m_pShadowAniVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"PointShandowGenVS_ANI");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"VS_Shader_ShadowAni", pComponent));
	pComponent = m_pShadowNonAniVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"PointShadowGenVS");
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"VS_Shader_Shadow", pComponent));

	return S_OK;
}

void CNpc::SetCurrling(void)
{
	auto player = CObjMgr::GetInstance()->Get_ObjList(L"Player")->begin();

	D3DXVECTOR3 vPlayerPos = (*player)->GetInfo()->m_vPos;

	if (abs(m_pInfo->m_vPos.x - vPlayerPos.x) > 30.f && abs(m_pInfo->m_vPos.z - vPlayerPos.z) > 30.f)
		m_bCurred = true;
	else
		m_bCurred = false;
}

void CNpc::NpcChat(void)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(*(CObjMgr::GetInstance()->Get_ObjList(L"Player")->begin()));

	D3DXVECTOR3 vPlayerPos = pPlayer->GetInfo()->m_vPos;

	CChatUI* pChatUI = dynamic_cast<CChatUI*>(*(CObjMgr::GetInstance()->Get_ObjList(L"ChatUI")->begin()));

	




	if ((abs(m_pInfo->m_vPos.x - vPlayerPos.x) < 2.f && abs(m_pInfo->m_vPos.z - vPlayerPos.z) < 2.f) )
	{
		if (pPlayer->m_eQuestState == QUEST_NOT && m_bPlayerIn == false && m_eNpcType == NPC_SLIME)
		{
			m_bPlayerIn = true;

			CFont* pFont = CFont::Create(L"Font_Clear");

			pFont->m_eType = FONT_TYPE_OUTLINE;
			pFont->m_fSize = 20.f;
			pFont->m_nColor = 0xFFFFFFFF;
			pFont->m_nFlag = FW1_LEFT | FW1_VCENTER | FW1_RESTORESTATE;
			pFont->m_vPos = D3DXVECTOR2(15.f, 620.f);
			pFont->m_fOutlineSize = 1.f;
			pFont->m_nOutlineColor = 0xFFFFFFFF /*0xFFFFFFFF*/;

			pFont->m_wstrText = L"주민: 슬라임이 갑자기 난폭해졌어요!";

			for (auto ChatList : pChatUI->m_ChatLogList)
				ChatList->m_vPos.y -= 20.f;

			if (pChatUI->m_ChatLogList.size() == 7)
			{
				pChatUI->m_ChatLogList.pop_front(); 
			}

			pChatUI->m_ChatLogList.push_back(pFont);

			pFont = CFont::Create(L"Font_Clear");

			pFont->m_eType = FONT_TYPE_OUTLINE;
			pFont->m_fSize = 20.f;
			pFont->m_nColor = 0xFFFFFFFF;
			pFont->m_nFlag = FW1_LEFT | FW1_VCENTER | FW1_RESTORESTATE;
			pFont->m_vPos = D3DXVECTOR2(15.f, 620.f);
			pFont->m_fOutlineSize = 1.f;
			pFont->m_nOutlineColor = 0xFFFFFFFF /*0xFFFFFFFF*/;

			pFont->m_wstrText = L"용사님깨서 도와주시지 않겠습니까?(수락: T키)";

			for (auto ChatList : pChatUI->m_ChatLogList)
				ChatList->m_vPos.y -= 20.f;

			if (pChatUI->m_ChatLogList.size() == 7)
			{
				pChatUI->m_ChatLogList.pop_front(); 
			}



			pChatUI->m_ChatLogList.push_back(pFont);
		}
		else if (pPlayer->m_eQuestState == QUEST_SLIME && m_bPlayerIn == false && m_eNpcType == NPC_GOBLIN && pPlayer->m_iQuestStateMount >= MAX_AI_SLIME)
		{
			m_bPlayerIn = true;

			CFont* pFont = CFont::Create(L"Font_Clear");

			pFont->m_eType = FONT_TYPE_OUTLINE;
			pFont->m_fSize = 20.f;
			pFont->m_nColor = 0xFFFFFFFF;
			pFont->m_nFlag = FW1_LEFT | FW1_VCENTER | FW1_RESTORESTATE;
			pFont->m_vPos = D3DXVECTOR2(15.f, 620.f);
			pFont->m_fOutlineSize = 1.f;
			pFont->m_nOutlineColor = 0xFFFFFFFF /*0xFFFFFFFF*/;

			pFont->m_wstrText = L"주민: 마을에 고블린이....용사님 도와주십시요!(수락: T키)";

			for (auto ChatList : pChatUI->m_ChatLogList)
				ChatList->m_vPos.y -= 20.f;

			if (pChatUI->m_ChatLogList.size() == 7)
			{
				pChatUI->m_ChatLogList.pop_front(); // 앞에 폰트객체지우는걸 해야하는대 일단 귀찮으니깐 팝만하자. 나중에 처리해야지
			}

			pChatUI->m_ChatLogList.push_back(pFont);
		}
		else if (pPlayer->m_eQuestState == QUEST_GOBLIN && m_bPlayerIn == false && m_eNpcType == NPC_BOSS && pPlayer->m_iQuestStateMount >= MAX_AI_GOBLIN)
		{
			m_bPlayerIn = true;

			CFont* pFont = CFont::Create(L"Font_Clear");

			pFont->m_eType = FONT_TYPE_OUTLINE;
			pFont->m_fSize = 20.f;
			pFont->m_nColor = 0xFFFFFFFF;
			pFont->m_nFlag = FW1_LEFT | FW1_VCENTER | FW1_RESTORESTATE;
			pFont->m_vPos = D3DXVECTOR2(15.f, 620.f);
			pFont->m_fOutlineSize = 1.f;
			pFont->m_nOutlineColor = 0xFFFFFFFF /*0xFFFFFFFF*/;

			pFont->m_wstrText = L"주민: 흉포의 원인 골렘이 근처에 있습니다.";



			for (auto ChatList : pChatUI->m_ChatLogList)
				ChatList->m_vPos.y -= 20.f;

			if (pChatUI->m_ChatLogList.size() == 7)
			{
				pChatUI->m_ChatLogList.pop_front(); // 앞에 폰트객체지우는걸 해야하는대 일단 귀찮으니깐 팝만하자. 나중에 처리해야지
			}

			pChatUI->m_ChatLogList.push_back(pFont);

			pFont = CFont::Create(L"Font_Clear");

			pFont->m_eType = FONT_TYPE_OUTLINE;
			pFont->m_fSize = 20.f;
			pFont->m_nColor = 0xFFFFFFFF;
			pFont->m_nFlag = FW1_LEFT | FW1_VCENTER | FW1_RESTORESTATE;
			pFont->m_vPos = D3DXVECTOR2(15.f, 620.f);
			pFont->m_fOutlineSize = 1.f;
			pFont->m_nOutlineColor = 0xFFFFFFFF /*0xFFFFFFFF*/;

			pFont->m_wstrText = L"다른 용사님들과 같이 퇴치를 부탁드려요!(수락: T키)";

			for (auto ChatList : pChatUI->m_ChatLogList)
				ChatList->m_vPos.y -= 20.f;

			if (pChatUI->m_ChatLogList.size() == 7)
			{
				pChatUI->m_ChatLogList.pop_front(); // 앞에 폰트객체지우는걸 해야하는대 일단 귀찮으니깐 팝만하자. 나중에 처리해야지
			}

			pChatUI->m_ChatLogList.push_back(pFont);
		}
	}

	else
	{
		if (pPlayer->m_eQuestState == QUEST_NOT && m_bPlayerIn == true && m_eNpcType == NPC_SLIME)
		{
			m_bPlayerIn = false;

			CFont* pFont = CFont::Create(L"Font_Clear");

			pFont->m_eType = FONT_TYPE_OUTLINE;
			pFont->m_fSize = 20.f;
			pFont->m_nColor = 0xFFFFFFFF;
			pFont->m_nFlag = FW1_LEFT | FW1_VCENTER | FW1_RESTORESTATE;
			pFont->m_vPos = D3DXVECTOR2(15.f, 620.f);
			pFont->m_fOutlineSize = 1.f;
			pFont->m_nOutlineColor = 0xFFFFFFFF /*0xFFFFFFFF*/;

			pFont->m_wstrText = L"주민: 저런!... 그럼 저흰 어떻게 되는겁니까?...";

			for (auto ChatList : pChatUI->m_ChatLogList)
				ChatList->m_vPos.y -= 20.f;

			if (pChatUI->m_ChatLogList.size() == 7)
			{
				pChatUI->m_ChatLogList.pop_front(); // 앞에 폰트객체지우는걸 해야하는대 일단 귀찮으니깐 팝만하자. 나중에 처리해야지
			}

			pChatUI->m_ChatLogList.push_back(pFont);
		}
		else if (pPlayer->m_eQuestState == QUEST_SLIME && m_bPlayerIn == true && m_eNpcType == NPC_GOBLIN)
		{
			m_bPlayerIn = false;

			CFont* pFont = CFont::Create(L"Font_Clear");

			pFont->m_eType = FONT_TYPE_OUTLINE;
			pFont->m_fSize = 20.f;
			pFont->m_nColor = 0xFFFFFFFF;
			pFont->m_nFlag = FW1_LEFT | FW1_VCENTER | FW1_RESTORESTATE;
			pFont->m_vPos = D3DXVECTOR2(15.f, 620.f);
			pFont->m_fOutlineSize = 1.f;
			pFont->m_nOutlineColor = 0xFFFFFFFF /*0xFFFFFFFF*/;

			pFont->m_wstrText = L"주민: 살려주십시요 용사님!...";

			for (auto ChatList : pChatUI->m_ChatLogList)
				ChatList->m_vPos.y -= 20.f;

			if (pChatUI->m_ChatLogList.size() == 7)
			{
				pChatUI->m_ChatLogList.pop_front(); // 앞에 폰트객체지우는걸 해야하는대 일단 귀찮으니깐 팝만하자. 나중에 처리해야지
			}

			pChatUI->m_ChatLogList.push_back(pFont);
		}
		else if (pPlayer->m_eQuestState == QUEST_GOBLIN && m_bPlayerIn == true && m_eNpcType == NPC_BOSS)
		{
			m_bPlayerIn = false;

			CFont* pFont = CFont::Create(L"Font_Clear");

			pFont->m_eType = FONT_TYPE_OUTLINE;
			pFont->m_fSize = 20.f;
			pFont->m_nColor = 0xFFFFFFFF;
			pFont->m_nFlag = FW1_LEFT | FW1_VCENTER | FW1_RESTORESTATE;
			pFont->m_vPos = D3DXVECTOR2(15.f, 620.f);
			pFont->m_fOutlineSize = 1.f;
			pFont->m_nOutlineColor = 0xFFFFFFFF /*0xFFFFFFFF*/;

			pFont->m_wstrText = L"주민: 아!... 이 마을은 누가 구원할 것인지...";

			for (auto ChatList : pChatUI->m_ChatLogList)
				ChatList->m_vPos.y -= 20.f;

			if (pChatUI->m_ChatLogList.size() == 7)
			{
				pChatUI->m_ChatLogList.pop_front(); // 앞에 폰트객체지우는걸 해야하는대 일단 귀찮으니깐 팝만하자. 나중에 처리해야지
			}

			pChatUI->m_ChatLogList.push_back(pFont);
		}
	}
}

void CNpc::QuestMark(void)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(*(CObjMgr::GetInstance()->Get_ObjList(L"Player")->begin()));

	if (pPlayer->m_eQuestState == QUEST_SLIME && m_eNpcType == NPC_SLIME)
	{
		m_bQuestAccept = true;
	}
	else if (pPlayer->m_eQuestState == QUEST_GOBLIN && m_eNpcType == NPC_GOBLIN)
	{
		m_bQuestAccept = true;
	}
	else if (pPlayer->m_eQuestState == QUEST_BOSS && m_eNpcType == NPC_BOSS)
	{
		m_bQuestAccept = true;
	}
}

