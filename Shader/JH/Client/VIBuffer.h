#pragma once
#include "Resources.h"
#include "Include.h"


class CVIBuffer :
	public CResources
{
public:
	CVIBuffer();
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer();

public:
	ID3D11RasterizerState*		m_pRasterizerState; // 파이프라인의 래슽화기 단계를 구성하는데 쓰이는 설정들을 묶은 상태집합을 대표함

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
public:
	virtual void	Render(void);
	virtual DWORD	Release(void);
protected:
	virtual void	CreateRasterizerState();

	//ID3D11VertexShader*	m_pVertexShader;
	//ID3D11InputLayout*	m_pVertexLayout;


};

