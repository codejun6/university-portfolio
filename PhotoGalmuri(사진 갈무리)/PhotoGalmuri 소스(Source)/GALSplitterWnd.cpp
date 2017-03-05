// GALSplitterWnd.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PhotoGalmuri.h"
#include "GALSplitterWnd.h"


// CGALSplitterWnd

IMPLEMENT_DYNAMIC(CGALSplitterWnd, CSplitterWnd)

CGALSplitterWnd::CGALSplitterWnd()
{
	m_cxSplitterGap = m_cySplitterGap = 1;
    m_cxBorder = m_cyBorder = 1;
}

CGALSplitterWnd::~CGALSplitterWnd()
{
}


BEGIN_MESSAGE_MAP(CGALSplitterWnd, CSplitterWnd)
END_MESSAGE_MAP()



// CGALSplitterWnd 메시지 처리기입니다.




void CGALSplitterWnd::OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rect)
{
	// 분할선을 없앤다.
	if(pDC == NULL)
	{
		RedrawWindow(rect, NULL, RDW_INVALIDATE | RDW_NOCHILDREN);
		return;
	}
}


int CGALSplitterWnd::HitTest(CPoint pt) const
{
	// 분할선을 선택할 수 없게 한다.
	return HTNOWHERE;
}
