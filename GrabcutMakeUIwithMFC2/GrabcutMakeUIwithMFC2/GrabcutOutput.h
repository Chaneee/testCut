#pragma once


// GrabcutOutput ��ȭ �����Դϴ�.

class GrabcutOutput : public CDialog
{
	DECLARE_DYNAMIC(GrabcutOutput)

public:
	GrabcutOutput(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~GrabcutOutput();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_Output };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnStnClickedOutput();
	void DisplayOutput(int IDC_PICTURE_TARGET, Mat targetMat);
	void DisplayPasteGrabcut(int IDC_PICTURE_TARGET, Mat targetMatm, int mode);
	void Compose(Mat BGMat, Mat originMat, int target_X, int target_Y, int mode);
	//afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCallbackground();
	afx_msg void OnBnClickedPastegrab();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnStnClickedGrabimg();
	virtual BOOL OnInitDialog();
};
