
// GALGalmuriView.h : CGALGalmuriView Ŭ������ �������̽�
//

#pragma once


class CGALGalmuriDoc;
class CGALThumbCtrl;

class CGALGalmuriView : public CScrollView
{
protected: // serialization������ ��������ϴ�.
	CGALGalmuriView();
	DECLARE_DYNCREATE(CGALGalmuriView)

// Ư���Դϴ�.
public:
	const static int m_nColCount = 4;  // �� ����
	const static int m_nRowGap = 343;  // �� ���� ����
	const static int m_nColGap = 343;  // �� ���� ����
	const static int m_nMargin = 55;   // ����

	CGALGalmuriDoc* GetDocument() const;
	CObArray m_arrOriginThumb;
	CObArray m_arrOutputThumb;

// �۾��Դϴ�.
public:
	void GetThumbCtrlRect(CRect& rect, int index);
	CGALThumbCtrl* GetThumbCtrlByCtrlID(int id);
	CGALThumbCtrl* GetSelectedThumbCtrl();
	int GetSelectedThumbCtrlIndex();
	void SelectThumbCtrl(int index);
	void UpdateOutputThumbArray();
	int UpdateScroll();

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// �����Դϴ�.
public:
	virtual ~CGALGalmuriView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	virtual void OnInitialUpdate();
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

#ifndef _DEBUG  // GALGalmuriView.cpp�� ����� ����
inline CGALGalmuriDoc* CGALGalmuriView::GetDocument() const
   { return reinterpret_cast<CGALGalmuriDoc*>(m_pDocument); }
#endif

