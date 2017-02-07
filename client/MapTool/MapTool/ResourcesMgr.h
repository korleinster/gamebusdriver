#pragma once


class CVIBuffer;
class CResources;
class CResourcesMgr
{
private:
	CResourcesMgr();
	virtual ~CResourcesMgr();

public:
	DECLARE_SINGLETON(CResourcesMgr)

private:
	map<wstring, CResources*>*		m_pmapResource;
	WORD							m_wReservedSize;

public:
	CResources* FindResources(const WORD& wContainerIndex, const wstring strResourceKey);
	CResources* CloneResource(const WORD& wContainerIndex, const wstring strResourceKey);

public:
	HRESULT ReserveContainerSize(const WORD& wSize);


public:
	HRESULT AddBuffer(const WORD& wContainerIndex, BUFFERTYPE  eBufferType, const wstring wstrResourceKey, const WORD& wCountX = 0, const WORD& wCountZ = 0, const WORD& wInterval = 1);
	HRESULT AddTexture(const WORD& wContainerIndex,
		LPCWSTR wstrResourceKey,
		LPCWSTR wstrFilePath,
		const WORD& wConut = 1);
	HRESULT AddMesh(const WORD& wContainerIdx
		, MESHTYPE eMeshType
		, const TCHAR* pMeshKey
		, const char* pFilePath
		, const char* pFileName
		, vector<string> _vecAniName = vector<string>());//Static은 Vector인자를 NULL로, Dynamic은 pFilename을 NULL로 받을것

public:
	void ResourceReset(const WORD& wContainerIndex);
	void Release(void);



};

