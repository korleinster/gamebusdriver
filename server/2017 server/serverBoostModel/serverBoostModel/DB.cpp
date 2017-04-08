#include "stdafx.h"

void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode)
{
	SQLSMALLINT iRec = 0;
	SQLINTEGER  iError;
	WCHAR       wszMessage[1000];
	WCHAR       wszState[SQL_SQLSTATE_SIZE + 1];

	if (RetCode == SQL_INVALID_HANDLE)
	{
		fwprintf(stderr, L"Invalid handle!\n");
		return;
	}

	while (SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage, (SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)), (SQLSMALLINT *)NULL) == SQL_SUCCESS)
	{
		// Hide data truncated..
		if (wcsncmp(wszState, L"01004", 5)) { fwprintf(stderr, L"[%5.5s] %s (%d)\n", wszState, wszMessage, iError); }
	}

}

void DB::Init() {
	// Allocate an environment

	if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv) == SQL_ERROR) {
		fwprintf(stderr, L"DBacess class :: Unable to allocate an environment handle\n");
		exit(-1);
	}

	TRYODBC(hEnv, SQL_HANDLE_ENV, SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0));
	TRYODBC(hEnv, SQL_HANDLE_ENV, SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc));
	TRYODBC(hDbc, SQL_HANDLE_DBC, SQLDriverConnect(hDbc, GetDesktopWindow(), SQLSERVERADDR, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE));
	TRYODBC(hDbc, SQL_HANDLE_DBC, SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt));

	fwprintf(stderr, L"Azure SQL Database Server Connected!\n");
	// Loop to get input and execute queries -- 연결은 여기까지 완료 되었고, 여기서 부터 명령어를 보내면 실행하게 된다.
}

void DB::Release() {
	// Free ODBC handles and exit

	if (hStmt) { SQLFreeHandle(SQL_HANDLE_STMT, hStmt); }

	if (hDbc) {
		SQLDisconnect(hDbc);
		SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	}

	if (hEnv) { SQLFreeHandle(SQL_HANDLE_ENV, hEnv); }

	wprintf(L"\nAzure SQL DataBase Server Disconnected.\n");
}

bool DB::DB_Login(wchar_t* id, wchar_t* pw) {
	wchar_t input_id[MAX_BUF_SIZE / 4]{ 0 };
	wchar_t input_pw[MAX_BUF_SIZE / 4]{ 0 };
	wchar_t Nickname[MAX_BUF_SIZE / 4]{ 0 };
	int PlayerLevel{ 0 };
	bool Admin{ false };

	SQLLEN pIndicators[5];		// DB 상 안에 있는 목차 갯 수

	if (SQLExecDirect(hStmt, (SQLWCHAR*)L"SELECT RTRIM(ID), RTRIM(Password), RTRIM(Nickname), RTRIM(PlayerLevel), RTRIM(Admin) FROM dbo.user_data", SQL_NTS) != SQL_ERROR) {
		// 숫자 1 은, 첫번째 파라미터 ID
		// 뒤에 배열은 몇 바이트 받았는지 담음
		SQLBindCol(hStmt, 1, SQL_C_WCHAR, (SQLPOINTER)&input_id, MAX_BUF_SIZE / 4, &pIndicators[0]);
		SQLBindCol(hStmt, 2, SQL_C_WCHAR, (SQLPOINTER)&input_pw, MAX_BUF_SIZE / 4, &pIndicators[1]);
		SQLBindCol(hStmt, 3, SQL_C_WCHAR, (SQLPOINTER)&Nickname, MAX_BUF_SIZE / 4, &pIndicators[2]);
		SQLBindCol(hStmt, 4, SQL_C_LONG, (SQLPOINTER)&PlayerLevel, sizeof(PlayerLevel), &pIndicators[3]);
		SQLBindCol(hStmt, 5, SQL_C_BIT, (SQLPOINTER)&Admin, sizeof(Admin), &pIndicators[4]);

		while (SQLFetch(hStmt) == SQL_SUCCESS)
		{
			if ((wcscmp(input_id, id) == 0) && (wcscmp(input_pw, pw) == 0)) {

				// 확인이 되었다면, 여기서 데이터를 복사해야 하는데..

				return true;
			}
		}
	}

	return false;
}

void DB::SQLcmd(SQLWCHAR* str) {

	RETCODE     RetCode;
	SQLSMALLINT sNumResults;

	RetCode = SQLExecDirect(hStmt, str, SQL_NTS);

	switch (RetCode)
	{
	case SQL_SUCCESS_WITH_INFO:	// 뭔가 실패는 했는데, 일단 계속 실행 할 수 있을 경우...
	{
		HandleDiagnosticRecord(hStmt, SQL_HANDLE_STMT, RetCode);
	}
	case SQL_SUCCESS:
	{
		// 성공했을 시... 코드 실행
		// SQLNumResultCols() 함수는 결과 세트의 해당 열 수를, 변수에 리턴 받는다. ( 열 = -> )
		TRYODBC(hStmt, SQL_HANDLE_STMT, SQLNumResultCols(hStmt, &sNumResults));

		// 결과가 0줄 이상이면 일을 처리한다. 꿍야꿍야...
		if (sNumResults > 0) {

		}
		else
		{
			SQLINTEGER cRowCount;

			// 반대로 SQLRowCount() 함수는 행 수를, 변수에 리턴 받는다. ( 행 = V )
			TRYODBC(hStmt, SQL_HANDLE_STMT, SQLRowCount(hStmt, reinterpret_cast<SQLLEN *>(&cRowCount)));

			// 행이 0 이상이면 데이터를 출력하는 형태...
			if (cRowCount >= 0) {
				wprintf(L"%Id %s affected\n", cRowCount, cRowCount == 1 ? L"row" : L"rows");
			}
		}
		break;
	}
	case SQL_ERROR: // 실패 했을 경우, 아래 함수를 통해서 에러 결과를 화면에 출력해준다.
	{
		HandleDiagnosticRecord(hStmt, SQL_HANDLE_STMT, RetCode);
	}
	default:
		fwprintf(stderr, L"Unexpected return code %hd!\n", RetCode);
	}


}
