#pragma once
#include "stdafx.h"

////콘스턴트 버퍼
struct ConstantBuffer
{
	D3DXMATRIX matWorld;
	D3DXMATRIX matView;
	D3DXMATRIX matProjection;
};

struct SelectBuffer
{
	D3DXVECTOR4	vSelect;
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


struct VertexAni
{
	D3DXVECTOR3		vPos;
	D3DXVECTOR3		vNormal;
	D3DXVECTOR2		vTextureUV;

	int				iBoneIdx[BONE_NUM];
	float			fBoneWeight[BONE_NUM];

	VertexAni() {}
	VertexAni(D3DXVECTOR3 _vPos, D3DXVECTOR3 _vNormal)
		: vPos(_vPos), vNormal(_vNormal) {}
	VertexAni(D3DXVECTOR3 _vPos, D3DXVECTOR3 _vNormal, D3DXVECTOR2 _vTextureUV)
		: vPos(_vPos), vNormal(_vNormal), vTextureUV(_vTextureUV) {}

	void SetPos(float _x, float _y, float _z) { vPos.x = _x; vPos.y = _y; vPos.z = _z; }
	void SetNormal(float _x, float _y, float _z) { vNormal.x = _x; vNormal.y = _y; vNormal.z = _z; }
	void AddBone(int _iIdx, float _fWeight)
	{
		for (int i = 0; i < BONE_NUM; ++i)
		{
			if (fBoneWeight[i] <= 0.0f)
			{
				iBoneIdx[i] = _iIdx;
				fBoneWeight[i] = _fWeight;
				return;
			}
		}

		fBoneWeight[0] += _fWeight;
	};

};
