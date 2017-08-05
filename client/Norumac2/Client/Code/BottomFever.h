#pragma once
#include "UI.h"

class CFont;
class CBottomFever : public CUI
{
public:
	CBottomFever();
	virtual ~CBottomFever();
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
	static CBottomFever* Create();


};