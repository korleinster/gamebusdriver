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
	ID3D11ShaderResourceView*           m_pTextureRV;
	ID3D11SamplerState*                 m_pSamplerLinear;
	ID3D11BlendState*					m_pBlendState;

private:
	virtual HRESULT CreateTexture(LPCWSTR szFileFath);
public:
	static CTexture* Create(LPCWSTR szFileFath);
public:
	virtual CResources* CloneResource(void);

private:
	DWORD Release(void);


};

