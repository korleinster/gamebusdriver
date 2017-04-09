// NaviTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "NaviTool.h"
#include "afxdialogex.h"
#include "NaviCell.h"


// CNaviTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CNaviTool, CDialog)

CNaviTool::CNaviTool(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_NAVITOOL, pParent)
	, m_iNaviCnt(0)
	, m_fPosX(0)
	, m_fPosY(0)
	, m_fPosZ(0)
	, m_fNearRange(0)
	, m_PointAx(0)
	, m_PointBx(0)
	, m_PointCx(0)
	, m_PointAy(0)
	, m_PointBy(0)
	, m_PointCy(0)
	, m_PointAz(0)
	, m_PointBz(0)
	, m_PointCz(0)
	, m_bStart(true)
{

}

CNaviTool::~CNaviTool()
{
}

void CNaviTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK1, m_WireFrame);
	DDX_Control(pDX, IDC_CHECK2, m_ObjectWire);

	DDX_Control(pDX, IDC_RADIO1, m_NaviMode[0]);
	DDX_Control(pDX, IDC_RADIO2, m_NaviMode[1]);
	DDX_Control(pDX, IDC_RADIO3, m_NaviMode[2]);
	DDX_Control(pDX, IDC_RADIO4, m_NaviMode[3]);

	DDX_Control(pDX, IDC_RADIO7, m_CreateMode[0]);
	DDX_Control(pDX, IDC_RADIO8, m_CreateMode[1]);

	DDX_Control(pDX, IDC_RADIO5, m_PickingMode[0]);
	DDX_Control(pDX, IDC_RADIO6, m_PickingMode[1]);

	DDX_Control(pDX, IDC_RADIO9, m_CellType[0]);
	DDX_Control(pDX, IDC_RADIO10, m_CellType[0]);

	DDX_Control(pDX, IDC_RADIO11, m_DeleteMode[0]);
	DDX_Control(pDX, IDC_RADIO12, m_DeleteMode[1]);

	DDX_Control(pDX, IDC_RADIO13, m_PosDecimal[0]);
	DDX_Control(pDX, IDC_RADIO14, m_PosDecimal[1]);
	DDX_Control(pDX, IDC_RADIO15, m_PosDecimal[2]);
	DDX_Control(pDX, IDC_RADIO16, m_PosDecimal[3]);
	DDX_Control(pDX, IDC_RADIO17, m_PosDecimal[4]);
	DDX_Control(pDX, IDC_CHECK3, m_NaviView);

	DDX_Text(pDX, IDC_EDIT2, m_fPosX);
	DDX_Text(pDX, IDC_EDIT3, m_fPosY);
	DDX_Text(pDX, IDC_EDIT4, m_fPosZ);

	DDX_Text(pDX, IDC_EDIT7, m_PointAx);
	DDX_Text(pDX, IDC_EDIT8, m_PointAy);
	DDX_Text(pDX, IDC_EDIT9, m_PointAz);

	DDX_Text(pDX, IDC_EDIT10, m_PointBx);
	DDX_Text(pDX, IDC_EDIT11, m_PointBy);
	DDX_Text(pDX, IDC_EDIT12, m_PointBz);

	DDX_Text(pDX, IDC_EDIT13, m_PointCx);
	DDX_Text(pDX, IDC_EDIT14, m_PointCy);
	DDX_Text(pDX, IDC_EDIT15, m_PointCz);
	DDX_Text(pDX, IDC_EDIT6, m_fNearRange);

	if (m_bStart == true)
	{
		m_NaviMode[0].SetCheck(TRUE);
		m_CreateMode[0].SetCheck(TRUE);
		m_DeleteMode[0].SetCheck(TRUE);
		m_PickingMode[0].SetCheck(TRUE);
		m_CellType[0].SetCheck(TRUE);
		m_PosDecimal[0].SetCheck(TRUE);
		m_NaviView.SetCheck(TRUE);

		m_bStart = false;
	}
	
}


BEGIN_MESSAGE_MAP(CNaviTool, CDialog)
	ON_BN_CLICKED(IDC_BUTTON2, &CNaviTool::OnNaviSave)
	ON_BN_CLICKED(IDC_BUTTON3, &CNaviTool::OnNaviLoad)
END_MESSAGE_MAP()


// CNaviTool 메시지 처리기입니다.

void CNaviTool::SetPickPos(D3DXVECTOR3 vIndex)
{
	UpdateData(TRUE);

	m_fPosX = vIndex.x;
	m_fPosY = vIndex.y;
	m_fPosZ = vIndex.z;

	UpdateData(FALSE);
}


void CNaviTool::SetPickCell(CNaviCell* pCell)
{
	UpdateData(TRUE);

	if (pCell != NULL)
	{
		m_pCurCell = pCell;

		m_PointAx = pCell->GetPoint(CNaviCell::POINT_A)->x;
		m_PointAy = pCell->GetPoint(CNaviCell::POINT_A)->y;
		m_PointAz = pCell->GetPoint(CNaviCell::POINT_A)->z;

		m_PointBx = pCell->GetPoint(CNaviCell::POINT_B)->x;
		m_PointBy = pCell->GetPoint(CNaviCell::POINT_B)->y;
		m_PointBz = pCell->GetPoint(CNaviCell::POINT_B)->z;

		m_PointCx = pCell->GetPoint(CNaviCell::POINT_C)->x;
		m_PointCy = pCell->GetPoint(CNaviCell::POINT_C)->y;
		m_PointCz = pCell->GetPoint(CNaviCell::POINT_C)->z;
	}
	else
	{
		m_pCurCell = NULL;

		m_PointAx = 0.f;
		m_PointAy = 0.f;
		m_PointAz = 0.f;

		m_PointBx = 0.f;
		m_PointBy = 0.f;
		m_PointBz = 0.f;

		m_PointCx = 0.f;
		m_PointCy = 0.f;
		m_PointCz = 0.f;
	}

	UpdateData(FALSE);

}

void CNaviTool::OnNaviSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CNaviTool::OnNaviLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
