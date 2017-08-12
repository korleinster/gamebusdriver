#pragma once

class CLoading
{
public:
	enum LOADINGID { LOADING_STAGE, LOADING_STAGE1 };

public:
	explicit CLoading(LOADINGID eLoadID);
	~CLoading(void);

public:
	bool GetComplete(void);
	const TCHAR* GetLoadMessage(void);

public:
	HRESULT InitLoading(void);
	void StageLoading(void);
	void TextureLoad(void);
	void StaticMeshLoad(void);

public:
	static CLoading* Create(LOADINGID eLoadID);
	static UINT WINAPI LoadingFunction(void* pArg);

public:
	void Release(void);

private:
	LOADINGID				m_eLoadID;
	CRITICAL_SECTION		m_CSKey;
	HANDLE					m_hThread;

private:
	TCHAR		m_szLoadMessage[128];
	bool		m_bComplete;
	
};

//class CLoading
//{
//public:
//	enum LOADINGID { LOADING_STAGE, LOADING_STAGE1 };
//
//public:
//	explicit CLoading(/*LOADINGID eLoadID*/);
//	~CLoading(void);
//
//public:
//	bool GetComplete(void);
//	const TCHAR* GetLoadMessage(void);
//
//public:
//	HRESULT InitLoading(LOADINGID eLoadID);
//	static HRESULT StageLoading();
//
//public:
//	static CLoading* Create(LOADINGID eLoadID);
//	static HRESULT /*WINAPI*/ LoadingFunction(LOADINGID eLoadID);
//
//public:
//	void Release(void);
//
//private:
//	LOADINGID				m_eLoadID;
//	//CRITICAL_SECTION		m_CSKey;
//	//HANDLE					m_hThread;
//
//private:
//	TCHAR		m_szLoadMessage[128];
//	bool		m_bComplete;
//};