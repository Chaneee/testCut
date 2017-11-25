// ControlView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "maptools.h"
#include "ControlView.h"

#define SAFE_DELETE(p)  { if(p){delete p; p=NULL;}}
// CControlView

IMPLEMENT_DYNCREATE(CControlView, CFormView)

CControlView::CControlView()
	: CFormView(IDD_CONTROLVIEW)
{
	m_pDialog1 = NULL;
	m_pDialog2 = NULL;
}

CControlView::~CControlView()
{
	SAFE_DELETE(m_pDialog1);
	SAFE_DELETE(m_pDialog2);
}

void CControlView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_cltTab);
}

BEGIN_MESSAGE_MAP(CControlView, CFormView)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CControlView::OnTcnSelchangeTab1)
END_MESSAGE_MAP()


// CControlView 진단입니다.

#ifdef _DEBUG
void CControlView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CControlView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CControlView 메시지 처리기입니다.


void CControlView::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int select = m_cltTab.GetCurSel();

	if (m_pDialog1->ShowWindow(SW_SHOW))
	{
		m_pDialog1->ShowWindow(SW_HIDE);
		m_pDialog2->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pDialog1->ShowWindow(SW_SHOW);
		m_pDialog2->ShowWindow(SW_HIDE);
		
	}


	*pResult = 0;
}


void CControlView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	m_cltTab.DeleteAllItems();
	CString strOne = _T("분할");
	CString strTwo = _T("합성");
	m_cltTab.InsertItem(0, strOne);
	m_cltTab.InsertItem(1, strTwo);

	CRect rect;

	m_pDialog1 = new CDlgTab1;
	m_pDialog1->Create(IDD_DIALOG1, &m_cltTab);
	m_pDialog1->GetWindowRect(&rect);
	m_pDialog1->MoveWindow(25, 50, rect.Width(), rect.Height());
	m_pDialog1->ShowWindow(SW_SHOW);
	
	m_pDialog2 = new CDlgTab2;
	m_pDialog2->Create(IDD_DIALOG2, &m_cltTab);
	m_pDialog2->GetWindowRect(&rect);
	m_pDialog2->MoveWindow(25, 50, rect.Width(), rect.Height());
	m_pDialog2->ShowWindow(SW_HIDE);

	UpdateData(FALSE);
}
