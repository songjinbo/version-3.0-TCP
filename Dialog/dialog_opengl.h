#pragma once
#include "Resource.h"
// dialog_opengl �Ի���

//#define WM_KEYCONTROL WM_USER+4

class dialog_opengl : public CDialog
{
	DECLARE_DYNAMIC(dialog_opengl)

public:
	dialog_opengl(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~dialog_opengl();

// �Ի�������
	enum { IDD = IDD_DIALOG_OPENGL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPaint();
//	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	bool bSetDCPixelFormat();

	//LRESULT KeyControl(UINT wParam, LONG lParam);
private:
	HGLRC m_hRC;
	CClientDC* m_pDC;
};
