#pragma once
#include "afxwin.h"



// CGALMenuView �� ���Դϴ�.

class CGALGalmuriView;

class CGALMenuView : public CFormView
{
	DECLARE_DYNCREATE(CGALMenuView)

protected:
	CGALMenuView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CGALMenuView();

public:
	enum { IDD = IDD_GALMENUVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CGALGalmuriView* m_pMainView;
	CBitmapButton m_btnOpen;
	CBitmapButton m_btnEdit;
	CBitmapButton m_btnDelete;
	CBitmapButton m_btnDeleteAll;
	CBitmapButton m_btnExit;

	virtual void OnInitialUpdate();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedOpen();
	afx_msg void OnBnClickedEdit();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedDeleteall();
	afx_msg void OnBnClickedExit();
};


