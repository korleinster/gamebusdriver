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
