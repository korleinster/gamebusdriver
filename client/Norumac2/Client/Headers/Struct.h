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
	//D3DXVECTOR2				vTexUV2;
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
//const DWORD VTXFVF_CUBE = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);

typedef struct tagIndex16
{
	WORD			_1, _2, _3;
}INDEX16;

typedef struct tagIndex32
{
	UINT			_1, _2, _3;
}INDEX32;



