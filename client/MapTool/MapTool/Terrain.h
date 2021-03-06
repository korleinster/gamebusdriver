#pragma once
#include "Obj.h"

class CVIBuffer;
class CComponent;
class CShader;
class CTexture;
class CTerrain :
	public CObj
{
private:
	CTerrain();
public:
	virtual ~CTerrain();

private:
	CVIBuffer*	m_pTerrainBuffer;
	CShader*	m_pVertexShader;
	CShader*	m_pPixelShader;
	CTexture*	m_pTexture;
	
	VTXTEX*		m_pConvertVerTex;

public:
	static CTerrain* Create(void);

public:
	bool m_bRender;

public:
	HRESULT Initialize(void);
	int		Update(void);
	void	Render(void);
	void	Release(void);


private:
	
	HRESULT	AddComponent(void);

};

