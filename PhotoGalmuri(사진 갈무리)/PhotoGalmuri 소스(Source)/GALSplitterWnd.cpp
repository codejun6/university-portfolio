// GALSplitterWnd.cpp : ���� �����Դϴ�.
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



// CGALSplitterWnd �޽��� ó�����Դϴ�.




void CGALSplitterWnd::OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rect)
{
	// ���Ҽ��� ���ش�.
	if(pDC == NULL)
	{
		RedrawWindow(rect, NULL, RDW_INVALIDATE | RDW_NOCHILDREN);
		return;
	}
}


int CGALSplitterWnd::HitTest(CPoint pt) const
{
	// ���Ҽ��� ������ �� ���� �Ѵ�.
	return HTNOWHERE;
}
