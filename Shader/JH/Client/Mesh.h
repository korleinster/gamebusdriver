#pragma once

#ifndef Mesh_h__
#define Mesh_h__

#include "VIBuffer.h"
#include "Shader.h"

class CMesh : public CVIBuffer
{
public:
	CShader* m_pShader;

public:
	virtual void Init_Shader() PURE;
	virtual CResources* CloneResource() PURE;
	virtual void Render();

	void MakeVertexNormal(BYTE* _pVertices, WORD* _pIndices);
	void SetNormalVectorByBasic(BYTE* _pVertices);
	void SetNormalVectorByAverage(BYTE* _pVertices, WORD* _pIndices, int _iPrimitives, int _iOffset, bool _bStrip);
	D3DXVECTOR3 GetTriAngleNormal(BYTE* _pVertices, USHORT _nIndex_0, USHORT _nIndex_1, USHORT _nIndex_2);

public:
	CMesh();
	virtual ~CMesh();


};

#endif // Mesh_h__