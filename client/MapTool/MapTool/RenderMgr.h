#pragma once
#include "Include.h"

class CScene;
class CObj;
class CRenderMgr
{
private:
	CRenderMgr();
	virtual ~CRenderMgr();

public:
	DECLARE_SINGLETON(CRenderMgr)

public:
	void SetCurrentScene(CScene* pScene);

public:
	HRESULT InitScene(void);
	void Render(const float& fTime);


public:
	void AddRenderGroup(RENDERGROUP eRednerID, CObj* pObj);
	void DelRenderGroup(RENDERGROUP eRednerID, CObj* pObj);

private:
	void Render_Priority(void);
	void Render_NoneAlpha(void);
	void Render_Alpha(void);
	void Render_UI(void);
	void Render_FPS(const float& fTime);

public:
	void Release(void);
	void ListClear(void);

private:
	CScene*		m_pScene;

private:
	float		m_fTime;
	TCHAR		m_szFps[128];
	DWORD		m_dwCount;
	list<CObj*>		m_RenderGroup[TYPE_END];
};

