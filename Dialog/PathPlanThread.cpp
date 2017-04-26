// PathPlanThread.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Dialog.h"
#include "PathPlanThread.h"
#include "./PathPlan.src/Star.h"

// PathPlanThread
IMPLEMENT_DYNCREATE(PathPlanThread, CWinThread)



PathPlanThread::PathPlanThread()
{
}

PathPlanThread::~PathPlanThread()
{
}

BOOL PathPlanThread::InitInstance()
{
	// TODO:    �ڴ�ִ���������̳߳�ʼ��
	return TRUE;
}

int PathPlanThread::ExitInstance()
{
	// TODO:    �ڴ�ִ���������߳�����
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(PathPlanThread, CWinThread)
	ON_THREAD_MESSAGE(WM_PATHPLAN_BEGIN, PathPlan)
END_MESSAGE_MAP()

volatile path_plan_ret_code path_plan_status = path_plan_is_running;

void PathPlanThread::PathPlan(UINT wParam, LONG lParam)
{
	path_plan_status = path_plan_is_running;

	Star star;
	star.findpath();
	//getchar();

	if (path_plan_status == subpath_accessible)
	{
		::PostMessage((HWND)(GetMainWnd()->GetSafeHwnd()), WM_DISPLAY_IMAGE, path_plan_status, NULL);
	}
	else
	{
		::PostMessage((HWND)(GetMainWnd()->GetSafeHwnd()), WM_UPDATE_STATUS, path_plan_status, NULL);
	}
	
}
// PathPlanThread ��Ϣ�������
