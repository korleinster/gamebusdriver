#include "stdafx.h"
#include "AnimationMgr.h"
#include "Include.h"

IMPLEMENT_SINGLETON(CAnimationMgr)

CAnimationMgr::CAnimationMgr()
{

}

CAnimationMgr::~CAnimationMgr()
{

}

HRESULT CAnimationMgr::AddAnimation(wstring wstrKey, vector<string>* vecAni)
{
	if (wstrKey == L"")
	{
		MessageBox(NULL, L"System Message", L"Can't Read Animation Key", MB_OK);
		return E_FAIL;
	}
	map<wstring, vector<string>>::iterator iter = m_mapAni.find(wstrKey);

	if (iter == m_mapAni.end())
	{
		m_mapAni.insert(pair<wstring, vector<string>>(wstrKey, *vecAni));
	}
	else
	{
		MessageBox(NULL, L"System Message", L"Already Exist!",MB_OK);
	}
	

	return S_OK;
}

HRESULT CAnimationMgr::DelAnimation(wstring wstrKey)
{
	map<wstring, vector<string>>::iterator iter = m_mapAni.find(wstrKey);

	if (iter == m_mapAni.end())
	{
		MessageBox(NULL, L"System Message", L"A file that does not exist or has already been deleted.",S_OK);
		return E_FAIL;
	}

	(*iter).second.clear();

	m_mapAni.erase(iter);

	return S_OK;
}

vector<string>* CAnimationMgr::GetAnimaiton(wstring wstrKey)
{
	map<wstring, vector<string>>::iterator iter = m_mapAni.find(wstrKey);

	if (iter == m_mapAni.end())
		return NULL;

	return &(*iter).second;
}
