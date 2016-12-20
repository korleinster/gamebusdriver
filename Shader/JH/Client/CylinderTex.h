#pragma once
#include "VIBuffer.h"
class CCylinderTex : public CVIBuffer
{
public:
	CCylinderTex();
	virtual ~CCylinderTex();

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
	static CCylinderTex* Create(float fBottomRadius, float fTopRadius, float fHeight, UINT iSliceCount, UINT iStackCount);

public:
	virtual CResources* CloneResource(void);
};