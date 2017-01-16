#include "stdafx.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "Back.h"

IMPLEMENT_SINGLETON(CSceneMgr)

CSceneMgr::CSceneMgr()
	:m_pScene(NULL)
{
}


CSceneMgr::~CSceneMgr()
{
}

HRESULT CSceneMgr::AddScene(Scene_Tyep _eType, CScene * _pScene)
{
	if (_pScene == NULL)
		return E_FAIL;

	map<Scene_Tyep, CScene*>::iterator iter = m_mapScene.find(_eType);

	if (iter == m_mapScene.end())
	{
		m_mapScene.insert(map<Scene_Tyep, CScene*>::value_type(_eType, _pScene));
	}

	else
	{
		MessageBox(NULL, L"System Message", L"중복씬", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSceneMgr::ChangeScene(Scene_Tyep _eType)
{
	map<Scene_Tyep, CScene*>::iterator iter = m_mapScene.find(_eType);

	if (iter == m_mapScene.end())
	{
		MessageBox(NULL, L"System Message", L"찾는 씬이 없습니다.", MB_OK);
		return E_FAIL;
	}

	m_eType = _eType;
	m_pScene = iter->second;

	return S_OK;
}

int CSceneMgr::Update(void)
{

	if (m_pScene == NULL)
		return 0;

	return m_pScene->Update();
}

void CSceneMgr::Render(void)
{
	if (m_pScene == NULL)
		return;

	m_pScene->Render();
}

Scene_Tyep CSceneMgr::GetScene(void)
{
	return m_eType;
}

void CSceneMgr::Release(void)
{
	map<Scene_Tyep, CScene*>::iterator iter = m_mapScene.begin();
	map<Scene_Tyep, CScene*>::iterator iter_end = m_mapScene.end();

	for (iter; iter != iter_end; ++iter)
	{
		::Safe_Delete(iter->second);
	}
	m_mapScene.clear();
}

