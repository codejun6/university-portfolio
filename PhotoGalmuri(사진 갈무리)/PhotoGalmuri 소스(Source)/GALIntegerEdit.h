#pragma once


// CGALIntegerEdit

class CGALIntegerEdit : public CEdit
{
	DECLARE_DYNAMIC(CGALIntegerEdit)

public:
	CGALIntegerEdit();
	virtual ~CGALIntegerEdit();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};


