#pragma once
#include "Include.h"
class CComponent
{
public:
	CComponent();
	virtual ~CComponent();

public:
	virtual	int  Update(void);
	virtual	DWORD Release(void) PURE;
};

