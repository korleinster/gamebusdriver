#pragma once
#include "Component.h"

class CShader : public CComponent
{
private:
	explicit CShader();
	explicit CShader(const CShader& rhs);
public:
	virtual ~CShader();

public:
	ID3D11VertexShader*     m_pVertexShader;
	ID3D11PixelShader*      m_pPixelShader; // 이 둘은 LPD3DXEFFECT를 반으로 쪼개놓은 형태이다

	ID3D11HullShader*		m_pHullShader; // DX11 에서 새로 생긴 쉐이더... VS와 PS의 사이에 작동함
	ID3D11DomainShader*		m_pDomainShader;
	ID3D11GeometryShader*	m_pGeometryShader;

	ID3D11InputLayout*      m_pVertexLayout; // 다이렉트9의 FVF를 대신한다

private:
	DWORD m_dwRefCount;

private:
	HRESULT Ready_ShaderFile(wstring wstrFilePath, LPCSTR wstrShaderName, LPCSTR wstrShaderVersion, ShaderType _SType);
	


public:
	static CShader*	Create(wstring wstrFilePath, LPCSTR wstrShaderName, LPCSTR wstrShaderVersion, ShaderType _SType);
	CShader*	CloneShader(void);
	DWORD	Release(void);


};

