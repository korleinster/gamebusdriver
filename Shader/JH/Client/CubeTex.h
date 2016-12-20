#pragma once
#include "VIBuffer.h"
class CCubeTex : public CVIBuffer
{
public:
	CCubeTex();
	virtual ~CCubeTex();
private:
	virtual HRESULT CreateBuffer(void);

public:
	static CCubeTex* Create(void);

public:
	virtual CResources* CloneResource(void);
};

