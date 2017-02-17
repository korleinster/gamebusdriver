// ObjectTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "ObjectTool.h"
#include "afxdialogex.h"
#include "Component.h"
#include "ResourcesMgr.h"
#include "StaticObject.h"
#include "Info.h"
#include "MainFrm.h"
#include "MapToolView.h"


// CObjectTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CObjectTool, CDialog)

CObjectTool::CObjectTool(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_OBJECTTOOL, pParent)
	, m_bStart(true)
	, m_bFirstLoad(false)
	, m_fEditScaleX(0)
	, m_fEditScaleY(0)
	, m_fEditScaleZ(0)
	, m_fEditAngleX(0)
	, m_fEditAngleY(0)
	, m_fEditAngleZ(0)
	, m_fEditPosX(0)
	, m_fEditPosY(0)
	, m_fEditPosZ(0)
	, m_pCurObject(NULL)
	, m_stCurrentMeshKey(_T(""))
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
	DDX_Control(pDX, IDC_RADIO11, m_ObjTypeRadio[0]);
	DDX_Control(pDX, IDC_RADIO12, m_ObjTypeRadio[1]);
	if (m_bStart)
	{
		m_ObjModeRadio[0].SetCheck(TRUE);
		m_Decimal[0].SetCheck(TRUE);
		m_ObjTypeRadio[0].SetCheck(TRUE);
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
	DDX_Text(pDX, IDC_EDIT10, m_stCurrentMeshKey);


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
	ON_LBN_SELCHANGE(IDC_LIST1, &CObjectTool::OnStaticListSel)
	ON_LBN_SELCHANGE(IDC_LIST2, &CObjectTool::OnDynamicListSel)
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

	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);

	while (bTexWorking)
	{
		bTexWorking = texfinder.FindNextFileW();
		if (!texfinder.IsDirectory() && !texfinder.IsDots())
		{
			CString TexturePath = texfinder.GetFilePath();//경로
			CString TextureKey = texfinder.GetFileName();//키값

			TCHAR* tcFilePath = new TCHAR[128];
			wsprintf(tcFilePath, TexturePath);
			TCHAR* tcFileKey = new TCHAR[128];
			wsprintf(tcFileKey, TextureKey);

			if (m_bFirstLoad == false)
			{
				vecTexKey.push_back(tcFileKey);
				vecTexPath.push_back(tcFilePath);
			}
		}
	}

	if (m_bFirstLoad == false)
	{
		vector<TCHAR*>::iterator Pathiter = vecTexPath.begin();
		vector<TCHAR*>::iterator Keyiter = vecTexKey.begin();
		vector<TCHAR*>::iterator Pathiter_End = vecTexPath.end();
		vector<TCHAR*>::iterator Keyiter_End = vecTexKey.end();

		bool bEndofVector = false;

		while (!bEndofVector)
		{
			if (FAILED(CResourcesMgr::GetInstance()->AddTexture(
				RESOURCE_STAGE
				, (*Keyiter)
				, (*Pathiter))))
			{
				MessageBox(L"Texture Load Failed", *Keyiter);
			}

			TCHAR szPath[MAX_PATH] = L"";
			TCHAR szDirPath[MAX_PATH] = L"";
			GetCurrentDirectory(sizeof(szDirPath), szDirPath);

			PathRelativePathTo((LPWSTR)szPath,
				szDirPath, FILE_ATTRIBUTE_DIRECTORY,
				*Pathiter, FILE_ATTRIBUTE_NORMAL);

			SaveTex << (*Keyiter) << L"|";
			SaveTex << szPath << endl;


			++Keyiter;
			++Pathiter;

			if (Keyiter == Keyiter_End && Pathiter == Pathiter_End)
			{
				bEndofVector = true;
			}

		}

		bEndofVector = false;

		while (!bEndofVector)
		{
			vecTexKey.pop_back();
			vecTexPath.pop_back();
			if (vecTexKey.size() == 0 && vecTexPath.size() == 0)
			{
				bEndofVector = true;
				vecTexKey.clear();
				vecTexPath.clear();
				break;
			}

		}
	}


	while (bWorking)
	{
		bWorking = finder.FindNextFileW();
		if (finder.IsDirectory() && finder.IsDots())
			continue;
		else
		{
			CString FilePath = finder.GetFilePath();//경로
			PathRemoveFileSpec((LPWSTR)(LPCTSTR)FilePath);//파일 이름을 날린다
			CString FileKey = finder.GetFileName();//컴포넌트 키값
			PathRemoveExtension((LPWSTR)(LPCTSTR)FileKey);//확장자 날린다
			CString FileName = finder.GetFileName();//fbx파일


			TCHAR* tcFilePath = new TCHAR[128];
			wsprintf(tcFilePath, FilePath);
			_tcscat(tcFilePath, _T("\\"));
			TCHAR* tcFileKey = new TCHAR[128];
			wsprintf(tcFileKey, FileKey);
			TCHAR* tcFileName = new TCHAR[128];
			wsprintf(tcFileName, FileName);


			if (m_bFirstLoad == false)
			{
				vecFileKey.push_back(tcFileKey);
				vecFilename.push_back(tcFileName);
				vecFilePath.push_back(tcFilePath);
			}

			m_StaticList.AddString((LPWSTR)(LPCTSTR)FileKey);
		}

	}


	if (m_bFirstLoad == false)
	{
		vector<TCHAR*>::iterator Pathiter = vecFilePath.begin();
		vector<TCHAR*>::iterator Nameiter = vecFilename.begin();
		vector<TCHAR*>::iterator Keyiter = vecFileKey.begin();
		vector<TCHAR*>::iterator Pathiter_End = vecFilePath.end();
		vector<TCHAR*>::iterator Nameiter_End = vecFilename.end();
		vector<TCHAR*>::iterator Keyiter_End = vecFileKey.end();

		bool bEndofVector = false;

		while (!bEndofVector)
		{
			int len = 128;
			char cPath[128];
			char cName[128];

			WideCharToMultiByte(CP_ACP, 0, (*Pathiter), len, cPath, len, NULL, NULL);
			WideCharToMultiByte(CP_ACP, 0, (*Nameiter), len, cName, len, NULL, NULL);


			if (FAILED(CResourcesMgr::GetInstance()->AddMesh(
				RESOURCE_STAGE,
				MESH_STATIC
				, (*Keyiter)
				, "..\\Resource\\Mesh\\"
				, cName)))
			{
				MessageBox(L"Mesh Load Failed Or Aready Exist Mesh", *Nameiter);
			}

			TCHAR szPath[MAX_PATH] = L"";
			TCHAR szDirPath[MAX_PATH] = L"";
			GetCurrentDirectory(sizeof(szDirPath), szDirPath);

			PathRelativePathTo((LPWSTR)szPath,
				szDirPath, FILE_ATTRIBUTE_DIRECTORY,
				*Pathiter, FILE_ATTRIBUTE_NORMAL);

			SaveFile << (*Keyiter) << L"|";
			SaveFile << szPath << L"|";
			SaveFile << (*Nameiter) << endl;


			++Keyiter;
			++Pathiter;
			++Nameiter;

			if (Keyiter == Keyiter_End && Pathiter == Pathiter_End && Nameiter == Nameiter_End)
			{
				bEndofVector = true;
			}

		}


		bEndofVector = false;

		while (!bEndofVector)
		{
			vecFileKey.pop_back();
			vecFilename.pop_back();
			vecFilePath.pop_back();
			if (vecFileKey.size() == 0 && vecFilePath.size() == 0 && vecFilename.size() == 0)
			{
				bEndofVector = true;
				vecFileKey.clear();
				vecFilename.clear();
				vecFilePath.clear();
				break;
			}

		}
		m_bFirstLoad = true;
	}

	SaveTex.close();

	SaveFile.close();

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
	if (m_pCurObject)
	{
		CString stScaleX = _T("0.0000");

		if (pNMUpDown->iDelta < 0)
		{
			if (m_Decimal[0].GetCheck() == TRUE)
				++m_fEditScaleX;
			else if (m_Decimal[1].GetCheck() == TRUE)
				m_fEditScaleX += 0.1f;
			else if (m_Decimal[2].GetCheck() == TRUE)
				m_fEditScaleX += 0.01f;
			else if (m_Decimal[3].GetCheck() == TRUE)
				m_fEditScaleX += 0.001f;
			else if (m_Decimal[4].GetCheck() == TRUE)
				m_fEditScaleX += 0.0001f;
		}
		else
		{
			if (m_Decimal[0].GetCheck() == TRUE)
				--m_fEditScaleX;
			else if (m_Decimal[1].GetCheck() == TRUE)
				m_fEditScaleX -= 0.1f;
			else if (m_Decimal[2].GetCheck() == TRUE)
				m_fEditScaleX -= 0.01f;
			else if (m_Decimal[3].GetCheck() == TRUE)
				m_fEditScaleX -= 0.001f;
			else if (m_Decimal[4].GetCheck() == TRUE)
				m_fEditScaleX -= 0.0001f;
		}

		const CComponent* pComponent = m_pCurObject->GetComponent(L"Transform");
		((CInfo*)pComponent)->m_vScale.x = m_fEditScaleX;

		stScaleX.Format(_T("%.5f"), m_fEditScaleX);

		SetDlgItemText(IDC_EDIT1, stScaleX);


	}
	*pResult = 0;
}


void CObjectTool::OnScaleYControl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pCurObject)
	{
		CString stScaleY = _T("0.0000");

		if (pNMUpDown->iDelta < 0)
		{
			if (m_Decimal[0].GetCheck() == TRUE)
				++m_fEditScaleY;
			else if (m_Decimal[1].GetCheck() == TRUE)
				m_fEditScaleY += 0.1f;
			else if (m_Decimal[2].GetCheck() == TRUE)
				m_fEditScaleY += 0.01f;
			else if (m_Decimal[3].GetCheck() == TRUE)
				m_fEditScaleY += 0.001f;
			else if (m_Decimal[4].GetCheck() == TRUE)
				m_fEditScaleY += 0.0001f;
		}
		else
		{
			if (m_Decimal[0].GetCheck() == TRUE)
				--m_fEditScaleY;
			else if (m_Decimal[1].GetCheck() == TRUE)
				m_fEditScaleY -= 0.1f;
			else if (m_Decimal[2].GetCheck() == TRUE)
				m_fEditScaleY -= 0.01f;
			else if (m_Decimal[3].GetCheck() == TRUE)
				m_fEditScaleY -= 0.001f;
			else if (m_Decimal[4].GetCheck() == TRUE)
				m_fEditScaleY -= 0.0001f;
		}

		const CComponent* pComponent = m_pCurObject->GetComponent(L"Transform");
		((CInfo*)pComponent)->m_vScale.y = m_fEditScaleY;

		stScaleY.Format(_T("%.5f"), m_fEditScaleY);

		SetDlgItemText(IDC_EDIT2, stScaleY);


	}
	*pResult = 0;
}


void CObjectTool::OnScaleZControl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pCurObject)
	{
		CString stScaleZ = _T("0.0000");

		if (pNMUpDown->iDelta < 0)
		{
			if (m_Decimal[0].GetCheck() == TRUE)
				++m_fEditScaleZ;
			else if (m_Decimal[1].GetCheck() == TRUE)
				m_fEditScaleZ += 0.1f;
			else if (m_Decimal[2].GetCheck() == TRUE)
				m_fEditScaleZ += 0.01f;
			else if (m_Decimal[3].GetCheck() == TRUE)
				m_fEditScaleZ += 0.001f;
			else if (m_Decimal[4].GetCheck() == TRUE)
				m_fEditScaleZ += 0.0001f;
		}
		else
		{
			if (m_Decimal[0].GetCheck() == TRUE)
				--m_fEditScaleZ;
			else if (m_Decimal[1].GetCheck() == TRUE)
				m_fEditScaleZ -= 0.1f;
			else if (m_Decimal[2].GetCheck() == TRUE)
				m_fEditScaleZ -= 0.01f;
			else if (m_Decimal[3].GetCheck() == TRUE)
				m_fEditScaleZ -= 0.001f;
			else if (m_Decimal[4].GetCheck() == TRUE)
				m_fEditScaleZ -= 0.0001f;
		}

		const CComponent* pComponent = m_pCurObject->GetComponent(L"Transform");
		((CInfo*)pComponent)->m_vScale.z = m_fEditScaleZ;

		stScaleZ.Format(_T("%.5f"), m_fEditScaleZ);

		SetDlgItemText(IDC_EDIT3, stScaleZ);


	}
	*pResult = 0;
}


void CObjectTool::OnRotXControl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pCurObject)
	{
		if (pNMUpDown->iDelta < 0)
		{
			++m_fEditAngleX;

			if (m_fEditAngleX > 361)
				m_fEditAngleX = (int)m_fEditAngleX % 360;
			else if (m_fEditAngleX > 360)
				m_fEditAngleX = 1;
		}
		else
		{
			--m_fEditAngleX;
			if (m_fEditAngleX < 0)
				m_fEditAngleX = 359;
		}

		const CComponent* pComponent = m_pCurObject->GetComponent(L"Transform");

		((CInfo*)pComponent)->m_fAngle[ANGLE_X] = D3DXToRadian(m_fEditAngleX);

		SetDlgItemInt(IDC_EDIT4, (float)m_fEditAngleX);

	}
	*pResult = 0;
}


void CObjectTool::OnRotYControl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pCurObject)
	{
		if (pNMUpDown->iDelta < 0)
		{
			++m_fEditAngleY;

			if (m_fEditAngleY > 361)
				m_fEditAngleY = (int)m_fEditAngleY % 360;
			else if (m_fEditAngleY > 360)
				m_fEditAngleY = 1;
		}
		else
		{
			--m_fEditAngleY;
			if (m_fEditAngleY < 0)
				m_fEditAngleY = 359;
		}

		const CComponent* pComponent = m_pCurObject->GetComponent(L"Transform");

		((CInfo*)pComponent)->m_fAngle[ANGLE_Y] = D3DXToRadian(m_fEditAngleY);

		SetDlgItemInt(IDC_EDIT5, (float)m_fEditAngleY);

	}
	*pResult = 0;
}


void CObjectTool::OnRotZControl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pCurObject)
	{
		if (pNMUpDown->iDelta < 0)
		{
			++m_fEditAngleZ;

			if (m_fEditAngleZ > 361)
				m_fEditAngleZ = (int)m_fEditAngleZ % 360;
			else if (m_fEditAngleZ > 360)
				m_fEditAngleZ = 1;
		}
		else
		{
			--m_fEditAngleZ;
			if (m_fEditAngleZ < 0)
				m_fEditAngleZ = 359;
		}

		const CComponent* pComponent = m_pCurObject->GetComponent(L"Transform");

		((CInfo*)pComponent)->m_fAngle[ANGLE_Z] = D3DXToRadian(m_fEditAngleZ);

		SetDlgItemInt(IDC_EDIT6, (float)m_fEditAngleZ);

	}
	*pResult = 0;
}


void CObjectTool::OnPosXControl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pCurObject)
	{
		CString stPosX = _T("0.0000");

		if (pNMUpDown->iDelta < 0)
		{
			if (m_Decimal[0].GetCheck() == TRUE)
				++m_fEditPosX;
			else if (m_Decimal[1].GetCheck() == TRUE)
				m_fEditPosX += 0.1f;
			else if (m_Decimal[2].GetCheck() == TRUE)
				m_fEditPosX += 0.01f;
			else if (m_Decimal[3].GetCheck() == TRUE)
				m_fEditPosX += 0.001f;
			else if (m_Decimal[4].GetCheck() == TRUE)
				m_fEditPosX += 0.0001f;
		}
		else
		{
			if (m_Decimal[0].GetCheck() == TRUE)
				--m_fEditPosX;
			else if (m_Decimal[1].GetCheck() == TRUE)
				m_fEditPosX -= 0.1f;
			else if (m_Decimal[2].GetCheck() == TRUE)
				m_fEditPosX -= 0.01f;
			else if (m_Decimal[3].GetCheck() == TRUE)
				m_fEditPosX -= 0.001f;
			else if (m_Decimal[4].GetCheck() == TRUE)
				m_fEditPosX -= 0.0001f;
		}

		const CComponent* pComponent = m_pCurObject->GetComponent(L"Transform");
		((CInfo*)pComponent)->m_vPos.x = m_fEditPosX;

		stPosX.Format(_T("%.5f"), m_fEditPosX);

		SetDlgItemText(IDC_EDIT7, stPosX);


	}
	*pResult = 0;
}


void CObjectTool::OnPosYControl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pCurObject)
	{
		CString stPosY = _T("0.0000");

		if (pNMUpDown->iDelta < 0)
		{
			if (m_Decimal[0].GetCheck() == TRUE)
				++m_fEditPosY;
			else if (m_Decimal[1].GetCheck() == TRUE)
				m_fEditPosY += 0.1f;
			else if (m_Decimal[2].GetCheck() == TRUE)
				m_fEditPosY += 0.01f;
			else if (m_Decimal[3].GetCheck() == TRUE)
				m_fEditPosY += 0.001f;
			else if (m_Decimal[4].GetCheck() == TRUE)
				m_fEditPosY += 0.0001f;
		}
		else
		{
			if (m_Decimal[0].GetCheck() == TRUE)
				--m_fEditPosY;
			else if (m_Decimal[1].GetCheck() == TRUE)
				m_fEditPosY -= 0.1f;
			else if (m_Decimal[2].GetCheck() == TRUE)
				m_fEditPosY -= 0.01f;
			else if (m_Decimal[3].GetCheck() == TRUE)
				m_fEditPosY -= 0.001f;
			else if (m_Decimal[4].GetCheck() == TRUE)
				m_fEditPosY -= 0.0001f;
		}

		const CComponent* pComponent = m_pCurObject->GetComponent(L"Transform");
		((CInfo*)pComponent)->m_vPos.y = m_fEditPosY;

		stPosY.Format(_T("%.5f"), m_fEditPosY);

		SetDlgItemText(IDC_EDIT8, stPosY);


	}
	*pResult = 0;
}


void CObjectTool::OnPosZControl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pCurObject)
	{
		CString stPosZ = _T("0.0000");

		if (pNMUpDown->iDelta < 0)
		{
			if (m_Decimal[0].GetCheck() == TRUE)
				++m_fEditPosZ;
			else if (m_Decimal[1].GetCheck() == TRUE)
				m_fEditPosZ += 0.1f;
			else if (m_Decimal[2].GetCheck() == TRUE)
				m_fEditPosZ += 0.01f;
			else if (m_Decimal[3].GetCheck() == TRUE)
				m_fEditPosZ += 0.001f;
			else if (m_Decimal[4].GetCheck() == TRUE)
				m_fEditPosZ += 0.0001f;
		}
		else
		{
			if (m_Decimal[0].GetCheck() == TRUE)
				--m_fEditPosZ;
			else if (m_Decimal[1].GetCheck() == TRUE)
				m_fEditPosZ -= 0.1f;
			else if (m_Decimal[2].GetCheck() == TRUE)
				m_fEditPosZ -= 0.01f;
			else if (m_Decimal[3].GetCheck() == TRUE)
				m_fEditPosZ -= 0.001f;
			else if (m_Decimal[4].GetCheck() == TRUE)
				m_fEditPosZ -= 0.0001f;
		}

		const CComponent* pComponent = m_pCurObject->GetComponent(L"Transform");
		((CInfo*)pComponent)->m_vPos.z = m_fEditPosZ;

		stPosZ.Format(_T("%.5f"), m_fEditPosZ);

		SetDlgItemText(IDC_EDIT9, stPosZ);


	}
	*pResult = 0;
}


void CObjectTool::OnInfoEdit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (m_pCurObject)
	{
		const CComponent* pComponent = m_pCurObject->GetComponent(L"Transform");

		((CInfo*)pComponent)->m_vScale.x = m_fEditScaleX;
		((CInfo*)pComponent)->m_vScale.y = m_fEditScaleY;
		((CInfo*)pComponent)->m_vScale.z = m_fEditScaleZ;

		((CInfo*)pComponent)->m_fAngle[ANGLE_X] = D3DXToRadian(m_fEditAngleX);
		((CInfo*)pComponent)->m_fAngle[ANGLE_Y] = D3DXToRadian(m_fEditAngleY);
		((CInfo*)pComponent)->m_fAngle[ANGLE_Z] = D3DXToRadian(m_fEditAngleZ);

		((CInfo*)pComponent)->m_vPos.x = m_fEditPosX;
		((CInfo*)pComponent)->m_vPos.y = m_fEditPosY;
		((CInfo*)pComponent)->m_vPos.z = m_fEditPosZ;
	}

	UpdateData(FALSE);

	((CMainFrame*)AfxGetMainWnd())->m_pMainView->Invalidate(FALSE);
}


void CObjectTool::OnInfoReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (m_pCurObject)
	{
		const CComponent* pComponent = m_pCurObject->GetComponent(L"Transform");

		m_fEditScaleX = ((CInfo*)pComponent)->m_vScale.x;
		m_fEditScaleY = ((CInfo*)pComponent)->m_vScale.y;
		m_fEditScaleZ = ((CInfo*)pComponent)->m_vScale.z;

		m_fEditAngleX = ((CInfo*)pComponent)->m_fAngle[ANGLE_X];
		m_fEditAngleY = ((CInfo*)pComponent)->m_fAngle[ANGLE_Y];
		m_fEditAngleZ = ((CInfo*)pComponent)->m_fAngle[ANGLE_Z];
		m_fEditAngleX = D3DXToDegree(m_fEditAngleX);
		m_fEditAngleY = D3DXToDegree(m_fEditAngleY);
		m_fEditAngleZ = D3DXToDegree(m_fEditAngleZ);

		m_fEditPosX = ((CInfo*)pComponent)->m_vPos.x;
		m_fEditPosY = ((CInfo*)pComponent)->m_vPos.y;
		m_fEditPosZ = ((CInfo*)pComponent)->m_vPos.z;

		//m_PotentialCollision.SetCheck(((Engine::CTransform*)pComponent)->m_bCollision);
		//m_stCurrentMeshKey = ((Engine::CTransform*)pComponent)->m_tcKey;

	}
	else
	{
		m_fEditScaleX = 0.f;
		m_fEditScaleY = 0.f;
		m_fEditScaleZ = 0.f;

		m_fEditAngleX = 0.f;
		m_fEditAngleY = 0.f;
		m_fEditAngleZ = 0.f;

		m_fEditPosX = 0.f;
		m_fEditPosY = 0.f;
		m_fEditPosZ = 0.f;

		//m_PotentialCollision.SetCheck(FALSE);
	}

	UpdateData(FALSE);
}


void CObjectTool::OnStaticListSel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (LB_ERR != m_StaticList.GetCurSel())
	{
		if (m_ObjTypeRadio[0].GetCheck() == TRUE)
			m_StaticList.GetText(m_StaticList.GetCurSel(), m_strCurKey);
	}
}


void CObjectTool::OnDynamicListSel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (LB_ERR != m_DynamicList.GetCurSel())
	{
		if (m_ObjTypeRadio[1].GetCheck() == TRUE)
			m_DynamicList.GetText(m_DynamicList.GetCurSel(), m_strCurKey);
	}
}
