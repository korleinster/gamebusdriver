#pragma once
#include "Include.h"

class CScene;
class CObj;
class CRenderer
{
public:
	CRenderer();
	virtual ~CRenderer();
public:
	void SetCurrentScene(CScene* pScene);
	void AddRenderGroup(REDNERGROUP eRednerID, CObj* pObj);
public:
	HRESULT InitScene(void);
	void Render(const float& fTime);
public:
	static CRenderer* Create();
private:
private:
	void Render_Priority(void);
	void Render_NoneAlpha(void);
	void Render_Alpha(void);
	void Render_UI(void);
	void Render_FPS(const float& fTime);
public:
	void Release(void);
private:
	CScene*			m_pScene;
	float			m_fTime;
	TCHAR			m_szFps[128];
	DWORD			m_dwCount;
	list<CObj*>		m_RenderGroup[TYPE_END];
};

