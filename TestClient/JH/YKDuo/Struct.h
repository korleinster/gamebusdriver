#pragma once
#include "stdafx.h"














////콘스턴트 버퍼
struct ConstantBuffer
{
	D3DXMATRIX matWorld;
	D3DXMATRIX matView;
	D3DXMATRIX matProjection;
};





///////////////////////////////////////////////////////////////////정점 구조체
typedef struct tagVertexTexture
{
	D3DXVECTOR3				vPos;
	D3DXVECTOR3				vNormal;
	D3DXVECTOR2				vTexUV;
}VTXTEX;
//const DWORD VTXFVF_TEX = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEX2;

typedef struct tagVertexCubeColor
{
	D3DXVECTOR3				vPos;
	D3DXCOLOR				dwColor;
}VTXCOL;
//const DWORD VTXFVF_COL = D3DFVF_XYZ | D3DFVF_DIFFUSE;

typedef struct tagVertexCubeTex
{
	D3DXVECTOR3				vPos;
	D3DXVECTOR3				vTex;
}VTXCUBE;

typedef struct tagVertexCylinderColor
{
	D3DXVECTOR3				vPos;
	D3DXCOLOR				dwColor;
}VTXCYL;

//const DWORD VTXFVF_CUBE = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);

typedef struct tagIndex16
{
	WORD			_1, _2, _3;
}INDEX16;

typedef struct tagIndex32
{
	DWORD			_1, _2, _3;
}INDEX32;






struct CylinderVertex
{
	CylinderVertex() {}
	CylinderVertex(const D3DXVECTOR3& p, const D3DXVECTOR3& n, const D3DXVECTOR3& t, const D3DXVECTOR2& uv)
		: Position(p), Normal(n), TangentU(t), TexC(uv) {}
	CylinderVertex(
		float px, float py, float pz,
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float u, float v)
		: Position(px, py, pz), Normal(nx, ny, nz),
		TangentU(tx, ty, tz), TexC(u, v) {}

	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	D3DXVECTOR3 TangentU;
	D3DXVECTOR2 TexC;
};

struct MeshData
{
	std::vector<CylinderVertex> Vertices;
	std::vector<UINT> Indices;
};