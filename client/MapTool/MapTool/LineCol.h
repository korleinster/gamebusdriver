#pragma  once

#include "VIBuffer.h"
#include "NaviMgr.h"

class CNaviCell;
class CLineCol
	: public CVIBuffer
{
public:
	static CLineCol* Create(CNaviCell* pNaviCell);
	virtual HRESULT CreateBuffer(CNaviCell* pNeviCell);

	
private:
	VTXCOL* m_vCol;
	INDEX16* m_idx;

public:
	virtual CResources* CloneResource(void);
	virtual DWORD Release(void);

public:
	CLineCol(CNaviCell* pNaviCell);
	~CLineCol();


	 

};