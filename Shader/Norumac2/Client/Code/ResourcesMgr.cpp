#include "stdafx.h"
#include "ResourcesMgr.h"
#include "VIBuffer.h"
#include "RcCol.h"



IMPLEMENT_SINGLETON(CResourcesMgr)

CResourcesMgr::CResourcesMgr()
	: m_pmapResource(NULL)
	, m_wReservedSize(0)
{
}


CResourcesMgr::~CResourcesMgr()
{
}

CResources * CResourcesMgr::FindResources(const WORD & wContainerIndex, const wstring strResourceKey)
{
	map<wstring, CResources*>::iterator iter = m_pmapResource[wContainerIndex].find(strResourceKey);

	if (iter == m_pmapResource[wContainerIndex].end())
	{
		MessageBox(NULL, strResourceKey.c_str(), L"중복된 리소스", MB_OK);
		return NULL;
	}

	return iter->second;
}

CResources * CResourcesMgr::CloneResource(const WORD & wContainerIndex, const wstring strResourceKey)
{
	CResources* pResources = FindResources(wContainerIndex, strResourceKey);

	if (pResources == NULL)
		return NULL;

	return pResources->CloneResource();
}

HRESULT CResourcesMgr::ReserveContainerSize(const WORD & wSize)
{
	if (m_wReservedSize != 0 || m_pmapResource != NULL)
		return E_FAIL;

	m_pmapResource = new map<wstring, CResources*>[wSize];

	m_wReservedSize = wSize;

	return S_OK;
}

HRESULT CResourcesMgr::AddBuffer(const WORD & wContainerIndex, BUFFERTYPE eBufferType, const wstring wstrResourceKey, const WORD & wCountX, const WORD & wCountZ, const WORD & wInterval)
{
	if (m_wReservedSize == 0)
	{
		MessageBox(NULL, L"저장공간이 할당되지 않았습니다.", L"정신차려 이친구야", MB_OK);
		return E_FAIL;
	}
	if (m_pmapResource == NULL)
	{
		MessageBox(NULL, L"저장공간이 할당되지 않았습니다.", L"정신차려 이친구야", MB_OK);
		return E_FAIL;
	}

	CResources* pResources = FindResources(wContainerIndex, wstrResourceKey);
	if (pResources != NULL)
	{
		MessageBox(NULL, L"이미 저장되어 있는 버퍼입니다.", L"정신차려 이친구야", MB_OK);
		return E_FAIL;
	}

	CVIBuffer* pBuffer = NULL;

	switch (eBufferType)
	{
	case BUFFER_RCCOL:
		pBuffer = CRcCol::Create();
		break;

	/*case BUFFER_TERRAIN:
		pBuffer = CTerrainTex::Create(pDevice, wCountX, wCountZ, wInterval);
		break;

	case BUFFER_CUBETEX:
		pBuffer = CCubeTex::Create(pDevice);
		break;*/
	}

	if (pBuffer == NULL)
		return E_FAIL;

	m_pmapResource[wContainerIndex].insert(map<wstring, CResources*>::value_type(wstrResourceKey, pBuffer));

	return S_OK;
}

HRESULT CResourcesMgr::AddTexture(const WORD & wContainerIndex, TEXTURETYPE eTextureType, const wstring wstrResourceKey, const wstring wstrFilePath, const WORD & wConut)
{
	return E_NOTIMPL;
}

void CResourcesMgr::ResourceReset(const WORD & wContainerIndex)
{
	map<wstring, CResources*>::iterator iter = m_pmapResource[wContainerIndex].begin();
	map<wstring, CResources*>::iterator iter_end = m_pmapResource[wContainerIndex].end();

	for (iter; iter != iter_end; ++iter)
	{
		delete iter->second;
	}

	m_pmapResource[wContainerIndex].clear();
}

void CResourcesMgr::Release(void)
{
	if (0 == m_wReservedSize)
		return;

	for (size_t i = 0; i < m_wReservedSize; ++i)
	{
		map<wstring, CResources*>::iterator	iter = m_pmapResource[i].begin();
		map<wstring, CResources*>::iterator	iter_end = m_pmapResource[i].end();

		DWORD		dwRefCnt = 0;
		for (; iter != iter_end; ++iter)
		{
			//dwRefCnt = iter->second->Release();
			if (dwRefCnt == 0)
				Safe_Delete(iter->second);
		}
		m_pmapResource[i].clear();
	}
	::Safe_Delete_Array(m_pmapResource);
}
