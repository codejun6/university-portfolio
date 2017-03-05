// GALMenuView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PhotoGalmuri.h"
#include "GALMenuView.h"

#include "MainFrm.h"
#include "GALSplitterWnd.h"
#include "GALGalmuriView.h"
#include "GALListOptionView.h"
#include "GALThumbCtrl.h"
#include "GALImageFeature.h"
#include "GALEditDialog.h"


// CGALMenuView

IMPLEMENT_DYNCREATE(CGALMenuView, CFormView)

CGALMenuView::CGALMenuView()
	: CFormView(CGALMenuView::IDD)
{
	
}

CGALMenuView::~CGALMenuView()
{
}

void CGALMenuView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MENU_OPEN, m_btnOpen);
	DDX_Control(pDX, IDC_MENU_EDIT, m_btnEdit);
	DDX_Control(pDX, IDC_MENU_DELETE, m_btnDelete);
	DDX_Control(pDX, IDC_MENU_DELETEALL, m_btnDeleteAll);
	DDX_Control(pDX, IDC_MENU_EXIT, m_btnExit);
}

BEGIN_MESSAGE_MAP(CGALMenuView, CFormView)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_MENU_OPEN, &CGALMenuView::OnBnClickedOpen)
	ON_BN_CLICKED(IDC_MENU_EDIT, &CGALMenuView::OnBnClickedEdit)
	ON_BN_CLICKED(IDC_MENU_DELETE, &CGALMenuView::OnBnClickedDelete)
	ON_BN_CLICKED(IDC_MENU_DELETEALL, &CGALMenuView::OnBnClickedDeleteall)
	ON_BN_CLICKED(IDC_MENU_EXIT, &CGALMenuView::OnBnClickedExit)
END_MESSAGE_MAP()


// CGALMenuView 진단입니다.

#ifdef _DEBUG
void CGALMenuView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CGALMenuView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CGALMenuView 메시지 처리기입니다.


void CGALMenuView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// 메뉴 비트맵버튼을 생성하고 설정한다.
	m_btnOpen.LoadBitmaps(IDB_MENU_OPEN_UP, IDB_MENU_OPEN_DOWN);
	m_btnOpen.SizeToContent();

	m_btnEdit.LoadBitmaps(IDB_MENU_EDIT_UP, IDB_MENU_EDIT_DOWN);
	m_btnEdit.MoveWindow(0, 130, 127, 127);
	m_btnEdit.SizeToContent();

	m_btnDelete.LoadBitmaps(IDB_MENU_DELETE_UP, IDB_MENU_DELETE_DOWN);
	m_btnDelete.MoveWindow(0, 260, 127, 127);
	m_btnDelete.SizeToContent();

	m_btnDeleteAll.LoadBitmaps(IDB_MENU_DELETEALL_UP, IDB_MENU_DELETEALL_DOWN);
	m_btnDeleteAll.MoveWindow(0, 390, 127, 127);
	m_btnDeleteAll.SizeToContent();

	m_btnExit.LoadBitmaps(IDB_MENU_EXIT_UP, IDB_MENU_EXIT_DOWN);
	m_btnExit.MoveWindow(0, 520, 127, 127);
	m_btnExit.SizeToContent();
}


BOOL CGALMenuView::OnEraseBkgnd(CDC* pDC)
{
	// 검은색 배경을 칠한다.
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(&rect, RGB(0, 0, 0));

	// 메뉴 버튼 사이에 흰색 경계선을 그린다.
	for(int i = 0; i < 5; i++)
	{
		pDC->FillSolidRect(0, 127 + (130 * i), 127, 3, RGB(255, 255, 255));
	}

	return TRUE;
}


void CGALMenuView::OnBnClickedOpen()
{
	ASSERT(m_pMainView != NULL);

	// 현재 프로그램의 작업 경로를 얻는다.
	char currPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, currPath);

	// 필터링을 위한 문자열을 초기화한다.
	char filter[] = "모든 이미지(*.*)|*.jpg;*.png;*.bmp;*.gif|"
					"JPG 이미지(*.jpg)|*.jpg|"
					"PNG 이미지(*.png)|*.png|"
					"BMP 이미지(*.bmp)|*.bmp|"
					"GIF 이미지(*.gif)|*.gif||";

	// 파일열기 대화상자를 생성하고 설정한다.
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_ALLOWMULTISELECT, filter);
	fileDlg.m_ofn.lpstrInitialDir = currPath;

	// 파일열기 대화상자를 연다.
	// 열기 버튼을 누른 경우에만 이미지를 불러온다.
	if(fileDlg.DoModal() == IDOK)
	{
		CStringArray failPathArr;

		// 하나 또는 다수의 이미지를 불러와 썸네일 컨트롤을 생성해 원본 썸네일 컨트롤 배열에 추가한다.
		for(POSITION pos = fileDlg.GetStartPosition(); pos != NULL;)
		{
			int thumbCount = m_pMainView->m_arrOriginThumb.GetSize();

			// 이미 존재하는 파일이면 추가하지 않는다.
			BOOL isExist = FALSE;
			CString pathName = fileDlg.GetNextPathName(pos);
			for(int i = 0; i < thumbCount; i++)
			{
				CGALImageFeature* pFeature = ((CGALThumbCtrl*)m_pMainView->m_arrOriginThumb[i])->GetImageFeature();
				if(pFeature->m_strFilePath.CompareNoCase(pathName) == 0)
				{
					isExist = TRUE;
					failPathArr.Add(pathName);
					break;
				}
			}
			if(isExist)
				continue;

			// 끝 다음 인덱스의 영역을 구한다.
			CRect rect;
			m_pMainView->GetThumbCtrlRect(rect, thumbCount);

			// 썸네일 컨트롤을 생성해 원본 썸네일 컨트롤 배열에 추가한다.
			CGALThumbCtrl* pThumbCtrl = new CGALThumbCtrl(pathName);
			pThumbCtrl->Create("CGALThumbCtrl", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW,
					rect, m_pMainView, thumbCount + 1);
			m_pMainView->m_arrOriginThumb.Add(pThumbCtrl);
		}

		// 출력 썸네일 컨트롤 배열에 원본의 항목을 복사하고 갱신한다.
		m_pMainView->m_arrOutputThumb.RemoveAll();
		m_pMainView->m_arrOutputThumb.Copy(m_pMainView->m_arrOriginThumb);
		m_pMainView->UpdateOutputThumbArray();

		// 마지막으로 추가한 썸네일 컨트롤을 선택한다.
		m_pMainView->SelectThumbCtrl(m_pMainView->m_arrOutputThumb.GetSize() - 1);

		// 썸네일 컨트롤의 개수에 따라 스크롤을 설정한다.
		// (썸네일 컨트롤이 화면을 넘어간다면, 스크롤을 늘린다.)
		m_pMainView->UpdateScroll();

		// 스크롤이 생긴다면 끝 위치로 스크롤한다.
		if(m_pMainView->m_arrOutputThumb.GetSize() > m_pMainView->m_nColCount * 2)
			m_pMainView->ScrollToPosition(CPoint(0, m_pMainView->GetScrollLimit(SB_VERT)));

		// 검색 및 정렬 옵션을 초기화한다.
		CGALSplitterWnd* pSplitterWnd = ((CMainFrame*)AfxGetMainWnd())->m_pSplitterWnd;
		((CGALListOptionView*)pSplitterWnd->GetPane(0, 2))->InitListOption();

		// 중복 파일인 경우, 메시지 박스를 보여준다.
		if(!failPathArr.IsEmpty())
		{
			CString str = "해당 사진은 이미 추가된 사진입니다 :";
			for(int i = 0; i < failPathArr.GetSize(); i++)
				str.Append("\n" + failPathArr[i]);
			AfxMessageBox(str);
		}
	}
}


void CGALMenuView::OnBnClickedEdit()
{
	ASSERT(m_pMainView != NULL);

	// 선택된 썸네일 컨트롤이 있는 경우
	CGALThumbCtrl* pThumbCtrl = m_pMainView->GetSelectedThumbCtrl();
	if(pThumbCtrl)
	{
		// 편집 대화상자를 열어 편집을 한다.
		CGALEditDialog dlg;
		dlg.SetOriginThumb(pThumbCtrl);
		dlg.DoModal();

		pThumbCtrl->Invalidate();
	}
}


void CGALMenuView::OnBnClickedDelete()
{
	ASSERT(m_pMainView != NULL);

	// 선택된 썸네일 컨트롤이 있는 경우
	CGALThumbCtrl* pThumbCtrl = m_pMainView->GetSelectedThumbCtrl();
	if(pThumbCtrl)
	{
		// 현재 스크롤 위치를 구한다.
		CPoint scrollPos = m_pMainView->GetScrollPosition();

		// 선택된 썸네일 컨트롤을 출력 썸네일 컨트롤 배열에서 제거한다.
		int idx = m_pMainView->GetSelectedThumbCtrlIndex();
		m_pMainView->m_arrOutputThumb.RemoveAt(idx);

		// 선택된 썸네일 컨트롤을 원본 썸네일 컨트롤 배열에서 제거하고 메모리를 해제한다.
		int id = pThumbCtrl->GetDlgCtrlID() - 1;
		m_pMainView->m_arrOriginThumb.RemoveAt(id);
		pThumbCtrl->DestroyWindow();
		delete pThumbCtrl;

		// 제거된 썸네일 컨트롤 이후의 컨트롤 ID들을 다시 설정한다.
		for(int i = id; i < m_pMainView->m_arrOriginThumb.GetSize(); i++)
			((CGALThumbCtrl*)m_pMainView->m_arrOriginThumb[i])->SetDlgCtrlID(i+1);

		// 출력 썸네일 컨트롤 배열을 갱신한다.
		m_pMainView->UpdateOutputThumbArray();

		// 제거된 썸네일 컨트롤이 마지막이 아닌 경우
		// 해당 인덱스로 대체되는 썸네일 컨트롤을 선택한다.
		if(!m_pMainView->m_arrOutputThumb.IsEmpty())
		{
			if(idx == m_pMainView->m_arrOutputThumb.GetSize())
				idx--;

			m_pMainView->SelectThumbCtrl(idx);
		}

		// 썸네일 컨트롤의 개수에 따라 스크롤을 설정한다.
		int height = m_pMainView->UpdateScroll();

		// 기존 위치로 스크롤한다.
		CRect rect;
		m_pMainView->GetClientRect(&rect);
		if(height > rect.Height())
			m_pMainView->ScrollToPosition(scrollPos);
	}
}


void CGALMenuView::OnBnClickedDeleteall()
{
	ASSERT(m_pMainView != NULL);

	// 원본 썸네일 컨트롤 배열이 비어있지 않은 경우, 모든 컨트롤을 제거한다.
	if(!m_pMainView->m_arrOriginThumb.IsEmpty())
	{
		CGALThumbCtrl* pThumbCtrl;
		for(int i = 0; i < m_pMainView->m_arrOriginThumb.GetSize(); i++)
		{
			pThumbCtrl = (CGALThumbCtrl*)m_pMainView->m_arrOriginThumb[i];
			pThumbCtrl->DestroyWindow();
			delete pThumbCtrl;
		}

		// 썸네일 컨트롤 배열을 비운다.
		m_pMainView->m_arrOriginThumb.RemoveAll();
		m_pMainView->m_arrOutputThumb.RemoveAll();

		// 썸네일 컨트롤이 존재하지 않으므로 스크롤을 제거한다.
		m_pMainView->UpdateScroll();
	}
}


void CGALMenuView::OnBnClickedExit()
{
	AfxGetMainWnd()->SendMessage(WM_CLOSE);
}
