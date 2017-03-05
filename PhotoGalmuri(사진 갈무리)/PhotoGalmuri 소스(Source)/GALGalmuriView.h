
// GALGalmuriView.h : CGALGalmuriView 클래스의 인터페이스
//

#pragma once


class CGALGalmuriDoc;
class CGALThumbCtrl;

class CGALGalmuriView : public CScrollView
{
protected: // serialization에서만 만들어집니다.
	CGALGalmuriView();
	DECLARE_DYNCREATE(CGALGalmuriView)

// 특성입니다.
public:
	const static int m_nColCount = 4;  // 열 개수
	const static int m_nRowGap = 343;  // 행 사이 간격
	const static int m_nColGap = 343;  // 열 사이 간격
	const static int m_nMargin = 55;   // 여백

	CGALGalmuriDoc* GetDocument() const;
	CObArray m_arrOriginThumb;
	CObArray m_arrOutputThumb;

// 작업입니다.
public:
	void GetThumbCtrlRect(CRect& rect, int index);
	CGALThumbCtrl* GetThumbCtrlByCtrlID(int id);
	CGALThumbCtrl* GetSelectedThumbCtrl();
	int GetSelectedThumbCtrlIndex();
	void SelectThumbCtrl(int index);
	void UpdateOutputThumbArray();
	int UpdateScroll();

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 구현입니다.
public:
	virtual ~CGALGalmuriView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	virtual void OnInitialUpdate();
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

#ifndef _DEBUG  // GALGalmuriView.cpp의 디버그 버전
inline CGALGalmuriDoc* CGALGalmuriView::GetDocument() const
   { return reinterpret_cast<CGALGalmuriDoc*>(m_pDocument); }
#endif

