
// maptools.h : maptools ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;


// CmaptoolsApp:
// �� Ŭ������ ������ ���ؼ��� maptools.cpp�� �����Ͻʽÿ�.
//

class CmaptoolsApp : public CWinAppEx
{
public:
	CmaptoolsApp();



// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// �����Դϴ�.
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CmaptoolsApp theApp;
