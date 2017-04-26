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

extern SOCKET sockConn, sockRrv;

extern CCriticalSection critical_rawdata;
extern vector<Mat> vec_depth;
extern vector<Mat> vec_left;
extern vector<Position> vec_position;
extern int rece_count;

extern volatile ProgressStatus progress_status;//��־���̵�����״̬��0����ͣ��1�ǽ���
volatile get_image_ret_code get_image_status = get_image_is_running; //��־��һGetImage�����Ƿ��Ѿ�����

string itos(double i)
{
	stringstream ss;

	ss << i;

	return ss.str();
}
//stringת����LPCWSTR����
LPCWSTR stringToLPCWSTR(std::string orig)
{
	size_t origsize = orig.length() + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t) *(orig.length() - 1));
	mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);
	return wcstring;
}
int RecvData(SOCKET sock, void *buf, int size);

void GetImageThread::GetImage(UINT wParam, LONG lParam)
{
	get_image_status = get_image_is_running;//ÿ�ε��ô˺����ȵ��ñ�־λ

	const int length = sizeof(MulDataStream);
	char data_stream[length]; //���մ����char *
	memset(data_stream, '0', length);
	MulDataStream data; //��data_stream����������ΪMulDataStream
	memset(&data, 0, length);

	//������ʱ����
	cv::Mat depth_image(HEIGHT, WIDTH, CV_16SC1);
	cv::Mat left_image(HEIGHT, WIDTH, CV_8U);
	Position position;

	while(1)
	{
		if ((progress_status == is_stopped) || (progress_status == complete))
		{
			closesocket(sockConn);
			closesocket(sockRrv);
			get_image_status = get_image_is_stopped;
			::PostMessage((HWND)(GetMainWnd()->GetSafeHwnd()), WM_UPDATE_STATUS, get_image_status, NULL);
			return;
		}

		memset(data_stream, 0, length);
		int ret = RecvData(sockConn, data_stream, length);
		if (ret < 0)
		{
			closesocket(sockConn);
			closesocket(sockRrv);
			get_image_status = receive_error;
			::PostMessage((HWND)(GetMainWnd()->GetSafeHwnd()), WM_UPDATE_STATUS, get_image_status, NULL);
			return;
		}
		if (!strncmp(data_stream, "send error!", 11))
		{
			closesocket(sockConn);
			closesocket(sockRrv);
			get_image_status = send_error;
			::PostMessage((HWND)(GetMainWnd()->GetSafeHwnd()), WM_UPDATE_STATUS, get_image_status, NULL);
			return;
		}
		std::memcpy(&data, data_stream, length);
		if (strncmp(data.head, "head", 5) || strncmp(data.tail, "tail", 5)) //�����β��У�����Ƿ���ȷ
		{
			get_image_status = data_error;
			::PostMessage((HWND)(GetMainWnd()->GetSafeHwnd()), WM_UPDATE_STATUS, get_image_status, NULL);
			continue;
		}
		rece_count++;
		std::memcpy(depth_image.data, data.depth, WIDTH*HEIGHT * 2);
		std::memcpy(left_image.data, data.left, WIDTH*HEIGHT);
		position.pitch = data.attitude.pitch;
		position.roll = data.attitude.roll;
		position.yaw = data.attitude.yaw;
		position.x = data.posi.gps_x;
		position.y = data.posi.gps_y;
		position.z = data.posi.gps_z;

		//����ȡ�����ݴ浽������
		critical_rawdata.Lock();
		vec_depth.push_back(depth_image.clone());
		vec_left.push_back(left_image.clone());
		vec_position.push_back(position);
		critical_rawdata.Unlock();
	}
}
//��ȡsize��Byte������
int RecvData(SOCKET sock, void *buf, int size)
{
	int sum = size;
	int err;
	int index = 0;
	while (size != 0)
	{
		err = recv(sock, (char*)buf + index, size, 0);
		if (err == SOCKET_ERROR) return -1;
		else if (err == 0) return -1;
		size -= err;
		index += err;
	}
	return sum - size;
}