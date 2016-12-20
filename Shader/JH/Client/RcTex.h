#pragma once

#include "VIBuffer.h"

class CRcTex :
	public CVIBuffer
{
public:
	CRcTex();
	virtual ~CRcTex();
private:
	virtual HRESULT CreateBuffer(void);

public:
	static CRcTex* Create(void);

public:
	virtual CResources* CloneResource(void);
};