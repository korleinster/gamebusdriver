#pragma once
class CScene
{
public:
	CScene();
	virtual ~CScene();

public:
	virtual HRESULT Initialize(void) { return S_OK; }
	virtual int		Update(void) { return 0; }
	virtual void	Render(void)PURE; //{}
	virtual void	Release(void) {}
};

