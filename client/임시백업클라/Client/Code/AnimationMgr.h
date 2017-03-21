#pragma once

class CAnimationMgr
{
private:
	CAnimationMgr();
	virtual ~CAnimationMgr();

public:
	DECLARE_SINGLETON(CAnimationMgr)

public:
	map<wstring, vector<string>> m_mapAni;

public:
	HRESULT AddAnimation(wstring wstrKey, vector<string>* vecAni);
	HRESULT DelAnimation(wstring wstrKey);
	vector<string>* GetAnimaiton(wstring wstrKey);
};