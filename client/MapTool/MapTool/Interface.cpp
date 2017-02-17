// Interface.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "Interface.h"
#include "afxdialogex.h"


// CInterface 대화 상자입니다.

IMPLEMENT_DYNAMIC(CInterface, CDialog)

CInterface::CInterface(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_INTERFACE, pParent)
{

}

CInterface::~CInterface()
{
}

void CInterface::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInterface, CDialog)
END_MESSAGE_MAP()


// CInterface 메시지 처리기입니다.


BOOL CInterface::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (pMsg->message == WM_KEYDOWN &&
		(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE))
	{
		pMsg->wParam = NULL;
	}

	return CDialog::PreTranslateMessage(pMsg);
}
