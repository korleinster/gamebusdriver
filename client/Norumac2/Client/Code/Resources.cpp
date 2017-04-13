#include "stdafx.h"
#include "Resources.h"


CResources::CResources()
{
	m_dwRefCount = 0;
}

CResources::CResources(CResources& rhs)
{
	m_dwRefCount = rhs.m_dwRefCount;

}


CResources::~CResources()
{
}

DWORD CResources::Release(void)
{
	return 0;
}

void CResources::AddRef(void)
{
	++m_dwRefCount;
}
