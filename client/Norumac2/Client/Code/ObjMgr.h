#pragma once

class CObj;
class CObjMgr
{
private:
	CObjMgr();
	virtual ~CObjMgr();

public:
	DECLARE_SINGLETON(CObjMgr)

public:
	map<wstring, list<CObj*>> m_mapObj;

public:
	HRESULT SceneChange(void);
	HRESULT AddObject(wstring wstrKey, CObj* pObj);
	void	Update(void);
	player_data* 	Get_PlayerServerData(UINT uID);
	player_data*	Get_MonsterServerData(UINT uID);
	player_data*	Get_BossServerData(UINT uID);
	list<CObj*>*  Get_ObjList(wstring wstrKey);
	void Release(void);

};

