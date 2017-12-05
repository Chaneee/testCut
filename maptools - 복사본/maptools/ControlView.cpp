// ControlView.cpp : ���� �����Դϴ�.
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


// CControlView �����Դϴ�.

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


// CControlView �޽��� ó�����Դϴ�.


void CControlView::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	m_cltTab.DeleteAllItems();
	CString strOne = _T("����");
	CString strTwo = _T("�ռ�");
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
