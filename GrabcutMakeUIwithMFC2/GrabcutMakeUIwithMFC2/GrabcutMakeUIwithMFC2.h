
// GrabcutMakeUIwithMFC2.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.

// CGrabcutMakeUIwithMFC2App:
// �� Ŭ������ ������ ���ؼ��� GrabcutMakeUIwithMFC2.cpp�� �����Ͻʽÿ�.
//

class CGrabcutMakeUIwithMFC2App : public CWinApp
{
public:
	CGrabcutMakeUIwithMFC2App();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CGrabcutMakeUIwithMFC2App theApp;
