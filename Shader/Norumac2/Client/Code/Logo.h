#pragma once
#include "Scene.h"

class CShader;
class CTexture;
class CVIBuffer;
class CLogo :
	public CScene
{
public:
	CLogo();
	virtual ~CLogo();
private:
	CShader*	m_pVertexShader;
	CShader*	m_pPixelShader;
	CTexture*	m_pTexture;
	CVIBuffer*	m_pPolygon;

public:
	virtual HRESULT Initialize(void);
	virtual int Update(void);
	virtual void Render(void);
	virtual void Release(void);

public:
	static CLogo* Create(void);

public:
	HRESULT AddBuufer(void);


};

