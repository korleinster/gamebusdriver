#pragma once
#include "VIBuffer.h"


class CShader;
class CMesh :
	public CVIBuffer
{
public:
	CMesh();
	virtual ~CMesh();

public:
	CShader* m_pShader;

public:
	virtual void Render();

	void MakeVertexNormal(BYTE* _pVertices, WORD* _pIndices);
	void SetNormalVectorByBasic(BYTE* _pVertices);
	void SetNormalVectorByAverage(BYTE* _pVertices, WORD* _pIndices, int _iPrimitives, int _iOffset, bool _bStrip);
	D3DXVECTOR3 GetTriAngleNormal(BYTE* _pVertices, USHORT _nIndex_0, USHORT _nIndex_1, USHORT _nIndex_2);
};

