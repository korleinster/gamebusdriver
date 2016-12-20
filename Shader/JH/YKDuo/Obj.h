#pragma once

class CComponent;
class CInfo;
class CObj
{
public:
	CObj();
	virtual ~CObj();

protected:
	map<wstring, CComponent*>	m_mapComponent;
	CInfo*						m_pInfo;
	float						m_fViewZ;
	bool						m_bZSort;

public:
	HRESULT Initialize(void) { return S_OK; }
	int		Update(void);
	void	Render(void) {}

private:
	void Release(void) {}

public:
	const float&	GetViewZ(void) const;
protected:
	void Compute_ViewZ(const D3DXVECTOR3* pPos);
};

