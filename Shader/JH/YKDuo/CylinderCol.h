#pragma once
#include "VIBuffer.h"
class CCylinderCol : public CVIBuffer
{
public:
	CCylinderCol();
	virtual ~CCylinderCol();

public:
	MeshData m_meshData;
	UINT m_iCylinderIndexCnt;
	UINT m_iCylinderVertexCnt;

private:
	virtual HRESULT CreateBuffer(float fBottomRadius, float fTopRadius, float fHeight, UINT iSliceCount, UINT iStackCount);
	void BuildCylinderTopCap(float fBottomRadius, float fTopRadius, float fHeight,
		UINT iSliceCount, UINT iStackCount, MeshData& meshData);
	void BuildCylinderBottomCap(float fBottomRadius, float fTopRadius, float fHeight,
		UINT iSliceCount, UINT iStackCount, MeshData& meshData);
	void BuildCylinder(float fBottomRadius, float fTopRadius, float fHeight,
		UINT iSliceCount, UINT iStackCount, MeshData& meshData);

public:
	static CCylinderCol* Create(float fBottomRadius, float fTopRadius, float fHeight, UINT iSliceCount, UINT iStackCount);

public:
	virtual CResources* CloneResource(void);
};