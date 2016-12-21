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
	ID3D11InputLayout*      m_pVertexLayout; // 입력배치 객체(정점 구조체의 각 성분이 어떤 용도인지 알려줌)
	ID3D11PixelShader*      m_pPixelShader;

private:
	DWORD m_dwRefCount;

private:
	HRESULT Ready_ShaderFile(wstring wstrFilePath, LPCSTR wstrShaderName, LPCSTR wstrShaderVersion, ShaderType _SType);
	DWORD	Release(void);


public:
	static CShader*	Create(wstring wstrFilePath, LPCSTR wstrShaderName, LPCSTR wstrShaderVersion, ShaderType _SType);
	CShader*	CloneShader(void);
	

};

