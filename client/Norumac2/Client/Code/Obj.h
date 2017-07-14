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
	//OBJECT_DIR					m_eObjDir;
	TCHAR						m_tcKey[256];
	TCHAR						m_tcMeshKey[256];
	bool						m_bCurred;
	float						m_fSpeed;
	
	

public:
	bool						m_bDeath = false;
	bool						m_bMoveForServer;//서버에서 이동을 하는가?

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
	

public:
	void SetPos(D3DXVECTOR3 vPos); 
	void SetScale(D3DXVECTOR3 vScale);
	player_data*	GetPacketData(void);
	void SetPacketData(player_data* pPacketData);
	void SetPacketHp(int* pPacketData);
	void SetPacketFever(short* pPacketData);

protected:
	void Compute_ViewZ(const D3DXVECTOR3* pPos);
};

