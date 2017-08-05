#pragma once
#include "UI.h"

class CFont;
class CFeverBar : public CUI
{
public:
	CFeverBar();
	virtual ~CFeverBar();
	//

public:
	virtual HRESULT Initialize(void);
	virtual int Update(void);
	virtual void Render(void);
	void UpdateBufferToFever(void);

public:
	virtual HRESULT AddComponent(void);

public:
	CFont* m_pFont;

public:
	static CFeverBar* Create();


};