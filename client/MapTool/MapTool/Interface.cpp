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
	, m_fCamPosX(0)
	, m_fCamPosY(0)
	, m_fCamPosZ(0)
	, m_CamAtX(0)
	, m_CamAtY(0)
	, m_CamAtZ(0)
	, m_CamState(_T("고정풀림"))
	, m_iCpu(0)
	, m_iFps(0)
	, m_iRamState(0)
	, m_iRamMaximum(0)
	, m_Navi1(_T("안찍힘"))
	, m_Navi2(_T("안찍힘"))
	, m_Navi3(_T("안찍힘"))
	, m_iCamSpeed(0)
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

void CInterface::CamUpdate(D3DXVECTOR3 vCamPos, D3DXVECTOR3 vCamAt, bool bCamState, int iCamSpeed)
{

	UpdateData(TRUE);
	m_fCamPosX = vCamPos.x;
	SetDlgItemInt(IDC_EDIT1, (float)m_fCamPosX);
	m_fCamPosY = vCamPos.y;
	SetDlgItemInt(IDC_EDIT2, (float)m_fCamPosY);
	m_fCamPosZ = vCamPos.z;
	SetDlgItemInt(IDC_EDIT3, (float)m_fCamPosZ);

	m_CamAtX = vCamAt.x;
	SetDlgItemInt(IDC_EDIT4, (float)m_CamAtX);
	m_CamAtY = vCamAt.y;
	SetDlgItemInt(IDC_EDIT5, (float)m_CamAtY);
	m_CamAtZ = vCamAt.z;
	SetDlgItemInt(IDC_EDIT6, (float)m_CamAtZ);

	if (bCamState == false)
		m_CamState = L"고정 모드";
	else if (bCamState == true)
		m_CamState = L"이동 모드";

	SetDlgItemText(IDC_EDIT7, (CString)m_CamState);

	m_iCamSpeed = iCamSpeed;
	SetDlgItemInt(IDC_EDIT15, (int)m_iCamSpeed);
	UpdateData(FALSE);
}

void CInterface::HardwareUpdate(int iCpu, int iRam, int iRamMax)
{
	UpdateData(TRUE);

	m_iCpu = iCpu;
	SetDlgItemInt(IDC_EDIT8, m_iCpu);


	m_iRamState = iRam;
	SetDlgItemInt(IDC_EDIT9, m_iRamState);

	m_iRamMaximum = iRamMax;
	SetDlgItemInt(IDC_EDIT14, m_iRamMaximum);

	UpdateData(FALSE);
}

void CInterface::FpsUpdate(int iFps)
{
	UpdateData(TRUE);

	m_iFps = iFps;
	SetDlgItemInt(IDC_EDIT10, m_iFps);

	UpdateData(FALSE);
}

void CInterface::NaviUpdate(bool bFirst, bool bSecond)
{
	UpdateData(TRUE);
	if (bFirst == false)
		m_Navi1 = L"안찍힘";
	else
		m_Navi1 = L"찍힘";

	if (bSecond == false)
		m_Navi2 = L"안찍힘";
	else
		m_Navi2 = L"찍힘";


	SetDlgItemText(IDC_EDIT11, (CString)m_Navi1);
	SetDlgItemText(IDC_EDIT12, (CString)m_Navi2);
	SetDlgItemText(IDC_EDIT13, (CString)m_Navi3);
	UpdateData(FALSE);
}
