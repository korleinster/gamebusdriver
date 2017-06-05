#pragma once
#include "UI.h"


class CFont;
class CMobHpBasic : public CUI
{
public:
	CMobHpBasic();
	virtual ~CMobHpBasic();
	//

public:
	virtual HRESULT Initialize(void);
	virtual int Update(void);
	virtual void Render(void);
	void SetName(wstring wstrText, int iID);
	void ResetRendTime(void) { m_fRendTime = 0.f; }

public:
	virtual HRESULT AddComponent(void);

public:
	CFont* m_pFont;
	float m_fRendTime;


public:
	static CMobHpBasic* Create();


};