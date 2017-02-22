#include "stdafx.h"
#include "ObjMgr.h"
#include "Obj.h"

IMPLEMENT_SINGLETON(CObjMgr)

CObjMgr::CObjMgr()
{
}


CObjMgr::~CObjMgr()
{
}

HRESULT CObjMgr::SceneChange(void)
{
	map<wstring, list<CObj*>>::iterator iter = m_mapObj.begin();
	map<wstring, list<CObj*>>::iterator iter_end = m_mapObj.end();

	for (iter; iter != iter_end; ++iter)
	{
		list<CObj*>::iterator Listiter = iter->second.begin();
		list<CObj*>::iterator Listiter_end = iter->second.end();

		for (Listiter; Listiter != Listiter_end; )
		{
			if ((*Listiter)->GetReleaseType() == Release_Dynamic)
			{
				::Safe_Delete(*Listiter);
				iter->second.erase(Listiter);
			}
			else
				++Listiter;
		}
	}

	return S_OK;
}

HRESULT CObjMgr::AddObject(wstring wstrKey, CObj * pObj)
{
	if (pObj == NULL)
	{
		MessageBox(NULL, L"System Message", wstrKey.c_str(), MB_OK);
		return E_FAIL;
	}

	map<wstring, list<CObj*>>::iterator	iter = m_mapObj.find(wstrKey);

	if (iter == m_mapObj.end())
	{
		m_mapObj[wstrKey] = list<CObj*>();
	}

	m_mapObj[wstrKey].push_back(pObj);


	return S_OK;
}

void CObjMgr::Update(void)
{
	map<wstring, list<CObj*>>::iterator iter = m_mapObj.begin();
	map<wstring, list<CObj*>>::iterator iter_end = m_mapObj.end();

	for (iter; iter != iter_end; ++iter)
	{
		list<CObj*>::iterator Listiter = iter->second.begin();
		list<CObj*>::iterator Listiter_end = iter->second.end();

		for (Listiter; Listiter != Listiter_end; )
		{
			if ((*Listiter)->Update() == 100)
			{
				::Safe_Delete(*Listiter);
				iter->second.erase(Listiter);
			}
			else
				++Listiter;
		}
	}

}

list<CObj*>*  CObjMgr::Get_ObjListForKey(wstring wstrKey)
{
	map<wstring, list<CObj*>>::iterator iter = m_mapObj.find(wstrKey);

	if (iter == m_mapObj.end())
		return NULL;

	return &(*iter).second;
}

void CObjMgr::DeleteByCompare(const CObj* pGameObject)
{
	map<wstring, list<CObj*>>::iterator		mapiter = m_mapObj.begin();
	map<wstring, list<CObj*>>::iterator		mapiter_end = m_mapObj.end();

	for (mapiter; mapiter != mapiter_end; ++mapiter)
	{
		list<CObj*>::iterator	iterlist = mapiter->second.begin();
		list<CObj*>::iterator	iterlist_end = mapiter->second.end();

		for (iterlist; iterlist != iterlist_end; ++iterlist)
		{
			if ((*iterlist) == pGameObject)
				break;
		}
		if (iterlist != iterlist_end)
		{
			mapiter->second.erase(iterlist);
			break;
		}
	}
	if (mapiter == mapiter_end)
		return;

	if (mapiter->second.empty())
		m_mapObj.erase(mapiter);
}
