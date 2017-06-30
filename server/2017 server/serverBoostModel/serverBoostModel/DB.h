#pragma once
#include "player_session.h"

// DB 접근용 class
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <tchar.h>

void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);

#define TRYODBC(h, ht, x)   {   RETCODE rc = x;\
                                if (rc != SQL_SUCCESS)	{ HandleDiagnosticRecord (h, ht, rc); } \
                                if (rc == SQL_ERROR)	{ fwprintf(stderr, L"Error in " L#x L"\n"); }} //\
                                    //goto Exit;  \
                                }  \
                            }
#define SQLSERVERADDR	L"Driver={ODBC Driver 13 for SQL Server};Server=tcp:mygame.database.windows.net,1433;Database=myDatabase;Uid=uzchowall@mygame;Pwd={rlagudwns1!};Encrypt=yes;TrustServerCertificate=no;Connection Timeout=30;"

// 데이터 추가
/// L"INSERT INTO dbo.user_data ( ID, Password, Nickname, PlayerLevel, AdminLevel ) VALUES ( '2013180056', 'ghdtmdvlf', N'홍승필', 1, 0 )"
// 데이터 참조 ( http://blog.naver.com/khjkhj2804/220725944795 )
/// if (SQLExecDirect(hstmt, (SQLWCHAR*)L"SELECT ID, Password, Nickname, PlayerLevel, PositionX, PositionY FROM dbo.user_data", SQL_NTS) != SQL_ERROR)
/// L"SELECT RTRIM(ID), RTRIM(Password), RTRIM(Nickname), RTRIM(PlayerLevel), RTRIM(Admin) FROM dbo.user_data"

class DB {
public:
	DB() { Init(); };
	~DB() { Release(); };

	void SQLcmd(SQLWCHAR* str);
	bool DB_Login(wchar_t* id, wchar_t* pw, player_session *ps);
	bool DB_Update(wchar_t* id, player_session *ps);

	void Init();
private:
	void Release();

	SQLHENV     hEnv = NULL;
	SQLHDBC     hDbc = NULL;
	SQLHSTMT    hStmt = NULL;
};