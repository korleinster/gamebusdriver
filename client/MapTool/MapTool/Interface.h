#pragma once


// CInterface 대화 상자입니다.

class CInterface : public CDialog
{
	DECLARE_DYNAMIC(CInterface)

public:
	CInterface(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CInterface();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INTERFACE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	float m_fCamPosX;
	float m_fCamPosY;
	float m_fCamPosZ;
	float m_CamAtX;
	float m_CamAtY;
	float m_CamAtZ;
	CString m_CamState;
	int m_iCpu;
	int m_iFps;
	int m_iRamState;
	int m_iRamMaximum;
	int m_iCamSpeed;
	CString m_Navi1;
	CString m_Navi2;
	CString m_Navi3;
public:
	void CamUpdate(D3DXVECTOR3 vCamPos, D3DXVECTOR3 vCamAt, bool bCamState, int iCamSpeed);
	void HardwareUpdate(int iCpu, int iRam, int iRamMax);
	void FpsUpdate(int iFps);
	void NaviUpdate(bool bFirst, bool bSecond);
};
