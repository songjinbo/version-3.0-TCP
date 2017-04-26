
// DialogDlg.h : ͷ�ļ�
//

#pragma once

//#include "afxwin.h"
#include "GetImageThread.h"
#include "GetVoxelThread.h"
#include "PathPlanThread.h"
//#include "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\atlmfc\include\afxwin.h"
#include "Resource.h"


#define WM_DISPLAY_IMAGE WM_USER+2
#define WM_UPDATE_STATUS WM_USER+5

// CDialogDlg �Ի���
class CDialogDlg : public CDialogEx
{
// ����
public:
	CDialogDlg(CWnd* pParent = NULL);	// ��׼���캯��
// �Ի�������
	enum { IDD = IDD_DIALOG_DIALOG };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStart();
	double m_dx;
	double m_dy;
	double m_dz;
	double m_dyaw;
	double m_dpitch;
	double m_droll;
protected:
	//�����߳���Ҫ�õ�
	HANDLE hThread_get_image;
	DWORD ThreadID_get_image;
	void InitWindow(CStatic *, CStatic *);
	void InitVariable();
	bool BuildConnection(SOCKET &,SOCKET &);
	LRESULT DisplayImage(WPARAM wParam, LPARAM lParam);
	LRESULT UpdateStatus(WPARAM wParam, LPARAM lParam);

public:
	CStatic m_DisplayLeft;
	CStatic m_DisplayDepth;
//	afx_msg void OnBnClickedBrowse();
	void InitThread();
	GetImageThread* m_pget_image_thread;
	GetVoxelThread *m_pget_voxel_thread;
	PathPlanThread *m_ppath_plan_thread;
	double m_dendx;
	double m_dendy;
	double m_dendz;
	double m_dstartx;
	double m_dstarty;
	double m_dstartz;
private:
	CFont titleFont;
	CFont groupFont;
	CFont poseFont;
	CFont staticFont;
public:
	double m_dfinish_frames;
	double m_drunning_time;
	afx_msg void OnBnClickedStop();
	double m_drece_frames;
};

enum ProgressStatus
{
	//���̵߳�״̬��־λ
	is_ruuning = 1,
	is_stopped,
	complete,

	//GetImage������״̬��־λ
	get_image_is_running,
	get_image_is_stopped,
	get_image_complete,

	data_error,
	send_error,
	receive_error,
	//GetVoxel������״̬��־λ
	get_voxel_is_running,
	no_data_in_queue,
	get_one_voxel, //������һ֡�Ĵ���������
	get_voxel_is_stopped,
	get_all_voxel_complete,

	//PathPlan������״̬��־λ
	path_plan_is_running,
	path_plan_is_stopped,
	no_voxel_in_queue,
	subpath_accessible, //�ҵ�һ����·���ı�־
	path_accessible,
	no_path_accessible
};

typedef ProgressStatus get_image_ret_code;
typedef ProgressStatus get_voxel_ret_code;
typedef ProgressStatus path_plan_ret_code;

#define WIDTH 320
#define HEIGHT 240

struct gps_data
{
	float gps_x;
	float gps_y;
	float gps_z;
	int gps_status;
	int gps_time_stamp;
};
struct attitude_data
{
	int attitude_time_stamp;

	float pitch;
	float roll;
	float yaw;
};
struct MulDataStream
{
	char head[5];
	int count;

	int image_time_stamp;
	unsigned char left[WIDTH*HEIGHT];
	unsigned char right[WIDTH*HEIGHT];
	unsigned char depth[WIDTH*HEIGHT * 2];

	gps_data posi;
	attitude_data attitude;
	char tail[5];
};