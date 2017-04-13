#include "stdafx.h"
#include "ObjMgr.h"
#include "Obj.h"
#include "OtherPlayer.h"

IMPLEMENT_SINGLETON(CObjMgr)

CObjMgr::CObjMgr()
{
}


CObjMgr::~CObjMgr()
{
	Release();
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
				Listiter = iter->second.erase(Listiter);
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
				Listiter = iter->second.erase(Listiter);
			}
			else
				++Listiter;
		}
	}

}

player_data*  CObjMgr::Get_PlayerServerData(UINT uID)
{
	map<wstring, list<CObj*>>::iterator iter = m_mapObj.find(L"OtherPlayer");

	if (iter == m_mapObj.end())
		return NULL;

	for (auto ptr : iter->second)
	{
		UINT uCompareId = ((COtherPlayer*)ptr)->GetPacketData()->id;

		if (uCompareId == uID)
			return ((COtherPlayer*)ptr)->GetPacketData();
	}

	return NULL;
}

list<CObj*>*  CObjMgr::Get_ObjList(wstring wstrKey)
{
	map<wstring, list<CObj*>>::iterator iter = m_mapObj.find(wstrKey);

	if (iter == m_mapObj.end())
		return NULL;

	return &(*iter).second;
}

void CObjMgr::Release(void)
{
	map<wstring, list<CObj*>>::iterator iter = m_mapObj.begin();
	map<wstring, list<CObj*>>::iterator iter_end = m_mapObj.end();

	for (iter; iter != iter_end; ++iter)
	{
		list<CObj*>::iterator Listiter = iter->second.begin();
		list<CObj*>::iterator Listiter_end = iter->second.end();

		for (Listiter; Listiter != Listiter_end; ++Listiter)
		{
			::Safe_Delete(*Listiter);
		}
		iter->second.clear();
	}
	m_mapObj.clear();
}

