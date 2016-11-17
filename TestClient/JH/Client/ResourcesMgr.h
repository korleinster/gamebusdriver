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
	HRESULT AddBuffer(LPDIRECT3DDEVICE9 pDevice, const WORD& wContainerIndex, BUFFERTYPE  eBufferType, const wstring wstrResourceKey, const WORD& wCountX = 0, const WORD& wCountZ = 0,	const WORD& wInterval = 1);

public:
	void ResourceReset(const WORD& wContainerIndex);

private:
	void Release(void);

};

