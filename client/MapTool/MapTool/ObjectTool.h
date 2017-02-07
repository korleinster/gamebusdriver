#pragma once
#include "afxwin.h"


// CObjectTool 대화 상자입니다.

class CInfo;
class CObj;
class CObjectTool : public CDialog
{
	DECLARE_DYNAMIC(CObjectTool)

public:
	CObjectTool(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CObjectTool();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OBJECTTOOL };
#endif

public:
	bool m_bStart;
	CObj* m_pCurObject;
	CInfo* m_pBeforTrans;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CButton m_ObjModeRadio[5];
	CButton m_Decimal[5];
	CListBox m_StaticList;
	CListBox m_DynamicList;
	float m_fEditScaleX;
	float m_fEditScaleY;
	float m_fEditScaleZ;
	float m_fEditAngleX;
	float m_fEditAngleY;
	float m_fEditAngleZ;
	float m_fEditPosX;
	float m_fEditPosY;
	float m_fEditPosZ;
	afx_msg void OnStaticMeshLoad();
	afx_msg void OnDynamicMeshLoad();
	afx_msg void OnObjSave();
	afx_msg void OnObjLoad();
	afx_msg void OnScaleXControl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnScaleYControl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnScaleZControl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRotXControl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRotYControl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRotZControl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPosXControl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPosYControl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPosZControl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnInfoEdit();
	afx_msg void OnInfoReset();
};
