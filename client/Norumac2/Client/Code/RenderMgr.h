#pragma once
#include "Include.h"

class CScene;
class CObj;
class CDevice;
class CTargetMgr;
class CLightMgr;
class CShader;
class CFont;
class CRenderMgr
{
public:
	enum RGB_COLOR// 추가
	{
		RGB_RED,
		RGB_GREEN,
		RGB_BLUE,
		RGB_END
	};
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

	void Render_BorderLine(void);

	void Render_ShadowMap(void);

public:
	void Release(void);
	void ListClear(void);

private:
	CScene*				m_pScene;
	CDevice*			m_pDevice;// 추가
	CTargetMgr*			m_pTargetMgr;// 추가
	CLightMgr*			m_pLightMgr;// 추가
	bool				m_bRenderGBuffer;
	bool				m_bDefferdOn;
	float				m_fTimeCheck;
private:
	float				m_fTime;
	TCHAR				m_szFps[128];
	DWORD				m_dwCount;
	list<CObj*>			m_RenderGroup[TYPE_END];
	CFont*				m_pFPSFont;
private:
	void Input(float fTime); // 추가
	D3DXVECTOR3			m_vDirLight;// 추가
	float				m_fDirColor[RGB_END];// 추가
	float				m_fPointColor[RGB_END];// 추가
	float				m_fPointY;
	float				m_fPointRadius;
private:
	CShader*			m_pBorderLineVS;
	CShader*			m_pBorderLinePS;
	class CVIBuffer*	m_pRcTex;
	ID3D11SamplerState*	m_pSamplerState;
	ID3D11Buffer*		m_pLinearDepthCB;

	float				m_fSobelValue;
};

