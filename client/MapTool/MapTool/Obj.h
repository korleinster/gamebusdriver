#pragma once

class CDevice;
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
	CDevice*					m_pGrapicDevice;
	ReleaseType					m_eReleaseType;
	vector<string>				vecAniName;
	VTXTEX*						m_pVerTex;
	
public:
	TCHAR						m_tcKey[256];
	TCHAR						m_tcMeshKey[256];

public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void) {};
	virtual void Release(void);

public:
	static CObj* Create(ReleaseType _eType);
	

public:
	const float&	GetViewZ(void) const;
	CInfo*			GetInfo(void) { return m_pInfo; }
	ReleaseType&	GetReleaseType(void) { return m_eReleaseType; }
	const CComponent*	GetComponent(const wstring&	_wstrComponentKey);
	VTXTEX*	GetVertex(void) { return m_pVerTex; }
	TCHAR* GetKey(void){ return m_tcKey; }
	TCHAR* GetMeshKey(void) { return m_tcMeshKey; }
	

public:
	void SetPos(D3DXVECTOR3 vPos);
	void SetScale(D3DXVECTOR3 vScale);
	void SetAngle(D3DXVECTOR3 vAngle);

protected:
	void Compute_ViewZ(const D3DXVECTOR3* pPos);
};

