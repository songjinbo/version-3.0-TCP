// GetImageThread.cpp : ʵ���ļ�
//
#include "stdafx.h"
#include <string>
#include <iostream>
#include <fstream>
#include <queue>
#include <tchar.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include "Dialog.h"
#include "DialogDlg.h"
#include "GetImageThread.h"
#include "GetVoxelThread.h"


IMPLEMENT_DYNCREATE(GetImageThread, CWinThread)

GetImageThread::GetImageThread()
{
}

GetImageThread::~GetImageThread()
{
}

BOOL GetImageThread::InitInstance()
{
	// TODO:    �ڴ�ִ���������̳߳�ʼ��
	return TRUE;
}

int GetImageThread::ExitInstance()
{
	// TODO:    �ڴ�ִ���������߳�����
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(GetImageThread, CWinThread)
	ON_THREAD_MESSAGE(WM_GETIMAGE_BEGIN, GetImage)
END_MESSAGE_MAP()


// GetImageThread ��Ϣ�������

using namespace cv;
using namespace std;

//extern GetVoxelThread *m_pget_voxel_thread;
extern char path[MAX_PATH];
extern CCriticalSection critical_rawdata;
extern vector<Mat> vec_depth;
extern vector<Mat> vec_left;
extern vector<Position> vec_position;

extern volatile ProgressStatus progress_status;//��־���̵�����״̬��0����ͣ��1�ǽ���

volatile get_image_ret_code get_image_status = get_image_is_running; //��־��һGetImage�����Ƿ��Ѿ�����

string itos(double i)
{
	stringstream ss;

	ss << i;

	return ss.str();
}

void GetImageThread::GetImage(UINT wParam, LONG lParam)
{
	//��ȡ�ɼ���֡��
	get_image_status = get_image_is_running;//ÿ�ε��ô˺����ȵ��ñ�־λ

	char str_file_count[80];
	string command = "cmd.exe /c dir " + string(path) + "| findstr \"depth[0-9]*\.xml\" | find /v /c \"\">>" + string(path) + "file_count.txt";
	WinExec(command.data(), SW_HIDE);
	fstream file_read(string(path) + "file_count.txt", ios::in);
	file_read.getline(str_file_count, 80);
	int file_count = atoi(str_file_count);
	file_read.close();
	fstream file_clear(string(path) + "file_count.txt", ios::out); //������������
	file_clear.close();

	if (!file_count) //û���ļ�
	{
		get_image_status = no_file;
		::PostMessage((HWND)(GetMainWnd()->GetSafeHwnd()), WM_DISPLAY_IMAGE, get_image_status, NULL);
		return; 
	}

	int count = 0; //���������Ѿ��򿪵��ļ�����
	FileStorage fs1_depth;
	FileStorage fs1_left;
	cv::Mat depth_image;
	cv::Mat left_image;
	Position position;
	bool is_depth_catch = 0,is_left_catch=0;
	//�п���������ѭ�����ؼ�����file_count�ļ����Ƿ�׼ȷ,���file_count����ʵ�ʵ�֡������ô�ͻ�������ѭ��
	int no;
	for (no = 1; count<file_count; no++)
	{
		if (progress_status==is_stopped) //ѭ������־λprogress_status����һ������Ҫ�У�postquitmessage()�޷������������forѭ��
		{
			get_image_status = get_image_is_stopped;
			::PostMessage((HWND)(GetMainWnd()->GetSafeHwnd()), WM_DISPLAY_IMAGE, get_image_status, NULL);
			return;
		}
		//���ж��ļ��ĸ�ʽ�Ƿ���ȷ
		is_depth_catch = is_left_catch = 0;
		try
		{	fs1_depth.open(string(path) + "depth" + itos(no) + ".xml", FileStorage::READ);	}
		catch (std::exception const& e)
		{	continue;	}

		try
		{fs1_left.open(string(path) + "left" + itos(no) + ".xml", FileStorage::READ);}
		catch (std::exception &const e)
		{	continue;	}

		if (!fs1_depth.isOpened() && !fs1_left.isOpened()) //ûleft��depth
		{
			fs1_left.release(); //��Ҫ���ǹرգ�����������
			fs1_depth.release(); //��Ҫ���ǹرգ�����������
			critical_rawdata.Unlock();
			continue;
		}
		else if (fs1_depth.isOpened() && fs1_left.isOpened())//��left��depth
		{
			count++;
			fs1_left["image"]["left"] >> left_image;
			fs1_depth["image"]["depth"] >> depth_image;
			fs1_depth["gps"]["gps_x"] >> position.x; fs1_depth["gps"]["gps_y"] >> position.y; fs1_depth["gps"]["gps_z"] >> position.z;
			fs1_depth["attitude"]["yaw"] >> position.yaw; fs1_depth["attitude"]["roll"] >> position.roll; fs1_depth["attitude"]["pitch"] >> position.pitch;
		}
		else if (fs1_depth.isOpened() && !fs1_left.isOpened())//��depthûleft
		{
			count++;
			left_image = Mat(Scalar(0));
			fs1_depth["image"]["depth"] >> depth_image;
			fs1_depth["gps"]["gps_x"] >> position.x; fs1_depth["gps"]["gps_y"] >> position.y; fs1_depth["gps"]["gps_z"] >> position.z;
			fs1_depth["attitude"]["yaw"] >> position.yaw; fs1_depth["attitude"]["roll"] >> position.roll; fs1_depth["attitude"]["pitch"] >> position.pitch;
		}
		else//��leftûdepth
		{
			fs1_left["image"]["left"] >> left_image;
			depth_image = Mat(Scalar(0));
			fs1_left["gps"]["gps_x"] >> position.x; fs1_left["gps"]["gps_y"] >> position.y; fs1_left["gps"]["gps_z"] >> position.z;
			fs1_left["attitude"]["yaw"] >> position.yaw; fs1_left["attitude"]["roll"] >> position.roll; fs1_left["attitude"]["pitch"] >> position.pitch;
		}
		//����ȡ�����ݴ浽������
		critical_rawdata.Lock();
		vec_depth.push_back(depth_image.clone());
		vec_left.push_back(left_image.clone());
		vec_position.push_back(position);
		critical_rawdata.Unlock();
		
		fs1_depth.release();
		fs1_left.release();
	}
	get_image_status = get_image_complete;
	::PostMessage((HWND)(GetMainWnd()->GetSafeHwnd()), WM_DISPLAY_IMAGE, get_image_status, NULL);
	
	return;//����������ֵ
}