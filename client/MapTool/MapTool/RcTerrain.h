#pragma once
#include "VIBuffer.h"
class CRcTerrain :
	public CVIBuffer
{
private:
	CRcTerrain();
public:
	virtual ~CRcTerrain();

public:
	virtual HRESULT CreateBuffer(UINT iCountX, UINT iCountZ, UINT iInterval);
	static CRcTerrain* Create(UINT iCountX, UINT iCountZ, UINT iInterval);
	virtual CResources * CloneResource(void) override;
	DWORD*	LoadImage(void);

private:
	DWORD Release(void);

};

