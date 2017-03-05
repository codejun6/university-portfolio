// GALListOptionView.cpp : 구현 파일입니다.
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
	m_font.CreatePointFont(100, "맑은 고딕");
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
	// 모든 데이터와 컨트롤을 초기화한다.
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


// CGALListOptionView 진단입니다.

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


// CGALListOptionView 메시지 처리기입니다.


void CGALListOptionView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// 스태틱 텍스트의 폰트를 설정한다.
	HWND hChild = ::FindWindowEx(GetSafeHwnd(), NULL, "Static", NULL);
	while(hChild)
	{
		::SendMessage(hChild, WM_SETFONT, (WPARAM)m_font.GetSafeHandle(), TRUE);
		hChild = ::FindWindowEx(GetSafeHwnd(), hChild, "Static", NULL);
	}
	
	// 체크박스 텍스트의 폰트를 설정한다.
	GetDlgItem(IDC_SEARCH_BNWPHOTO)->SetFont(&m_font);
	GetDlgItem(IDC_SEARCH_COLORPHOTO)->SetFont(&m_font);
	GetDlgItem(IDC_SEARCH_SELFPHOTO)->SetFont(&m_font);
	
	// 버튼 텍스트의 폰트를 설정한다.
	GetDlgItem(IDC_SEARCH_SEARCH)->SetFont(&m_font);
	GetDlgItem(IDC_SEARCH_INIT)->SetFont(&m_font);

	// 라디오버튼 텍스트의 폰트를 설정한다.
	GetDlgItem(IDC_SEARCH_ADDORDER)->SetFont(&m_font);
	GetDlgItem(IDC_SEARCH_LIGHTORDER)->SetFont(&m_font);
	GetDlgItem(IDC_SEARCH_DARKORDER)->SetFont(&m_font);
	GetDlgItem(IDC_SEARCH_NAMEORDER)->SetFont(&m_font);
	GetDlgItem(IDC_SEARCH_NAMEREVORDER)->SetFont(&m_font);
	GetDlgItem(IDC_SEARCH_RECENTORDER)->SetFont(&m_font);
	GetDlgItem(IDC_SEARCH_OLDORDER)->SetFont(&m_font);

	// 컨트롤들의 초기 설정을 한다.
	m_comboFileType.SetCurSel(0);
	m_comboColorType.SetCurSel(0);
}


BOOL CGALListOptionView::OnEraseBkgnd(CDC* pDC)
{
	// 검은색 배경을 칠한다.
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(&rect, RGB(0, 0, 0));

	return TRUE;
}


void CGALListOptionView::OnPaint()
{
	CPaintDC dc(this);

	// 제목 텍스트를 그린다.
	CFont titleFont;
	titleFont.CreateFont(35, 0, 0, 0, FW_HEAVY, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "맑은 고딕");
	CFont* pOldFont = dc.SelectObject(&titleFont);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255, 255, 255));
	CRect rect;
	GetClientRect(&rect);
	dc.DrawText("걸러내기", CRect(0, 55, rect.Width(), rect.Height()), DT_SINGLELINE | DT_CENTER);
	dc.DrawText("줄세우기", CRect(0, 670, rect.Width(), rect.Height()), DT_SINGLELINE | DT_CENTER);
	dc.SelectObject(pOldFont);
}


HBRUSH CGALListOptionView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);

	switch(nCtlColor)
	{
	// 텍스트의 배경을 투명하게 하고, 텍스트 색상을 흰색으로 설정한다.
	case CTLCOLOR_STATIC:
		pDC->SetBkMode(TRANSPARENT);

		// 체크박스 텍스트의 경우, 각 체크박스의 상태에 따라 텍스트 색상을 비활성화 색상(회색)으로 설정한다.
		// 흑백사진 체크박스의 경우, 색상사진이나 자가촬영사진 체크박스가 체크된 경우 회색으로 설정한다.
		// 색상사진 체크박스의 경우, 흑백사진이나 자가촬영사진 체크박스가 체크된 경우 회색으로 설정한다.
		// 자가촬영사진 체크박스의 경우, 흑백사진 체크박스가 체크된 경우 회색으로 설정한다.
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

	// 기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}


void CGALListOptionView::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	switch(lpDrawItemStruct->CtlType)
	{
	// 콤보박스의 경우
	case ODT_COMBOBOX:
	{
		CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

		// 상태에 따라 글자색, 배경색을 설정한다.
		// 선택한 아이템의 경우
		if(lpDrawItemStruct->itemState & ODS_SELECTED)
		{
			pDC->SetTextColor(RGB(255, 255, 255));
			pDC->FillSolidRect(&lpDrawItemStruct->rcItem, RGB(0, 0, 0));
		}
		// 사용불가 아이템의 경우
		else if(lpDrawItemStruct->itemState & ODS_DISABLED)
		{
			pDC->SetTextColor(RGB(0, 0, 0));
			pDC->FillSolidRect(&lpDrawItemStruct->rcItem, RGB(179, 179, 179));
		}
		// 그 밖의 경우
		else
		{
			pDC->SetTextColor(RGB(0, 0, 0));
			pDC->FillSolidRect(&lpDrawItemStruct->rcItem, RGB(255, 255, 255));
		}

		 // 선택된 아이템의 경우
		if(lpDrawItemStruct->itemID != -1)
		{
			// 텍스트를 읽을 콤보박스를 결정한다.
			CComboBox* pCombo;
			if(nIDCtl == IDC_SEARCH_FILETYPE)  // 파일 형식 콤보박스
				pCombo = &m_comboFileType;
			else                               // 색상 계열 콤보박스
				pCombo = &m_comboColorType;

			// 선택된 아이템의 텍스트를 읽어 정렬을 적용해 그린다.
			CString str;
			pCombo->GetLBText(lpDrawItemStruct->itemID, str);
			pDC->DrawText(str, &lpDrawItemStruct->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
		break;
	}

	// 버튼의 경우
	case ODT_BUTTON:
	{
		CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

		// 버튼이 눌렸을 경우, 색을 바꿔준다.
		CRect rect(lpDrawItemStruct->rcItem);
		if(lpDrawItemStruct->itemState & ODS_SELECTED)
			pDC->FillSolidRect(&rect, RGB(179, 179, 179));
		else
			pDC->FillSolidRect(&rect, RGB(255, 255, 255));

		// 버튼에 따라 텍스트를 그린다.
		CString str;
		GetDlgItem(nIDCtl)->GetWindowText(str);
		pDC->DrawText(str, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		break;
	}

	// 그 밖의 경우
	default:
		CFormView::OnDrawItem(nIDCtl, lpDrawItemStruct);
		break;
	}
}


void CGALListOptionView::OnBnClickedSearchBnwphoto()
{
	// 흑백사진 체크박스를 체크한 경우
	if(m_checkBNWPhoto.GetCheck())
	{
		// 색상사진이나 자가촬영사진 체크박스가 체크된 경우, 흑백사진 체크박스를 체크하지 못하게 체크를 해제한다.
		if(m_checkColorPhoto.GetCheck() || m_checkSelfPhoto.GetCheck())
		{
			m_checkBNWPhoto.SetCheck(FALSE);
		}
		// 정상적으로 체크한 경우, 색상 계열 콤보박스를 선택하지 못하게 비활성화한다.
		else
		{
			m_comboColorType.SetCurSel(0);
			m_comboColorType.EnableWindow(FALSE);
		}
	}
	// 흑백사진 체크박스의 체크를 해제한 경우
	else
	{
		// 색상 계열 콤보박스를 선택할 수 있게 활성화한다.
		m_comboColorType.EnableWindow(TRUE);
	}

	// 컨트롤의 모양을 갱신한다.
	m_btnSearch.SetFocus();
	m_checkBNWPhoto.Invalidate();
	m_checkColorPhoto.Invalidate();
	m_checkSelfPhoto.Invalidate();
}


void CGALListOptionView::OnBnClickedSearchColorphoto()
{
	// 색상사진 체크박스를 체크한 경우
	if(m_checkColorPhoto.GetCheck())
	{
		// 흑백사진이나 자가촬영사진 체크박스가 체크된 경우, 색상사진 체크박스를 체크하지 못하게 체크를 해제한다.
		if(m_checkBNWPhoto.GetCheck() || m_checkSelfPhoto.GetCheck())
		{
			m_checkColorPhoto.SetCheck(FALSE);
		}
	}

	// 컨트롤의 모양을 갱신한다.
	m_btnSearch.SetFocus();
	m_checkBNWPhoto.Invalidate();
	m_checkColorPhoto.Invalidate();
	m_checkSelfPhoto.Invalidate();
}


void CGALListOptionView::OnBnClickedSearchSelfphoto()
{
	// 자가촬영사진 체크박스를 체크한 경우
	if(m_checkSelfPhoto.GetCheck())
	{
		// 흑백사진 체크박스가 체크된 경우, 자가촬영사진 체크박스를 체크하지 못하게 체크를 해제한다.
		if(m_checkBNWPhoto.GetCheck())
		{
			m_checkSelfPhoto.SetCheck(FALSE);
		}
		// 색상사진 체크박스가 체크된 경우, 색상사진 체크박스를 체크하지 못하게 체크를 해제한다.
		// (자가촬영사진은 이미 흑백사진을 제외한 색상사진이기 때문이다.)
		else
		{
			m_checkColorPhoto.SetCheck(FALSE);
		}
	}

	// 컨트롤의 모양을 갱신한다.
	m_btnSearch.SetFocus();
	m_checkBNWPhoto.Invalidate();
	m_checkColorPhoto.Invalidate();
	m_checkSelfPhoto.Invalidate();
}


void CGALListOptionView::OnBnClickedSearchInit()
{
	// 검색 부분에 있는 데이터와 컨트롤을 초기화한다.
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

	// 원본 썸네일 컨트롤 배열이 비어있다면, 아무 동작도 하지 않는다.
	if(m_pMainView->m_arrOriginThumb.IsEmpty())
		return;

	// 출력 썸네일 컨트롤 배열에 원본의 항목을 복사하고 선택되어 있는 정렬 순서에 따라 정렬한다.
	m_pMainView->m_arrOutputThumb.RemoveAll();
	m_pMainView->m_arrOutputThumb.Copy(m_pMainView->m_arrOriginThumb);
	m_nCurrSortOrder = -1;
	OnBnClickedSearchSortOrder(0);
	m_pMainView->UpdateScroll();
}


void CGALListOptionView::OnBnClickedSearchSearch()
{
	// 원본 썸네일 컨트롤 배열이 비어있다면, 아무 동작도 하지 않는다.
	if(m_pMainView->m_arrOriginThumb.IsEmpty())
		return;

	// 설정한 검색 옵션을 보정한다.
	UpdateData(TRUE);
	m_strTag.Remove(' ');
	if(m_strTag != "" && m_strTag[0] != '#') m_strTag.Insert(0, '#');
	while(m_strYear[0] == '0')  m_strYear.Delete(0);
	while(m_strMonth[0] == '0') m_strMonth.Delete(0);
	while(m_strDay[0] == '0')   m_strDay.Delete(0);
	UpdateData(FALSE);

	// 태그 문자열에서 각각의 태그를 분리해 태그 배열에 추가한다.
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

	// 원본의 항목 중 검색 조건에 적합한 항목들로만 출력 썸네일 컨트롤 배열을 구성한다.
	m_pMainView->m_arrOutputThumb.RemoveAll();
	CGALThumbCtrl* pThumbCtrl;
	CGALImageFeature* pFeature;
	for(int i = 0; i < m_pMainView->m_arrOriginThumb.GetSize(); i++)
	{
		pThumbCtrl = (CGALThumbCtrl*)m_pMainView->m_arrOriginThumb[i];
		pFeature = pThumbCtrl->GetImageFeature();

		// 파일명 옵션이 설정되어 있을 때, 파일명의 일부분이 일치하지 않으면 제외한다.
		if(m_strFileName != "" && pFeature->m_strFileName.Find(m_strFileName) == -1)
			continue;
		
		// 태그 옵션이 설정되어 있을 때, 모든 태그가 포함되어 있지 않으면 제외한다.
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

		// 찍은 날짜는 연도, 월, 일에 대해 각각 적용되므로 원하는 기준만 적용할 수 있다.
		// 연도, 월, 일 각각에 대해 일치하지 않으면 제외한다.
		if(m_strYear != "" && pFeature->m_timeCaptured.GetYear() != _ttoi(m_strYear))
			continue;
		if(m_strMonth != "" && pFeature->m_timeCaptured.GetMonth() != _ttoi(m_strMonth))
			continue;
		if(m_strDay != "" && pFeature->m_timeCaptured.GetDay() != _ttoi(m_strDay))
			continue;

		// 파일 형식 옵션이 "전체"가 아닐 때, 파일 형식이 일치하지 않으면 제외한다.
		if(m_comboFileType.GetCurSel() != 0)
		{
			CString str;
			m_comboFileType.GetLBText(m_comboFileType.GetCurSel(), str);
			if(pFeature->m_strFileType.Compare(str) != 0)
				continue;
		}

		// 색상 계열 옵션이 "전체"가 아닐 때, 흑백사진이거나 색상 계열이 일치하지 않으면 제외한다.
		if(m_comboColorType.GetCurSel() != 0)
		{
			// 흑백사진은 색상 계열 값이 0이므로 무조건 제외된다.
			if(m_comboColorType.GetCurSel() != pFeature->m_ctColorType)
				continue;
		}

		// 흑백사진 옵션이 설정되어 있을 때, 흑백사진이 아니면 제외한다.
		if(m_checkBNWPhoto.GetCheck() && !pFeature->m_bIsBNWPhoto)
			continue;

		// 색상사진 옵션이 설정되어 있을 때, 흑백사진이면 제외한다.
		if(m_checkColorPhoto.GetCheck() && pFeature->m_bIsBNWPhoto)
			continue;

		// 자가촬영사진 옵션이 설정되어 있을 때, 자가촬영사진이 아니면 제외한다.
		if(m_checkSelfPhoto.GetCheck() && !pFeature->m_bIsSelfPhoto)
			continue;

		// 제외 대상이 아니면 출력 썸네일 컨트롤 배열에 추가한다.
		m_pMainView->m_arrOutputThumb.Add(pThumbCtrl);
	}

	// 선택되어 있는 정렬 순서에 따라 정렬한다.
	m_nCurrSortOrder = -1;
	OnBnClickedSearchSortOrder(0);

	// 썸네일 컨트롤의 개수에 따라 스크롤을 설정한다.
	m_pMainView->UpdateScroll();

	// 아무 것도 검색되지 않았을 경우, 메시지 박스를 보여준다.
	if(m_pMainView->m_arrOutputThumb.IsEmpty())
	{
		AfxMessageBox("검색 조건에 맞는 항목이 없습니다.");
		OnBnClickedSearchInit();
	}
}


void CGALListOptionView::OnBnClickedSearchSortOrder(UINT nID)
{
	// 원본 썸네일 컨트롤 배열이 비어있다면, 아무 동작도 하지 않는다.
	if(m_pMainView->m_arrOriginThumb.IsEmpty())
		return;

	UpdateData(TRUE);

	// 현재 정렬 순서와 선택한 정렬 순서가 같은 경우, 아무 동작도 하지 않는다.
	// (이미 선택된 라디오 버튼을 선택한 경우에 발생하는 불필요한 동작을 방지해 성능을 향상시킨다.)
	if(m_nCurrSortOrder == m_radioSortOrder)
		return;

	m_nCurrSortOrder = m_radioSortOrder;

	// 선택한 라디오 버튼에 따라 썸네일 컨트롤 배열을 정렬한다.
	// (단순한 구현을 위해 버블 정렬을 이용해 정렬한다.)
	// (성능 향상을 위해 정렬 방법을 바꿀 수 있다.)
	int count = m_pMainView->m_arrOutputThumb.GetSize();
	switch(nID)
	{
	// 추가한 순 라디오 버튼인 경우(기본값)
	case IDC_SEARCH_ADDORDER:
	{
		int value[2];
		for(int i = 0; i < count - 1; i++)
		{
			for(int j = 0; j < count - i - 1; j++)
			{
				// 추가한 순서는 컨트롤 ID의 순서와 일치하므로 컨트롤 ID를 이용한다.
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

	// 밝은 순 라디오 버튼인 경우
	case IDC_SEARCH_LIGHTORDER:
	{
		int value[2];
		for(int i = 0; i < count - 1; i++)
		{
			for(int j = 0; j < count - i - 1; j++)
			{
				// 평균 밝기값을 이용한다. 밝을수록 앞 쪽에 위치한다.
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

	// 어두운 순 라디오 버튼인 경우
	case IDC_SEARCH_DARKORDER:
	{
		int value[2];
		for(int i = 0; i < count - 1; i++)
		{
			for(int j = 0; j < count - i - 1; j++)
			{
				// 평균 밝기값을 이용한다. 어두울수록 앞 쪽에 위치한다.
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
		
	// 이름 순 라디오 버튼인 경우
	case IDC_SEARCH_NAMEORDER:
	{
		CString value[2];
		for(int i = 0; i < count - 1; i++)
		{
			for(int j = 0; j < count - i - 1; j++)
			{
				// 파일명을 이용한다. 사전 순서가 작을수록 앞 쪽에 위치한다.
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

	// 이름 역순 라디오 버튼인 경우
	case IDC_SEARCH_NAMEREVORDER:
	{
		CString value[2];
		for(int i = 0; i < count - 1; i++)
		{
			for(int j = 0; j < count - i - 1; j++)
			{
				// 파일명을 이용한다. 사전 순서가 클수록 앞 쪽에 위치한다.
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

	// 최근 날짜 순 라디오 버튼인 경우
	case IDC_SEARCH_RECENTORDER:
	{
		CTime value[2];
		for(int i = 0; i < count - 1; i++)
		{
			for(int j = 0; j < count - i - 1; j++)
			{
				// 찍은 날짜를 이용한다. 현재 시간에 가까울수록 앞 쪽에 위치한다.
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

	// 오래된 날짜 순 라디오 버튼인 경우
	case IDC_SEARCH_OLDORDER:
	{
		CTime value[2];
		for(int i = 0; i < count - 1; i++)
		{
			for(int j = 0; j < count - i - 1; j++)
			{
				// 찍은 날짜를 이용한다. 오래된 시간일수록 앞 쪽에 위치한다.
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

	// 출력 썸네일 컨트롤 배열을 갱신하고 첫 번째 썸네일 컨트롤을 선택한다.
	m_pMainView->UpdateOutputThumbArray();
	m_pMainView->SelectThumbCtrl(0);
}

