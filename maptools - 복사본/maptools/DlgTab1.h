#pragma once


// CDlgTab1 ��ȭ �����Դϴ�.
#include "afxwin.h"
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

class CDlgTab1 : public CDialog
{
	DECLARE_DYNAMIC(CDlgTab1)

public:
	CDlgTab1(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgTab1();
	void DisplayImage(int IDC_PICTURE_TARGET, Mat targetMat);
	void DisplayHowTo(int IDC_PICTURE_TARGET, Mat targetMat);
	//���콺 ���� ��ġ, ������ġ
	CPoint nowMousePos, oldMousePoint;
	Mat doGrabcut(Mat targetMat, int mode);

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedMfcbutton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedCommand1();
//	afx_msg void OnStnClickedPic();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnClickedBack();
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	bool m_bWaitCursor;
};
