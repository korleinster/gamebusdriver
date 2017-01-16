#pragma once
#include "VIBuffer.h"
class CRcTex :
	public CVIBuffer
{
private:
	CRcTex();
	CRcTex(CRcTex* rhs);

public:
	virtual ~CRcTex();


private:
	virtual HRESULT CreateBuffer(void);
public:
	static CRcTex* Create(void);
public:
	virtual CResources* CloneResource(void);
};

