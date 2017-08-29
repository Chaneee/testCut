
// GrabcutMakeUIwithMFC2Dlg.h : ��� ����
//

#pragma once
#include "afxwin.h"
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

// CGrabcutMakeUIwithMFC2Dlg ��ȭ ����
class CGrabcutMakeUIwithMFC2Dlg : public CDialogEx
{
// �����Դϴ�.
public:
	CGrabcutMakeUIwithMFC2Dlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.
	
// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GRABCUTMAKEUIWITHMFC2_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.
public:
	void DisplayImage(int IDC_PICTURE_TARGET, Mat targetMat);
	//���콺 ���� ��ġ, ������ġ
	CPoint nowMousePos, oldMousePoint;

// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	CButton imgOpen;
	afx_msg void OnBnClickedImageopen();
	CStatic mstDisplay;

	afx_msg void OnBnClickedOutputbtn();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
