#pragma once
#include "UI.h"


class CFont;
class CQuestUI : public CUI
{
public:
	CQuestUI();
	virtual ~CQuestUI();
	//

public:
	virtual HRESULT Initialize(void);
	virtual int Update(void);
	virtual void Render(void);

public:
	virtual HRESULT AddComponent(void);

public:
	CFont*		m_QuestScript;
	CFont*		m_QuestState;
	bool		m_bRender;
	float		m_fCoolTime;


public:
	static CQuestUI* Create();


};