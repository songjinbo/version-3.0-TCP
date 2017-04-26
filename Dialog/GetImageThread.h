#pragma once


//#include "afxmt.h"
//#include <vector>
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <iostream>
//#include "GetVoxelThread.h"


#define WM_GETIMAGE_BEGIN WM_USER+1


// GetImageThread

class GetImageThread : public CWinThread
{
	DECLARE_DYNCREATE(GetImageThread)

public:
	GetImageThread();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~GetImageThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void GetImage(UINT wParam, LONG lParam);
};


//�洢λ�ú���̬��Ϣ
class Position
{
public:
	double x;
	double y;
	double z;
	double roll;
	double pitch;
	double yaw;
};
