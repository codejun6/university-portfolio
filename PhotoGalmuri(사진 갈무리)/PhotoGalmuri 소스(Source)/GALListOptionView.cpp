// GALListOptionView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "PhotoGalmuri.h"
#include "GALListOptionView.h"

#include "GALGalmuriView.h"
#include "GALThumbCtrl.h"
#include "GALImageFeature.h"


// CGALListOptionView

IMPLEMENT_DYNCREATE(CGALListOptionView, CFormView)

CGALListOptionView::CGALListOptionView()
	: CFormView(CGALListOptionView::IDD)
	, m_nCurrSortOrder(0)
	, m_strFileName(_T(""))
	, m_strTag(_T(""))
	, m_strYear(_T(""))
	, m_strMonth(_T(""))
	, m_strDay(_T(""))
	, m_radioSortOrder(0)
{
	m_font.CreatePointFont(100, "���� ���");
}

CGALListOptionView::~CGALListOptionView()
{
	m_font.DeleteObject();
}

void CGALListOptionView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SEARCH_FILENAME, m_strFileName);
	DDX_Text(pDX, IDC_SEARCH_TAG, m_strTag);
	DDX_Text(pDX, IDC_SEARCH_YEAR, m_strYear);
	DDV_MaxChars(pDX, m_strYear, 4);
	DDX_Text(pDX, IDC_SEARCH_MONTH, m_strMonth);
	DDV_MaxChars(pDX, m_strMonth, 2);
	DDX_Text(pDX, IDC_SEARCH_DAY, m_strDay);
	DDV_MaxChars(pDX, m_strDay, 2);
	DDX_Control(pDX, IDC_SEARCH_FILETYPE, m_comboFileType);
	DDX_Control(pDX, IDC_SEARCH_COLORTYPE, m_comboColorType);
	DDX_Control(pDX, IDC_SEARCH_BNWPHOTO, m_checkBNWPhoto);
	DDX_Control(pDX, IDC_SEARCH_COLORPHOTO, m_checkColorPhoto);
	DDX_Control(pDX, IDC_SEARCH_SELFPHOTO, m_checkSelfPhoto);
	DDX_Radio(pDX, IDC_SEARCH_ADDORDER, m_radioSortOrder);
	DDX_Control(pDX, IDC_SEARCH_SEARCH, m_btnSearch);
}

BEGIN_MESSAGE_MAP(CGALListOptionView, CFormView)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_SEARCH_BNWPHOTO, &CGALListOptionView::OnBnClickedSearchBnwphoto)
	ON_BN_CLICKED(IDC_SEARCH_COLORPHOTO, &CGALListOptionView::OnBnClickedSearchColorphoto)
	ON_BN_CLICKED(IDC_SEARCH_SELFPHOTO, &CGALListOptionView::OnBnClickedSearchSelfphoto)
	ON_BN_CLICKED(IDC_SEARCH_INIT, &CGALListOptionView::OnBnClickedSearchInit)
	ON_BN_CLICKED(IDC_SEARCH_SEARCH, &CGALListOptionView::OnBnClickedSearchSearch)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_SEARCH_ADDORDER, IDC_SEARCH_OLDORDER, &CGALListOptionView::OnBnClickedSearchSortOrder)
END_MESSAGE_MAP()


void CGALListOptionView::InitListOption()
{
	// ��� �����Ϳ� ��Ʈ���� �ʱ�ȭ�Ѵ�.
	m_strFileName = m_strTag = m_strYear = m_strMonth = m_strDay = "";
	m_comboFileType.SetCurSel(0);
	m_comboColorType.SetCurSel(0);
	m_checkBNWPhoto.SetCheck(FALSE);
	m_checkColorPhoto.SetCheck(FALSE);
	m_checkSelfPhoto.SetCheck(FALSE);
	m_nCurrSortOrder = m_radioSortOrder = 0;
	m_checkBNWPhoto.Invalidate();
	m_checkColorPhoto.Invalidate();
	m_checkSelfPhoto.Invalidate();
	m_comboColorType.EnableWindow(TRUE);
	UpdateData(FALSE);
}


// CGALListOptionView �����Դϴ�.

#ifdef _DEBUG
void CGALListOptionView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CGALListOptionView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CGALListOptionView �޽��� ó�����Դϴ�.


void CGALListOptionView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// ����ƽ �ؽ�Ʈ�� ��Ʈ�� �����Ѵ�.
	HWND hChild = ::FindWindowEx(GetSafeHwnd(), NULL, "Static", NULL);
	while(hChild)
	{
		::SendMessage(hChild, WM_SETFONT, (WPARAM)m_font.GetSafeHandle(), TRUE);
		hChild = ::FindWindowEx(GetSafeHwnd(), hChild, "Static", NULL);
	}
	
	// üũ�ڽ� �ؽ�Ʈ�� ��Ʈ�� �����Ѵ�.
	GetDlgItem(IDC_SEARCH_BNWPHOTO)->SetFont(&m_font);
	GetDlgItem(IDC_SEARCH_COLORPHOTO)->SetFont(&m_font);
	GetDlgItem(IDC_SEARCH_SELFPHOTO)->SetFont(&m_font);
	
	// ��ư �ؽ�Ʈ�� ��Ʈ�� �����Ѵ�.
	GetDlgItem(IDC_SEARCH_SEARCH)->SetFont(&m_font);
	GetDlgItem(IDC_SEARCH_INIT)->SetFont(&m_font);

	// ������ư �ؽ�Ʈ�� ��Ʈ�� �����Ѵ�.
	GetDlgItem(IDC_SEARCH_ADDORDER)->SetFont(&m_font);
	GetDlgItem(IDC_SEARCH_LIGHTORDER)->SetFont(&m_font);
	GetDlgItem(IDC_SEARCH_DARKORDER)->SetFont(&m_font);
	GetDlgItem(IDC_SEARCH_NAMEORDER)->SetFont(&m_font);
	GetDlgItem(IDC_SEARCH_NAMEREVORDER)->SetFont(&m_font);
	GetDlgItem(IDC_SEARCH_RECENTORDER)->SetFont(&m_font);
	GetDlgItem(IDC_SEARCH_OLDORDER)->SetFont(&m_font);

	// ��Ʈ�ѵ��� �ʱ� ������ �Ѵ�.
	m_comboFileType.SetCurSel(0);
	m_comboColorType.SetCurSel(0);
}


BOOL CGALListOptionView::OnEraseBkgnd(CDC* pDC)
{
	// ������ ����� ĥ�Ѵ�.
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(&rect, RGB(0, 0, 0));

	return TRUE;
}


void CGALListOptionView::OnPaint()
{
	CPaintDC dc(this);

	// ���� �ؽ�Ʈ�� �׸���.
	CFont titleFont;
	titleFont.CreateFont(35, 0, 0, 0, FW_HEAVY, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "���� ���");
	CFont* pOldFont = dc.SelectObject(&titleFont);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255, 255, 255));
	CRect rect;
	GetClientRect(&rect);
	dc.DrawText("�ɷ�����", CRect(0, 55, rect.Width(), rect.Height()), DT_SINGLELINE | DT_CENTER);
	dc.DrawText("�ټ����", CRect(0, 670, rect.Width(), rect.Height()), DT_SINGLELINE | DT_CENTER);
	dc.SelectObject(pOldFont);
}


HBRUSH CGALListOptionView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);

	switch(nCtlColor)
	{
	// �ؽ�Ʈ�� ����� �����ϰ� �ϰ�, �ؽ�Ʈ ������ ������� �����Ѵ�.
	case CTLCOLOR_STATIC:
		pDC->SetBkMode(TRANSPARENT);

		// üũ�ڽ� �ؽ�Ʈ�� ���, �� üũ�ڽ��� ���¿� ���� �ؽ�Ʈ ������ ��Ȱ��ȭ ����(ȸ��)���� �����Ѵ�.
		// ������ üũ�ڽ��� ���, ��������̳� �ڰ��Կ����� üũ�ڽ��� üũ�� ��� ȸ������ �����Ѵ�.
		// ������� üũ�ڽ��� ���, �������̳� �ڰ��Կ����� üũ�ڽ��� üũ�� ��� ȸ������ �����Ѵ�.
		// �ڰ��Կ����� üũ�ڽ��� ���, ������ üũ�ڽ��� üũ�� ��� ȸ������ �����Ѵ�.
		if((pWnd->GetDlgCtrlID() == IDC_SEARCH_BNWPHOTO && (m_checkColorPhoto.GetCheck() || m_checkSelfPhoto.GetCheck()))
			|| (pWnd->GetDlgCtrlID() == IDC_SEARCH_COLORPHOTO && (m_checkBNWPhoto.GetCheck() || m_checkSelfPhoto.GetCheck()))
			|| (pWnd->GetDlgCtrlID() == IDC_SEARCH_SELFPHOTO && m_checkBNWPhoto.GetCheck()))
		{
			pDC->SetTextColor(RGB(100, 100, 100));
		}
		else
			pDC->SetTextColor(RGB(255, 255, 255));

		return (HBRUSH)::GetStockObject(BLACK_BRUSH);
	}

	// �⺻���� �������� ������ �ٸ� �귯�ø� ��ȯ�մϴ�.
	return hbr;
}


void CGALListOptionView::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	switch(lpDrawItemStruct->CtlType)
	{
	// �޺��ڽ��� ���
	case ODT_COMBOBOX:
	{
		CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

		// ���¿� ���� ���ڻ�, ������ �����Ѵ�.
		// ������ �������� ���
		if(lpDrawItemStruct->itemState & ODS_SELECTED)
		{
			pDC->SetTextColor(RGB(255, 255, 255));
			pDC->FillSolidRect(&lpDrawItemStruct->rcItem, RGB(0, 0, 0));
		}
		// ���Ұ� �������� ���
		else if(lpDrawItemStruct->itemState & ODS_DISABLED)
		{
			pDC->SetTextColor(RGB(0, 0, 0));
			pDC->FillSolidRect(&lpDrawItemStruct->rcItem, RGB(179, 179, 179));
		}
		// �� ���� ���
		else
		{
			pDC->SetTextColor(RGB(0, 0, 0));
			pDC->FillSolidRect(&lpDrawItemStruct->rcItem, RGB(255, 255, 255));
		}

		 // ���õ� �������� ���
		if(lpDrawItemStruct->itemID != -1)
		{
			// �ؽ�Ʈ�� ���� �޺��ڽ��� �����Ѵ�.
			CComboBox* pCombo;
			if(nIDCtl == IDC_SEARCH_FILETYPE)  // ���� ���� �޺��ڽ�
				pCombo = &m_comboFileType;
			else                               // ���� �迭 �޺��ڽ�
				pCombo = &m_comboColorType;

			// ���õ� �������� �ؽ�Ʈ�� �о� ������ ������ �׸���.
			CString str;
			pCombo->GetLBText(lpDrawItemStruct->itemID, str);
			pDC->DrawText(str, &lpDrawItemStruct->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
		break;
	}

	// ��ư�� ���
	case ODT_BUTTON:
	{
		CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

		// ��ư�� ������ ���, ���� �ٲ��ش�.
		CRect rect(lpDrawItemStruct->rcItem);
		if(lpDrawItemStruct->itemState & ODS_SELECTED)
			pDC->FillSolidRect(&rect, RGB(179, 179, 179));
		else
			pDC->FillSolidRect(&rect, RGB(255, 255, 255));

		// ��ư�� ���� �ؽ�Ʈ�� �׸���.
		CString str;
		GetDlgItem(nIDCtl)->GetWindowText(str);
		pDC->DrawText(str, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		break;
	}

	// �� ���� ���
	default:
		CFormView::OnDrawItem(nIDCtl, lpDrawItemStruct);
		break;
	}
}


void CGALListOptionView::OnBnClickedSearchBnwphoto()
{
	// ������ üũ�ڽ��� üũ�� ���
	if(m_checkBNWPhoto.GetCheck())
	{
		// ��������̳� �ڰ��Կ����� üũ�ڽ��� üũ�� ���, ������ üũ�ڽ��� üũ���� ���ϰ� üũ�� �����Ѵ�.
		if(m_checkColorPhoto.GetCheck() || m_checkSelfPhoto.GetCheck())
		{
			m_checkBNWPhoto.SetCheck(FALSE);
		}
		// ���������� üũ�� ���, ���� �迭 �޺��ڽ��� �������� ���ϰ� ��Ȱ��ȭ�Ѵ�.
		else
		{
			m_comboColorType.SetCurSel(0);
			m_comboColorType.EnableWindow(FALSE);
		}
	}
	// ������ üũ�ڽ��� üũ�� ������ ���
	else
	{
		// ���� �迭 �޺��ڽ��� ������ �� �ְ� Ȱ��ȭ�Ѵ�.
		m_comboColorType.EnableWindow(TRUE);
	}

	// ��Ʈ���� ����� �����Ѵ�.
	m_btnSearch.SetFocus();
	m_checkBNWPhoto.Invalidate();
	m_checkColorPhoto.Invalidate();
	m_checkSelfPhoto.Invalidate();
}


void CGALListOptionView::OnBnClickedSearchColorphoto()
{
	// ������� üũ�ڽ��� üũ�� ���
	if(m_checkColorPhoto.GetCheck())
	{
		// �������̳� �ڰ��Կ����� üũ�ڽ��� üũ�� ���, ������� üũ�ڽ��� üũ���� ���ϰ� üũ�� �����Ѵ�.
		if(m_checkBNWPhoto.GetCheck() || m_checkSelfPhoto.GetCheck())
		{
			m_checkColorPhoto.SetCheck(FALSE);
		}
	}

	// ��Ʈ���� ����� �����Ѵ�.
	m_btnSearch.SetFocus();
	m_checkBNWPhoto.Invalidate();
	m_checkColorPhoto.Invalidate();
	m_checkSelfPhoto.Invalidate();
}


void CGALListOptionView::OnBnClickedSearchSelfphoto()
{
	// �ڰ��Կ����� üũ�ڽ��� üũ�� ���
	if(m_checkSelfPhoto.GetCheck())
	{
		// ������ üũ�ڽ��� üũ�� ���, �ڰ��Կ����� üũ�ڽ��� üũ���� ���ϰ� üũ�� �����Ѵ�.
		if(m_checkBNWPhoto.GetCheck())
		{
			m_checkSelfPhoto.SetCheck(FALSE);
		}
		// ������� üũ�ڽ��� üũ�� ���, ������� üũ�ڽ��� üũ���� ���ϰ� üũ�� �����Ѵ�.
		// (�ڰ��Կ������� �̹� �������� ������ ��������̱� �����̴�.)
		else
		{
			m_checkColorPhoto.SetCheck(FALSE);
		}
	}

	// ��Ʈ���� ����� �����Ѵ�.
	m_btnSearch.SetFocus();
	m_checkBNWPhoto.Invalidate();
	m_checkColorPhoto.Invalidate();
	m_checkSelfPhoto.Invalidate();
}


void CGALListOptionView::OnBnClickedSearchInit()
{
	// �˻� �κп� �ִ� �����Ϳ� ��Ʈ���� �ʱ�ȭ�Ѵ�.
	m_strFileName = m_strTag = m_strYear = m_strMonth = m_strDay = "";
	m_comboFileType.SetCurSel(0);
	m_comboColorType.SetCurSel(0);
	m_checkBNWPhoto.SetCheck(FALSE);
	m_checkColorPhoto.SetCheck(FALSE);
	m_checkSelfPhoto.SetCheck(FALSE);
	m_checkBNWPhoto.Invalidate();
	m_checkColorPhoto.Invalidate();
	m_checkSelfPhoto.Invalidate();
	m_comboColorType.EnableWindow(TRUE);
	UpdateData(FALSE);

	// ���� ����� ��Ʈ�� �迭�� ����ִٸ�, �ƹ� ���۵� ���� �ʴ´�.
	if(m_pMainView->m_arrOriginThumb.IsEmpty())
		return;

	// ��� ����� ��Ʈ�� �迭�� ������ �׸��� �����ϰ� ���õǾ� �ִ� ���� ������ ���� �����Ѵ�.
	m_pMainView->m_arrOutputThumb.RemoveAll();
	m_pMainView->m_arrOutputThumb.Copy(m_pMainView->m_arrOriginThumb);
	m_nCurrSortOrder = -1;
	OnBnClickedSearchSortOrder(0);
	m_pMainView->UpdateScroll();
}


void CGALListOptionView::OnBnClickedSearchSearch()
{
	// ���� ����� ��Ʈ�� �迭�� ����ִٸ�, �ƹ� ���۵� ���� �ʴ´�.
	if(m_pMainView->m_arrOriginThumb.IsEmpty())
		return;

	// ������ �˻� �ɼ��� �����Ѵ�.
	UpdateData(TRUE);
	m_strTag.Remove(' ');
	if(m_strTag != "" && m_strTag[0] != '#') m_strTag.Insert(0, '#');
	while(m_strYear[0] == '0')  m_strYear.Delete(0);
	while(m_strMonth[0] == '0') m_strMonth.Delete(0);
	while(m_strDay[0] == '0')   m_strDay.Delete(0);
	UpdateData(FALSE);

	// �±� ���ڿ����� ������ �±׸� �и��� �±� �迭�� �߰��Ѵ�.
	CStringArray tagArr;
	if(m_strTag != "")
	{
		int pos = 0;
		CString str = m_strTag.Tokenize("#", pos);	
		while(str != "")
		{
			tagArr.Add(str);
			str = m_strTag.Tokenize("#", pos);
		}
	}

	// ������ �׸� �� �˻� ���ǿ� ������ �׸��θ� ��� ����� ��Ʈ�� �迭�� �����Ѵ�.
	m_pMainView->m_arrOutputThumb.RemoveAll();
	CGALThumbCtrl* pThumbCtrl;
	CGALImageFeature* pFeature;
	for(int i = 0; i < m_pMainView->m_arrOriginThumb.GetSize(); i++)
	{
		pThumbCtrl = (CGALThumbCtrl*)m_pMainView->m_arrOriginThumb[i];
		pFeature = pThumbCtrl->GetImageFeature();

		// ���ϸ� �ɼ��� �����Ǿ� ���� ��, ���ϸ��� �Ϻκ��� ��ġ���� ������ �����Ѵ�.
		if(m_strFileName != "" && pFeature->m_strFileName.Find(m_strFileName) == -1)
			continue;
		
		// �±� �ɼ��� �����Ǿ� ���� ��, ��� �±װ� ���ԵǾ� ���� ������ �����Ѵ�.
		if(!tagArr.IsEmpty())
		{
			BOOL isAllExist = TRUE;
			for(int i = 0; i < tagArr.GetSize(); i++)
			{
				BOOL isExist = FALSE;
				for(int j = 0; j < pFeature->m_arrTag.GetSize(); j++)
				{
					if(tagArr[i].Compare(pFeature->m_arrTag[j]) == 0)
					{
						isExist = TRUE;
						break;
					}
				}
				if(!isExist)
				{
					isAllExist = FALSE;
					break;
				}
			}
			if(!isAllExist)
				continue;
		}

		// ���� ��¥�� ����, ��, �Ͽ� ���� ���� ����ǹǷ� ���ϴ� ���ظ� ������ �� �ִ�.
		// ����, ��, �� ������ ���� ��ġ���� ������ �����Ѵ�.
		if(m_strYear != "" && pFeature->m_timeCaptured.GetYear() != _ttoi(m_strYear))
			continue;
		if(m_strMonth != "" && pFeature->m_timeCaptured.GetMonth() != _ttoi(m_strMonth))
			continue;
		if(m_strDay != "" && pFeature->m_timeCaptured.GetDay() != _ttoi(m_strDay))
			continue;

		// ���� ���� �ɼ��� "��ü"�� �ƴ� ��, ���� ������ ��ġ���� ������ �����Ѵ�.
		if(m_comboFileType.GetCurSel() != 0)
		{
			CString str;
			m_comboFileType.GetLBText(m_comboFileType.GetCurSel(), str);
			if(pFeature->m_strFileType.Compare(str) != 0)
				continue;
		}

		// ���� �迭 �ɼ��� "��ü"�� �ƴ� ��, �������̰ų� ���� �迭�� ��ġ���� ������ �����Ѵ�.
		if(m_comboColorType.GetCurSel() != 0)
		{
			// �������� ���� �迭 ���� 0�̹Ƿ� ������ ���ܵȴ�.
			if(m_comboColorType.GetCurSel() != pFeature->m_ctColorType)
				continue;
		}

		// ������ �ɼ��� �����Ǿ� ���� ��, �������� �ƴϸ� �����Ѵ�.
		if(m_checkBNWPhoto.GetCheck() && !pFeature->m_bIsBNWPhoto)
			continue;

		// ������� �ɼ��� �����Ǿ� ���� ��, �������̸� �����Ѵ�.
		if(m_checkColorPhoto.GetCheck() && pFeature->m_bIsBNWPhoto)
			continue;

		// �ڰ��Կ����� �ɼ��� �����Ǿ� ���� ��, �ڰ��Կ������� �ƴϸ� �����Ѵ�.
		if(m_checkSelfPhoto.GetCheck() && !pFeature->m_bIsSelfPhoto)
			continue;

		// ���� ����� �ƴϸ� ��� ����� ��Ʈ�� �迭�� �߰��Ѵ�.
		m_pMainView->m_arrOutputThumb.Add(pThumbCtrl);
	}

	// ���õǾ� �ִ� ���� ������ ���� �����Ѵ�.
	m_nCurrSortOrder = -1;
	OnBnClickedSearchSortOrder(0);

	// ����� ��Ʈ���� ������ ���� ��ũ���� �����Ѵ�.
	m_pMainView->UpdateScroll();

	// �ƹ� �͵� �˻����� �ʾ��� ���, �޽��� �ڽ��� �����ش�.
	if(m_pMainView->m_arrOutputThumb.IsEmpty())
	{
		AfxMessageBox("�˻� ���ǿ� �´� �׸��� �����ϴ�.");
		OnBnClickedSearchInit();
	}
}


void CGALListOptionView::OnBnClickedSearchSortOrder(UINT nID)
{
	// ���� ����� ��Ʈ�� �迭�� ����ִٸ�, �ƹ� ���۵� ���� �ʴ´�.
	if(m_pMainView->m_arrOriginThumb.IsEmpty())
		return;

	UpdateData(TRUE);

	// ���� ���� ������ ������ ���� ������ ���� ���, �ƹ� ���۵� ���� �ʴ´�.
	// (�̹� ���õ� ���� ��ư�� ������ ��쿡 �߻��ϴ� ���ʿ��� ������ ������ ������ ����Ų��.)
	if(m_nCurrSortOrder == m_radioSortOrder)
		return;

	m_nCurrSortOrder = m_radioSortOrder;

	// ������ ���� ��ư�� ���� ����� ��Ʈ�� �迭�� �����Ѵ�.
	// (�ܼ��� ������ ���� ���� ������ �̿��� �����Ѵ�.)
	// (���� ����� ���� ���� ����� �ٲ� �� �ִ�.)
	int count = m_pMainView->m_arrOutputThumb.GetSize();
	switch(nID)
	{
	// �߰��� �� ���� ��ư�� ���(�⺻��)
	case IDC_SEARCH_ADDORDER:
	{
		int value[2];
		for(int i = 0; i < count - 1; i++)
		{
			for(int j = 0; j < count - i - 1; j++)
			{
				// �߰��� ������ ��Ʈ�� ID�� ������ ��ġ�ϹǷ� ��Ʈ�� ID�� �̿��Ѵ�.
				value[0] = ((CGALThumbCtrl*)m_pMainView->m_arrOutputThumb[j])->GetDlgCtrlID();
				value[1] = ((CGALThumbCtrl*)m_pMainView->m_arrOutputThumb[j+1])->GetDlgCtrlID();
				if(value[0] > value[1])
				{
					CGALThumbCtrl* temp = (CGALThumbCtrl*)m_pMainView->m_arrOutputThumb[j];
					m_pMainView->m_arrOutputThumb[j] = m_pMainView->m_arrOutputThumb[j+1];
					m_pMainView->m_arrOutputThumb[j+1] = temp;
				}
			}
		}
	}
	break;

	// ���� �� ���� ��ư�� ���
	case IDC_SEARCH_LIGHTORDER:
	{
		int value[2];
		for(int i = 0; i < count - 1; i++)
		{
			for(int j = 0; j < count - i - 1; j++)
			{
				// ��� ��Ⱚ�� �̿��Ѵ�. �������� �� �ʿ� ��ġ�Ѵ�.
				value[0] = ((CGALThumbCtrl*)m_pMainView->m_arrOutputThumb[j])->GetImageFeature()->m_nAvgBright;
				value[1] = ((CGALThumbCtrl*)m_pMainView->m_arrOutputThumb[j+1])->GetImageFeature()->m_nAvgBright;
				if(value[0] < value[1])
				{
					CGALThumbCtrl* temp = (CGALThumbCtrl*)m_pMainView->m_arrOutputThumb[j];
					m_pMainView->m_arrOutputThumb[j] = m_pMainView->m_arrOutputThumb[j+1];
					m_pMainView->m_arrOutputThumb[j+1] = temp;
				}
			}
		}
	}
	break;

	// ��ο� �� ���� ��ư�� ���
	case IDC_SEARCH_DARKORDER:
	{
		int value[2];
		for(int i = 0; i < count - 1; i++)
		{
			for(int j = 0; j < count - i - 1; j++)
			{
				// ��� ��Ⱚ�� �̿��Ѵ�. ��ο���� �� �ʿ� ��ġ�Ѵ�.
				value[0] = ((CGALThumbCtrl*)m_pMainView->m_arrOutputThumb[j])->GetImageFeature()->m_nAvgBright;
				value[1] = ((CGALThumbCtrl*)m_pMainView->m_arrOutputThumb[j+1])->GetImageFeature()->m_nAvgBright;
				if(value[0] > value[1])
				{
					CGALThumbCtrl* temp = (CGALThumbCtrl*)m_pMainView->m_arrOutputThumb[j];
					m_pMainView->m_arrOutputThumb[j] = m_pMainView->m_arrOutputThumb[j+1];
					m_pMainView->m_arrOutputThumb[j+1] = temp;
				}
			}
		}
	}
	break;
		
	// �̸� �� ���� ��ư�� ���
	case IDC_SEARCH_NAMEORDER:
	{
		CString value[2];
		for(int i = 0; i < count - 1; i++)
		{
			for(int j = 0; j < count - i - 1; j++)
			{
				// ���ϸ��� �̿��Ѵ�. ���� ������ �������� �� �ʿ� ��ġ�Ѵ�.
				value[0] = ((CGALThumbCtrl*)m_pMainView->m_arrOutputThumb[j])->GetImageFeature()->m_strFileName;
				value[1] = ((CGALThumbCtrl*)m_pMainView->m_arrOutputThumb[j+1])->GetImageFeature()->m_strFileName;
				if(value[0].Compare(value[1]) > 0)
				{
					CGALThumbCtrl* temp = (CGALThumbCtrl*)m_pMainView->m_arrOutputThumb[j];
					m_pMainView->m_arrOutputThumb[j] = m_pMainView->m_arrOutputThumb[j+1];
					m_pMainView->m_arrOutputThumb[j+1] = temp;
				}
			}
		}
	}
	break;

	// �̸� ���� ���� ��ư�� ���
	case IDC_SEARCH_NAMEREVORDER:
	{
		CString value[2];
		for(int i = 0; i < count - 1; i++)
		{
			for(int j = 0; j < count - i - 1; j++)
			{
				// ���ϸ��� �̿��Ѵ�. ���� ������ Ŭ���� �� �ʿ� ��ġ�Ѵ�.
				value[0] = ((CGALThumbCtrl*)m_pMainView->m_arrOutputThumb[j])->GetImageFeature()->m_strFileName;
				value[1] = ((CGALThumbCtrl*)m_pMainView->m_arrOutputThumb[j+1])->GetImageFeature()->m_strFileName;
				if(value[0].Compare(value[1]) < 0)
				{
					CGALThumbCtrl* temp = (CGALThumbCtrl*)m_pMainView->m_arrOutputThumb[j];
					m_pMainView->m_arrOutputThumb[j] = m_pMainView->m_arrOutputThumb[j+1];
					m_pMainView->m_arrOutputThumb[j+1] = temp;
				}
			}
		}
	}
	break;

	// �ֱ� ��¥ �� ���� ��ư�� ���
	case IDC_SEARCH_RECENTORDER:
	{
		CTime value[2];
		for(int i = 0; i < count - 1; i++)
		{
			for(int j = 0; j < count - i - 1; j++)
			{
				// ���� ��¥�� �̿��Ѵ�. ���� �ð��� �������� �� �ʿ� ��ġ�Ѵ�.
				value[0] = ((CGALThumbCtrl*)m_pMainView->m_arrOutputThumb[j])->GetImageFeature()->m_timeCaptured;
				value[1] = ((CGALThumbCtrl*)m_pMainView->m_arrOutputThumb[j+1])->GetImageFeature()->m_timeCaptured;
				if(value[0] < value[1])
				{
					CGALThumbCtrl* temp = (CGALThumbCtrl*)m_pMainView->m_arrOutputThumb[j];
					m_pMainView->m_arrOutputThumb[j] = m_pMainView->m_arrOutputThumb[j+1];
					m_pMainView->m_arrOutputThumb[j+1] = temp;
				}
			}
		}
	}
	break;

	// ������ ��¥ �� ���� ��ư�� ���
	case IDC_SEARCH_OLDORDER:
	{
		CTime value[2];
		for(int i = 0; i < count - 1; i++)
		{
			for(int j = 0; j < count - i - 1; j++)
			{
				// ���� ��¥�� �̿��Ѵ�. ������ �ð��ϼ��� �� �ʿ� ��ġ�Ѵ�.
				value[0] = ((CGALThumbCtrl*)m_pMainView->m_arrOutputThumb[j])->GetImageFeature()->m_timeCaptured;
				value[1] = ((CGALThumbCtrl*)m_pMainView->m_arrOutputThumb[j+1])->GetImageFeature()->m_timeCaptured;
				if(value[0] > value[1])
				{
					CGALThumbCtrl* temp = (CGALThumbCtrl*)m_pMainView->m_arrOutputThumb[j];
					m_pMainView->m_arrOutputThumb[j] = m_pMainView->m_arrOutputThumb[j+1];
					m_pMainView->m_arrOutputThumb[j+1] = temp;
				}
			}
		}
	}
	break;
	}

	// ��� ����� ��Ʈ�� �迭�� �����ϰ� ù ��° ����� ��Ʈ���� �����Ѵ�.
	m_pMainView->UpdateOutputThumbArray();
	m_pMainView->SelectThumbCtrl(0);
}

