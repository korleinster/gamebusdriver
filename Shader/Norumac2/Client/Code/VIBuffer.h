#pragma once
#include "Resources.h"
#include "Include.h"

const WORD BONE_NUM = 8;
const WORD BONE_MATRIX_NUM = 128;

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
	void AddBone(int _iIdx, float _fWeight);
};


class CVIBuffer :
	public CResources
{
public:
	CVIBuffer();
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer();

public:
	UINT				m_nInstanceBufferStride;
	UINT				m_nInstanceBufferOffset;

	ID3D11Buffer*		m_ConstantBuffer;
	ID3D11Buffer*		m_VertexBuffer;
	//ID3D11InputLayout*  m_VertexLayout;
	int					m_iVertices;
	int					m_iBuffers;
	UINT				m_iSlot;
	UINT				m_iStartVertex;
	UINT				m_iVertexStrides;
	UINT				m_iVertexOffsets;

	ID3D11Buffer*		m_IndexBuffer;
	UINT				m_iIndex;
	UINT				m_iStartIndex;
	int					m_iBaseVertex;
	DXGI_FORMAT			m_dxgiIndexFormat;
	UINT				m_iIndexOffset;

	DrawType			m_eDrawType;

	//레스터 라이저 스테이트
	ID3D11RasterizerState*		m_pRasterizerState;

public:
	virtual void	Render(void);
	virtual DWORD	Release(void);
	virtual void	CreateRasterizerState();
	

	
	//ID3D11VertexShader*	m_pVertexShader;
	//ID3D11InputLayout*	m_pVertexLayout;


};

