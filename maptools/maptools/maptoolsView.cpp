
// maptoolsView.cpp : CmaptoolsView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "maptools.h"
#endif

#include "maptoolsDoc.h"
#include "maptoolsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CmaptoolsView

IMPLEMENT_DYNCREATE(CmaptoolsView, CView)

BEGIN_MESSAGE_MAP(CmaptoolsView, CView)
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CmaptoolsView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CmaptoolsView ����/�Ҹ�

CmaptoolsView::CmaptoolsView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CmaptoolsView::~CmaptoolsView()
{
}

BOOL CmaptoolsView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CmaptoolsView �׸���

void CmaptoolsView::OnDraw(CDC* /*pDC*/)
{
	CmaptoolsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}


// CmaptoolsView �μ�


void CmaptoolsView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CmaptoolsView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CmaptoolsView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CmaptoolsView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}

void CmaptoolsView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CmaptoolsView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CmaptoolsView ����

#ifdef _DEBUG
void CmaptoolsView::AssertValid() const
{
	CView::AssertValid();
}

void CmaptoolsView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CmaptoolsDoc* CmaptoolsView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CmaptoolsDoc)));
	return (CmaptoolsDoc*)m_pDocument;
}
#endif //_DEBUG


// CmaptoolsView �޽��� ó����
