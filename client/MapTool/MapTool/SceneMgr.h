#pragma once


class CScene;
class CSceneMgr
{
private:
	CSceneMgr();
	virtual ~CSceneMgr();

public:
	DECLARE_SINGLETON(CSceneMgr)

private:
	CScene*				  m_pScene;
	map<Scene_Tyep, CScene*> m_mapScene;

public:
	Scene_Tyep				m_eType;

public:
	HRESULT	AddScene(Scene_Tyep _eType, CScene* _pScene);
	HRESULT	ChangeScene(Scene_Tyep _eType);
	int		Update(void);
	void	Render(void);
	Scene_Tyep GetScene(void);
	void Release(void);







};

