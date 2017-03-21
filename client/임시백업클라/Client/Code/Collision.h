#pragma once

#include "Component.h"

class CCollision : public CComponent
{
public:
	CCollision();
	virtual ~CCollision();

	// CComponent을(를) 통해 상속됨
	virtual DWORD Release(void);
};

