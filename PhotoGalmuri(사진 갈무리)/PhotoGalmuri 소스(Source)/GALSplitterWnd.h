#pragma once


// CGALSplitterWnd

class CGALSplitterWnd : public CSplitterWnd
{
	DECLARE_DYNAMIC(CGALSplitterWnd)

public:
	CGALSplitterWnd();
	virtual ~CGALSplitterWnd();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rect);
	virtual int HitTest(CPoint pt) const;
};


