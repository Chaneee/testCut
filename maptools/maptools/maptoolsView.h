
// maptoolsView.h : CmaptoolsView Ŭ������ �������̽�
//

#pragma once


class CmaptoolsView : public CView
{
protected: // serialization������ ��������ϴ�.
	CmaptoolsView();
	DECLARE_DYNCREATE(CmaptoolsView)

// Ư���Դϴ�.
public:
	CmaptoolsDoc* GetDocument() const;

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// �����Դϴ�.
public:
	virtual ~CmaptoolsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // maptoolsView.cpp�� ����� ����
inline CmaptoolsDoc* CmaptoolsView::GetDocument() const
   { return reinterpret_cast<CmaptoolsDoc*>(m_pDocument); }
#endif
