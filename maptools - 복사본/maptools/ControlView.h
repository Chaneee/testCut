#pragma once
#include "afxcmn.h"

#include "DlgTab1.h"
#include "DlgTab2.h"

// CControlView �� ���Դϴ�.

class CControlView : public CFormView
{
	DECLARE_DYNCREATE(CControlView)

protected:
	CControlView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CControlView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONTROLVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_cltTab;
	//afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	CDlgTab1 *m_pDialog1;
	CDlgTab2 *m_pDialog2;
	virtual void OnInitialUpdate();
	virtual void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
};


