#include "stdafx.h"
#include "ResourcesMgr.h"
#include "VIBuffer.h"
#include "RcCol.h"
#include "Texture.h"
#include "StaticMesh.h"
#include "DynamicMesh.h"
#include "RcTex.h"
#include "RcTerrain.h"



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
		MessageBox(NULL, L"저장공간이 할당되지 않았습니다.", L"Error", MB_OK);
		return E_FAIL;
	}
	if (m_pmapResource == NULL)
	{
		MessageBox(NULL, L"저장공간이 할당되지 않았습니다.", L"Error", MB_OK);
		return E_FAIL;
	}

	CResources* pResources = FindResources(wContainerIndex, wstrResourceKey);
	if (pResources != NULL)
	{
		MessageBox(NULL, L"이미 저장되어 있는 버퍼입니다.", L"Error", MB_OK);
		return E_FAIL;
	}

	CVIBuffer* pBuffer = NULL;

	switch (eBufferType)
	{
	case BUFFER_RCCOL:
		pBuffer = CRcCol::Create();
		break;

	case BUFFER_RCTEX:
		pBuffer = CRcTex::Create();
		break;

	case BUFFER_TERRAIN:
		pBuffer = CRcTerrain::Create(wCountX, wCountZ, wInterval);
		break;

		/*case BUFFER_CUBETEX:
		pBuffer = CCubeTex::Create(pDevice);
		break;*/
	}

	if (pBuffer == NULL)
		return E_FAIL;

	m_pmapResource[wContainerIndex].insert(map<wstring, CResources*>::value_type(wstrResourceKey, pBuffer));

	return S_OK;
}

HRESULT CResourcesMgr::AddTexture(const WORD & wContainerIndex, LPCWSTR wstrResourceKey, LPCWSTR wstrFilePath, const WORD & wConut)
{

	NULL_CHECK_RETURN_MSG(m_wReservedSize, E_FAIL, L"ReservedSize Not Failed");
	NULL_CHECK_RETURN_MSG(m_pmapResource, E_FAIL, L"pmapResource Not Failed");

	CResources*		pResource = FindResources(wContainerIndex, wstrResourceKey);
	if (pResource != NULL)
	{
		TAGMSG_BOX(wstrResourceKey, L"텍스쳐가 이미 추가 되어 있음");
		return E_FAIL;
	}
	//pDevice->AddRef();

	pResource = CTexture::Create(wstrFilePath);
	NULL_CHECK_RETURN_MSG(pResource, E_FAIL, L"리소스 할당 실패");

	m_pmapResource[wContainerIndex].insert(map<wstring, CResources*>::value_type(wstrResourceKey, pResource));
	return S_OK;
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

HRESULT CResourcesMgr::AddMesh(const WORD& wContainerIdx
	, MESHTYPE eMeshType
	, const TCHAR* pMeshKey
	, const char* pFilePath
	, const char* pFileName
	, vector<string> _vecAniName /*= vector<string>()*/)//Static은 Vector인자를 NULL로, Dynamic은 pFilename을 NULL로 받을것
{
	//Static은 Vector인자를 NULL로, Dynamic은 pFilename을 NULL로 받을것
	if (m_wReservedSize == NULL)
		return E_FAIL;

	CResources*	pResource = FindResources(wContainerIdx, pMeshKey);
	if (pResource != NULL)
		return E_FAIL;

	//pDevice->AddRef();

	switch (eMeshType)
	{
	case MESH_STATIC:
		pResource = CStaticMesh::Create(pFilePath, pFileName);
		break;

	case MESH_DYNAMIC:
		pResource = CDynamicMesh::Create(pFilePath, _vecAniName);
		break;
	}
	NULL_CHECK_RETURN(pResource, E_FAIL);

	m_pmapResource[wContainerIdx].insert(map<wstring, CResources*>::value_type(pMeshKey, pResource));
	return S_OK;
}