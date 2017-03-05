#pragma once
#include "afxwin.h"



// CGALListOptionView �� ���Դϴ�.

class CGALGalmuriView;

class CGALListOptionView : public CFormView
{
	DECLARE_DYNCREATE(CGALListOptionView)

protected:
	CGALListOptionView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CGALListOptionView();

public:
	enum { IDD = IDD_GALLISTOPTIONVIEW };
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
	CFont m_font;
	int m_nCurrSortOrder;

	// ��Ʈ�� ���� ������
	CString m_strFileName;
	CString m_strTag;
	CString m_strYear;
	CString m_strMonth;
	CString m_strDay;
	CComboBox m_comboFileType;
	CComboBox m_comboColorType;
	CButton m_checkBNWPhoto;
	CButton m_checkColorPhoto;
	CButton m_checkSelfPhoto;
	int m_radioSortOrder;
	CButton m_btnSearch;

	void InitListOption();

	virtual void OnInitialUpdate();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnBnClickedSearchBnwphoto();
	afx_msg void OnBnClickedSearchColorphoto();
	afx_msg void OnBnClickedSearchSelfphoto();
	afx_msg void OnBnClickedSearchInit();
	afx_msg void OnBnClickedSearchSearch();
	afx_msg void OnBnClickedSearchSortOrder(UINT nID);
};


