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
};
