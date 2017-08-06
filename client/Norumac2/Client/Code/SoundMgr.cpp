#include "stdafx.h"
#include "SoundMgr.h"
#include <io.h>


IMPLEMENT_SINGLETON(CSoundMgr)

CSoundMgr::CSoundMgr( void )
{
	m_pSystem = NULL;
	m_iVersion = 0;

	Initialize();
}

CSoundMgr::~CSoundMgr( void )
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter = m_mapSound.begin();
	map<TCHAR*, FMOD_SOUND*>::iterator iter_end = m_mapSound.end();

	for(iter; iter != iter_end; ++iter)
	{
		m_Result = FMOD_Sound_Release(iter->second);
		ErrorCheck(m_Result);

		delete [](iter->first);
		iter = m_mapSound.erase(iter);
		iter_end = m_mapSound.end();
		if(iter == iter_end)
			break;
	}

	m_Result = FMOD_System_Close(m_pSystem);
	ErrorCheck(m_Result);

	m_Result = FMOD_System_Release(m_pSystem);
	ErrorCheck(m_Result);
}

void CSoundMgr::Initialize()
{
	m_Result = FMOD_System_Create(&m_pSystem);
	ErrorCheck(m_Result);

	m_Result = FMOD_System_GetVersion(m_pSystem, &m_iVersion);
	ErrorCheck(m_Result);
	
	m_Result = FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);
	ErrorCheck(m_Result);
}

void CSoundMgr::LoadSoundFile()
{
	_finddata_t	fd;
	long handle;
	int iResult = 1;

	handle = _findfirst("../Resource/SoundFile/*.*", &fd);  //현재 폴더 내 모든 파일을 찾는다.       
	if (handle == -1)
	{ 
		return;
	}      
	while (iResult != -1)     
	{ 
		TCHAR	szName[256];
		
		ZeroMemory(szName, sizeof(szName));
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, fd.name, strlen(fd.name), szName, 256);
		
		TCHAR*	pName = new TCHAR[256];
		ZeroMemory(pName, sizeof(TCHAR) * 256);
		lstrcpy(pName, szName);

		char szFullName[256];
		strcpy_s(szFullName, "..\\Resource\\SoundFile\\" );
		strcat_s(szFullName, fd.name);
		
		FMOD_SOUND*		pSound;
		m_Result = FMOD_System_CreateSound(m_pSystem, szFullName, FMOD_HARDWARE, 0, &pSound);

		if(m_Result == FMOD_OK)
			m_mapSound.insert(make_pair(pName, pSound));
		else
			delete [] pName;

		iResult = _findnext(handle, &fd); 
	}       
	_findclose(handle); 

	m_Result = FMOD_System_Update(m_pSystem);
	ErrorCheck(m_Result);
}

void CSoundMgr::PlaySound( TCHAR* szSoundKey )
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;
	iter =  find_if(m_mapSound.begin(), m_mapSound.end(), CStringCompare(szSoundKey));

	if(iter == m_mapSound.end())
		return;
	
	m_Result = FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_REUSE, iter->second, 0 ,&m_pEffCh);
	ErrorCheck(m_Result);
}

void CSoundMgr::PlayBgm( TCHAR* szSoundKey )
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;
	iter =  find_if(m_mapSound.begin(), m_mapSound.end(), CStringCompare(szSoundKey));

	if(iter == m_mapSound.end())
		return;

	FMOD_Sound_SetMode(iter->second, FMOD_LOOP_NORMAL);

	m_Result = FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_REUSE, iter->second, 0 ,&m_pBgaCh);
	ErrorCheck(m_Result);
}

void CSoundMgr::PlaySkillSound( TCHAR* szSoundKey )
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;
	iter =  find_if(m_mapSound.begin(), m_mapSound.end(), CStringCompare(szSoundKey));

	if(iter == m_mapSound.end())
		return;

	m_Result = FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_REUSE, iter->second, 0 ,&m_pSkillCh);
	ErrorCheck(m_Result);
}

void CSoundMgr::Monster1Sound( TCHAR* szSoundKey )
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;
	iter =  find_if(m_mapSound.begin(), m_mapSound.end(), CStringCompare(szSoundKey));

	if(iter == m_mapSound.end())
		return;

	m_Result = FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_REUSE, iter->second, 0 ,&m_pMonster1);
	ErrorCheck(m_Result);
}

void CSoundMgr::Monster2Sound( TCHAR* szSoundKey )
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;
	iter =  find_if(m_mapSound.begin(), m_mapSound.end(), CStringCompare(szSoundKey));

	if(iter == m_mapSound.end())
		return;

	m_Result = FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_REUSE, iter->second, 0 ,&m_pMonster2);
	ErrorCheck(m_Result);
}

void CSoundMgr::Monster3Sound( TCHAR* szSoundKey )
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;
	iter =  find_if(m_mapSound.begin(), m_mapSound.end(), CStringCompare(szSoundKey));

	if(iter == m_mapSound.end())
		return;

	m_Result = FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_REUSE, iter->second, 0 ,&m_pMonster3);
	ErrorCheck(m_Result);
}

void CSoundMgr::Monster4Sound( TCHAR* szSoundKey )
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;
	iter =  find_if(m_mapSound.begin(), m_mapSound.end(), CStringCompare(szSoundKey));

	if(iter == m_mapSound.end())
		return;

	m_Result = FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_REUSE, iter->second, 0 ,&m_pMonster4);
	ErrorCheck(m_Result);
}

void CSoundMgr::PlaySkillSound2(TCHAR* szSoundKey)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;
	iter =  find_if(m_mapSound.begin(), m_mapSound.end(), CStringCompare(szSoundKey));

	if(iter == m_mapSound.end())
		return;

	m_Result = FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_REUSE, iter->second, 0 ,&m_pEffCh2);
	ErrorCheck(m_Result);
}

void CSoundMgr::PlaySkillSound3(TCHAR* szSoundKey)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;
	iter =  find_if(m_mapSound.begin(), m_mapSound.end(), CStringCompare(szSoundKey));

	if(iter == m_mapSound.end())
		return;

	m_Result = FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_REUSE, iter->second, 0 ,&m_pEffCh3);
	ErrorCheck(m_Result);
}

void CSoundMgr::PlayInterface1(TCHAR* szSoundKey)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;
	iter =  find_if(m_mapSound.begin(), m_mapSound.end(), CStringCompare(szSoundKey));

	if(iter == m_mapSound.end())
		return;

	m_Result = FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_REUSE, iter->second, 0 ,&m_pQuest1);
	ErrorCheck(m_Result);
}

void CSoundMgr::PlayInterface2(TCHAR* szSoundKey)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;
	iter =  find_if(m_mapSound.begin(), m_mapSound.end(), CStringCompare(szSoundKey));

	if(iter == m_mapSound.end())
		return;

	m_Result = FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_REUSE, iter->second, 0 ,&m_pQuest2);
	ErrorCheck(m_Result);
}

void CSoundMgr::PlayInterface3(TCHAR* szSoundKey)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;
	iter =  find_if(m_mapSound.begin(), m_mapSound.end(), CStringCompare(szSoundKey));

	if(iter == m_mapSound.end())
		return;

	m_Result = FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_REUSE, iter->second, 0 ,&m_pQuest3);
	ErrorCheck(m_Result);
}

void CSoundMgr::RedBullSound(TCHAR* szSoundKey)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;
	iter =  find_if(m_mapSound.begin(), m_mapSound.end(), CStringCompare(szSoundKey));

	if(iter == m_mapSound.end())
		return;

	m_Result = FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_REUSE, iter->second, 0 ,&m_pRedBull);
	ErrorCheck(m_Result);
}

void CSoundMgr::RedBullSoundStop()
{
	m_Result = FMOD_Channel_Stop(m_pRedBull);
	ErrorCheck(m_Result);
}

void CSoundMgr::UpdateSound()
{
	m_Result = FMOD_System_Update(m_pSystem);
	ErrorCheck(m_Result);
}

void CSoundMgr::StopBga()
{
	m_Result = FMOD_Channel_Stop(m_pBgaCh);
	ErrorCheck(m_Result);
}

void CSoundMgr::StopSoundAll()
{
	m_Result = FMOD_Channel_Stop(m_pEffCh);
	ErrorCheck(m_Result);
	m_Result = FMOD_Channel_Stop(m_pBgaCh);
	ErrorCheck(m_Result);
	m_Result = FMOD_Channel_Stop(m_pSkillCh);
	ErrorCheck(m_Result);
	m_Result = FMOD_Channel_Stop(m_pMonster1);
	ErrorCheck(m_Result);
	m_Result = FMOD_Channel_Stop(m_pMonster2);
	ErrorCheck(m_Result);
	m_Result = FMOD_Channel_Stop(m_pMonster3);
	ErrorCheck(m_Result);
	m_Result = FMOD_Channel_Stop(m_pMonster4);
	ErrorCheck(m_Result);
	m_Result = FMOD_Channel_Stop(m_pMonster5);
	ErrorCheck(m_Result);
}

void CSoundMgr::ErrorCheck( FMOD_RESULT _res )
{
	
}

void CSoundMgr::PlaySoundByNum( int iNum )
{
	if((size_t)iNum > m_mapSound.size())
		return;

	map<TCHAR*, FMOD_SOUND*>::iterator iter = m_mapSound.begin();
	for(int i = 0; i < iNum + 1; ++i)
		++iter;
	
	m_Result = FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_REUSE, iter->second, 0 ,&m_pEffCh);
	ErrorCheck(m_Result);
}

