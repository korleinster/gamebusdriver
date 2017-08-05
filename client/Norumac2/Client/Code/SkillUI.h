#pragma once
#include "UI.h"

class CFont;
class CSkillUI : public CUI
{
public:
	CSkillUI();
	virtual ~CSkillUI();
	//

public:
	virtual HRESULT Initialize(SKILLUITYPE eUiType, wstring wstUiImage, D3DXVECTOR2 vUIPos);
	virtual int Update(void);
	virtual void Render(void);
	void UpdateBufferToCool(void);

public:
	virtual HRESULT AddComponent(wstring wstUiImage);

public:
	SKILLUITYPE m_eUiType;

public:
	static CSkillUI* Create(SKILLUITYPE eUiType, wstring wstUiImage, D3DXVECTOR2 vUIPos);


};