#include "stdafx.h"
#include "LightMgr.h"
#include "Light.h"

IMPLEMENT_SINGLETON(CLightMgr)
CLightMgr::CLightMgr(void)
{

}

CLightMgr::~CLightMgr(void)
{

}

const LIGHTINFO* CLightMgr::Get_LightInfo(const DWORD& dwIndex)
{
	LIGHTLIST::iterator	iter = m_LightList.begin();

	for (DWORD i = 0; i < dwIndex; ++i)
		++iter;

	if (iter == m_LightList.end())
		return NULL;

	return (*iter)->Get_LightInfo();
}

HRESULT CLightMgr::AddLight(const LIGHTINFO* pLightInfo, const DWORD& dwLightIdx)
{
	if (pLightInfo == NULL)
		return E_FAIL;

	CLight*		pLight = CLight::Create(pLightInfo, dwLightIdx);

	if (pLight == NULL)
		return E_FAIL;

	m_LightList.push_back(pLight);
	return S_OK;
}

void CLightMgr::Render_Light()
{
	LIGHTLIST::iterator	iter = m_LightList.begin();
	LIGHTLIST::iterator	iter_end = m_LightList.end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Render_Light();
	}
}

void CLightMgr::Release(void)
{
	LIGHTLIST::iterator	iter = m_LightList.begin();
	LIGHTLIST::iterator	iter_end = m_LightList.end();

	for (; iter != iter_end; ++iter)
	{
		delete (*iter);
		*iter = nullptr;
	}
	m_LightList.clear();
}

