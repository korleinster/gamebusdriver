#pragma once
#include "VIBuffer.h"
class CRcCol :
	public CVIBuffer
{
public:
	CRcCol();
	virtual ~CRcCol();

private:
	virtual HRESULT CreateBuffer(void);

public:
	static CRcCol* Create(void);

public:
	virtual CResources* CloneResource(void);
};

