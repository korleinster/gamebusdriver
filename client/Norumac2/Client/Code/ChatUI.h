#pragma once
#include "UI.h"


class CFont;
class CChatUI : public CUI
{
public:
	CChatUI();
	virtual ~CChatUI();
	//

public:
	virtual HRESULT Initialize(void);
	virtual int Update(void);
	virtual void Render(void);

public:
	virtual HRESULT AddComponent(void);

public:
	list<CFont*> m_ChatLogList;


public:
	static CChatUI* Create();


};