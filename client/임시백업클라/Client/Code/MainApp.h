#pragma once
class CDevice;
class CInfo;
class CVIBuffer;
class CShader;
class CTexture;
class CScene;
class CMainApp
{
public:
	CMainApp();
	virtual ~CMainApp();

private:
	CDevice*	m_pGrapicDevcie;
	CVIBuffer*  m_pRcCol;
	CInfo*		m_pInfo;
	CShader*	m_pVertexShader;
	CShader*	m_pPixelShader;
	CTexture*	m_pTexture;

public:
	HRESULT Initialize(void);
	int		Update(void);
	void	Render(void);
	void Release(void);

public:
	static CMainApp* Create(void);
	
};

