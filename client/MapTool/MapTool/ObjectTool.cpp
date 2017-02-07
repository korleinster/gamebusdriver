// ObjectTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "ObjectTool.h"
#include "afxdialogex.h"
#include "Component.h"


// CObjectTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CObjectTool, CDialog)

CObjectTool::CObjectTool(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_OBJECTTOOL, pParent)
	, m_bStart(true)
{

}

CObjectTool::~CObjectTool()
{
}

void CObjectTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO1, m_ObjModeRadio[0]);
	DDX_Control(pDX, IDC_RADIO2, m_ObjModeRadio[1]);
	DDX_Control(pDX, IDC_RADIO3, m_ObjModeRadio[2]);
	DDX_Control(pDX, IDC_RADIO4, m_ObjModeRadio[3]);
	DDX_Control(pDX, IDC_RADIO5, m_ObjModeRadio[4]);
	DDX_Control(pDX, IDC_RADIO6, m_Decimal[0]);
	DDX_Control(pDX, IDC_RADIO7, m_Decimal[1]);
	DDX_Control(pDX, IDC_RADIO8, m_Decimal[2]);
	DDX_Control(pDX, IDC_RADIO9, m_Decimal[3]);
	DDX_Control(pDX, IDC_RADIO10, m_Decimal[4]);
	if (m_bStart)
	{
		m_ObjModeRadio[0].SetCheck(TRUE);
		m_Decimal[0].SetCheck(TRUE);
		m_bStart = false;
	}

	DDX_Control(pDX, IDC_LIST1, m_StaticList);
	DDX_Control(pDX, IDC_LIST2, m_DynamicList);
	DDX_Text(pDX, IDC_EDIT1, m_fEditScaleX);
	DDX_Text(pDX, IDC_EDIT2, m_fEditScaleY);
	DDX_Text(pDX, IDC_EDIT3, m_fEditScaleZ);
	DDX_Text(pDX, IDC_EDIT4, m_fEditAngleX);
	DDX_Text(pDX, IDC_EDIT5, m_fEditAngleY);
	DDX_Text(pDX, IDC_EDIT6, m_fEditAngleZ);
	DDX_Text(pDX, IDC_EDIT7, m_fEditPosX);
	DDX_Text(pDX, IDC_EDIT8, m_fEditPosY);
	DDX_Text(pDX, IDC_EDIT9, m_fEditPosZ);
}


BEGIN_MESSAGE_MAP(CObjectTool, CDialog)
	ON_BN_CLICKED(IDC_BUTTON3, &CObjectTool::OnStaticMeshLoad)
	ON_BN_CLICKED(IDC_BUTTON4, &CObjectTool::OnDynamicMeshLoad)
	ON_BN_CLICKED(IDC_BUTTON1, &CObjectTool::OnObjSave)
	ON_BN_CLICKED(IDC_BUTTON2, &CObjectTool::OnObjLoad)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, &CObjectTool::OnScaleXControl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN2, &CObjectTool::OnScaleYControl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN3, &CObjectTool::OnScaleZControl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN4, &CObjectTool::OnRotXControl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN5, &CObjectTool::OnRotYControl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN6, &CObjectTool::OnRotZControl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN7, &CObjectTool::OnPosXControl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN8, &CObjectTool::OnPosYControl)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN9, &CObjectTool::OnPosZControl)
	ON_BN_CLICKED(IDC_BUTTON5, &CObjectTool::OnInfoEdit)
	ON_BN_CLICKED(IDC_BUTTON6, &CObjectTool::OnInfoReset)
END_MESSAGE_MAP()


// CObjectTool 메시지 처리기입니다.


void CObjectTool::OnStaticMeshLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	HRESULT hr = NULL;
	CFileFind finder, texfinder;
	BOOL bWorking = false;
	BOOL bTexWorking = false;

	wofstream SaveFile, SaveTex;

	bWorking = finder.FindFile(_T("..//Resource/Mesh/*.*"));
	bTexWorking = texfinder.FindFile(_T("..//Resource/MeshImage/*.png"));
	SaveFile.open(L"..//Resource/MeshPath.txt", ios::out);
	SaveTex.open(L"..//Resource/MeshTexPath.txt", ios::out);


}


void CObjectTool::OnDynamicMeshLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CObjectTool::OnObjSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CObjectTool::OnObjLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CObjectTool::OnScaleXControl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CObjectTool::OnScaleYControl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CObjectTool::OnScaleZControl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CObjectTool::OnRotXControl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CObjectTool::OnRotYControl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CObjectTool::OnRotZControl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CObjectTool::OnPosXControl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CObjectTool::OnPosYControl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CObjectTool::OnPosZControl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CObjectTool::OnInfoEdit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CObjectTool::OnInfoReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//if (m_pCurObject)
	//{
	//	const CComponent* pComponent = m_pCurObject->GetComponent(L"Transform");

	//	m_fEditScaleX = ((CInfo*)pComponent)->m_vScale.x;
	//	m_fEditScaleY = ((CInfo*)pComponent)->m_vScale.y;
	//	m_fEditScaleZ = ((CInfo*)pComponent)->m_vScale.z;

	//	m_fEditAngleX = ((CInfo*)pComponent)->m_fAngle[ANGLE_X];
	//	m_fEditAngleY = ((CInfo*)pComponent)->m_fAngle[ANGLE_Y];
	//	m_fEditAngleZ = ((CInfo*)pComponent)->m_fAngle[ANGLE_Z];
	//	m_fEditAngleX = D3DXToDegree(m_fEditAngleX);
	//	m_fEditAngleY = D3DXToDegree(m_fEditAngleY);
	//	m_fEditAngleZ = D3DXToDegree(m_fEditAngleZ);

	//	fPosX = ((Engine::CTransform*)pComponent)->m_vPos.x;
	//	fPosY = ((Engine::CTransform*)pComponent)->m_vPos.y;
	//	fPosZ = ((Engine::CTransform*)pComponent)->m_vPos.z;

	//	m_PotentialCollision.SetCheck(((Engine::CTransform*)pComponent)->m_bCollision);

	//	//m_stCurrentMeshKey = ((Engine::CTransform*)pComponent)->m_tcKey;

	//}
	//else
	//{
	//	m_fScaleX = 0.f;
	//	m_fScaleY = 0.f;
	//	m_fScaleZ = 0.f;

	//	m_fRotX = 0.f;
	//	m_fRotY = 0.f;
	//	m_fRotZ = 0.f;

	//	m_fTransX = 0.f;
	//	m_fTransY = 0.f;
	//	m_fTransZ = 0.f;

	//	m_PotentialCollision.SetCheck(FALSE);



	//}

	//UpdateData(FALSE);
}
