#pragma once


// CGALSliderCtrl

class CGALSliderCtrl : public CSliderCtrl
{
	DECLARE_DYNAMIC(CGALSliderCtrl)

public:
	CGALSliderCtrl();
	virtual ~CGALSliderCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};


