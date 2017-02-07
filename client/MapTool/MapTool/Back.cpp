#include "stdafx.h"
#include "Back.h"
#include "SceneMgr.h"
#include "RenderMgr.h"
#include "TimeMgr.h"
#include "ObjMgr.h"
#include "MapToolView.h"
#include "MainFrm.h"
#include "StaticObject.h"
#include "ResourcesMgr.h"

CBack::CBack()
{
}


CBack::~CBack()
{
}

HRESULT CBack::Initialize(void)
{
	

	HRESULT hr = NULL;

	//lstrcpy(m_szLoadMessage, L"텍스쳐 로딩중...");
	cout << "TextureLoading" << endl;
	//Texture
	hr = CResourcesMgr::GetInstance()->AddTexture(
		RESOURCE_STAGE
		, L"Texture_Town"
		, L"../Resource/MeshImage/Town.png");
	FAILED_CHECK_RETURN(hr, );


	//lstrcpy(m_szLoadMessage, L"버퍼 로딩중...");
	cout << "StaticBufferLoading" << endl;
	//Buffer
	hr = CResourcesMgr::GetInstance()->AddMesh(
		RESOURCE_STAGE,
		MESH_STATIC
		, L"Mesh_Town"
		, "../Resource/Mesh/"
		, "Town.FBX");
	FAILED_CHECK_RETURN(hr, );
	
	if (FAILED(CreateObj()))
		return E_FAIL;



	return S_OK;
}

int CBack::Update(void)
{
	CObjMgr::GetInstance()->Update();


	return 0;
}

void CBack::Render(void)
{
	float fTime = CTimeMgr::GetInstance()->GetTime();
	CRenderMgr::GetInstance()->Render(fTime);
}

void CBack::Release(void)
{
}

CBack * CBack::Create(void)
{

	CBack* pLogo = new CBack;
	if (FAILED(pLogo->Initialize()))
	{
		::Safe_Delete(pLogo);
	}
	return pLogo;

}

HRESULT CBack::CreateObj(void)
{
	CRenderMgr* pRenderer = CRenderMgr::GetInstance();
	//터레인
	CObj* pObj = NULL;
	pObj = CStaticObject::Create();
	if (pObj == NULL)
		return E_FAIL;

	CObjMgr::GetInstance()->AddObject(L"StaticObject", pObj);
	pRenderer->AddRenderGroup(TYPE_NONEALPHA, pObj);



	return S_OK;
}


void CBack::SetMainView(CMapToolView* pMainView)
{
	m_pMainView = pMainView;
}
