#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CDlgTab2 대화 상자입니다.

class CDlgTab2 : public CDialog
{
	DECLARE_DYNAMIC(CDlgTab2)

public:
	CDlgTab2(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgTab2();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	void DisplayOutput(int IDC_PICTURE_TARGET, Mat targetMat);
	void DisplayPasteGrabcut(int IDC_PICTURE_TARGET, Mat targetMatm, int mode);
	void Compose(Mat BGMat, Mat originMat, int target_X, int target_Y, int mode);
	cv::Vec3d RGBtoYCbCr(cv::Vec3d bgr);
	cv::Vec3d YCbCrtoRGB(cv::Vec3d YCbCr);
	int kMeanss(int clusterMemberCount, int sum, int mode);
	void calcEnergy(const Mat& img, Mat& sobelMat);
	int seamcarving(int startPointX, int mode, int cutCount);
	void seamEnergyDown(int mode);
	CPoint RevisionPoint(CPoint point, CPoint pivot);

	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSlider3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedCallbgd();
	CButton test;
	CStatic testpic;
	
//	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CSliderCtrl widthSlide;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CSliderCtrl heightSlide;
	CSliderCtrl allSlide;
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedSeamdelete();
	afx_msg void OnBnClickedAutocolorbtn();
	afx_msg void OnBnClickedSeamprotect();
	afx_msg void OnBnClickedSavebutton();
};
