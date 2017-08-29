
// GrabcutMakeUIwithMFC2Dlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

// CGrabcutMakeUIwithMFC2Dlg 대화 상자
class CGrabcutMakeUIwithMFC2Dlg : public CDialogEx
{
// 생성입니다.
public:
	CGrabcutMakeUIwithMFC2Dlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GRABCUTMAKEUIWITHMFC2_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.
public:
	void DisplayImage(int IDC_PICTURE_TARGET, Mat targetMat);
	//마우스 현재 위치, 이전위치
	CPoint nowMousePos, oldMousePoint;

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
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
