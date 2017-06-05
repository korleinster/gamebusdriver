#pragma once
#include "UI.h"

class CMobHpBar : public CUI
{
public:
	CMobHpBar();
	virtual ~CMobHpBar();
	//

public:
	virtual HRESULT Initialize(void);
	virtual int Update(void);
	virtual void Render(void);
	void ResetRendTime(void) { m_fRendTime = 0.f; }
	void SetRendHp(int iHp, float fMaxHp);

public:
	virtual HRESULT AddComponent(void);

public:
	float m_fRendTime;

public:
	static CMobHpBar* Create();


};