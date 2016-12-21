#pragma once
#include "Resources.h"
class CTexture :
	public CResources
{
private:
	CTexture();
	explicit CTexture(const CTexture& rhs);
public:
	virtual ~CTexture();

public:
	ID3D11ShaderResourceView*           m_pTextureRV; // ¼ÎÀÌ´õ ÀÚ¿ø ºä
	ID3D11SamplerState*                 m_pSamplerLinear;
	ID3D11BlendState*               m_pBlendState;

private:
	virtual HRESULT CreateTexture(LPCWSTR szFilePath);
	virtual HRESULT CreateCubeTexture(LPCWSTR szFilePath);
public:
	static CTexture* Create(LPCWSTR szFilePath);
	void Render(void);
public:
	virtual CResources* CloneResource(void);

private:
	DWORD Release(void);


};