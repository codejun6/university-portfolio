// GALMenuView.cpp : ���� �����Դϴ�.
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


// CGALMenuView �����Դϴ�.

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


// CGALMenuView �޽��� ó�����Դϴ�.


void CGALMenuView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// �޴� ��Ʈ�ʹ�ư�� �����ϰ� �����Ѵ�.
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
	// ������ ����� ĥ�Ѵ�.
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(&rect, RGB(0, 0, 0));

	// �޴� ��ư ���̿� ��� ��輱�� �׸���.
	for(int i = 0; i < 5; i++)
	{
		pDC->FillSolidRect(0, 127 + (130 * i), 127, 3, RGB(255, 255, 255));
	}

	return TRUE;
}


void CGALMenuView::OnBnClickedOpen()
{
	ASSERT(m_pMainView != NULL);

	// ���� ���α׷��� �۾� ��θ� ��´�.
	char currPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, currPath);

	// ���͸��� ���� ���ڿ��� �ʱ�ȭ�Ѵ�.
	char filter[] = "��� �̹���(*.*)|*.jpg;*.png;*.bmp;*.gif|"
					"JPG �̹���(*.jpg)|*.jpg|"
					"PNG �̹���(*.png)|*.png|"
					"BMP �̹���(*.bmp)|*.bmp|"
					"GIF �̹���(*.gif)|*.gif||";

	// ���Ͽ��� ��ȭ���ڸ� �����ϰ� �����Ѵ�.
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_ALLOWMULTISELECT, filter);
	fileDlg.m_ofn.lpstrInitialDir = currPath;

	// ���Ͽ��� ��ȭ���ڸ� ����.
	// ���� ��ư�� ���� ��쿡�� �̹����� �ҷ��´�.
	if(fileDlg.DoModal() == IDOK)
	{
		CStringArray failPathArr;

		// �ϳ� �Ǵ� �ټ��� �̹����� �ҷ��� ����� ��Ʈ���� ������ ���� ����� ��Ʈ�� �迭�� �߰��Ѵ�.
		for(POSITION pos = fileDlg.GetStartPosition(); pos != NULL;)
		{
			int thumbCount = m_pMainView->m_arrOriginThumb.GetSize();

			// �̹� �����ϴ� �����̸� �߰����� �ʴ´�.
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

			// �� ���� �ε����� ������ ���Ѵ�.
			CRect rect;
			m_pMainView->GetThumbCtrlRect(rect, thumbCount);

			// ����� ��Ʈ���� ������ ���� ����� ��Ʈ�� �迭�� �߰��Ѵ�.
			CGALThumbCtrl* pThumbCtrl = new CGALThumbCtrl(pathName);
			pThumbCtrl->Create("CGALThumbCtrl", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW,
					rect, m_pMainView, thumbCount + 1);
			m_pMainView->m_arrOriginThumb.Add(pThumbCtrl);
		}

		// ��� ����� ��Ʈ�� �迭�� ������ �׸��� �����ϰ� �����Ѵ�.
		m_pMainView->m_arrOutputThumb.RemoveAll();
		m_pMainView->m_arrOutputThumb.Copy(m_pMainView->m_arrOriginThumb);
		m_pMainView->UpdateOutputThumbArray();

		// ���������� �߰��� ����� ��Ʈ���� �����Ѵ�.
		m_pMainView->SelectThumbCtrl(m_pMainView->m_arrOutputThumb.GetSize() - 1);

		// ����� ��Ʈ���� ������ ���� ��ũ���� �����Ѵ�.
		// (����� ��Ʈ���� ȭ���� �Ѿ�ٸ�, ��ũ���� �ø���.)
		m_pMainView->UpdateScroll();

		// ��ũ���� ����ٸ� �� ��ġ�� ��ũ���Ѵ�.
		if(m_pMainView->m_arrOutputThumb.GetSize() > m_pMainView->m_nColCount * 2)
			m_pMainView->ScrollToPosition(CPoint(0, m_pMainView->GetScrollLimit(SB_VERT)));

		// �˻� �� ���� �ɼ��� �ʱ�ȭ�Ѵ�.
		CGALSplitterWnd* pSplitterWnd = ((CMainFrame*)AfxGetMainWnd())->m_pSplitterWnd;
		((CGALListOptionView*)pSplitterWnd->GetPane(0, 2))->InitListOption();

		// �ߺ� ������ ���, �޽��� �ڽ��� �����ش�.
		if(!failPathArr.IsEmpty())
		{
			CString str = "�ش� ������ �̹� �߰��� �����Դϴ� :";
			for(int i = 0; i < failPathArr.GetSize(); i++)
				str.Append("\n" + failPathArr[i]);
			AfxMessageBox(str);
		}
	}
}


void CGALMenuView::OnBnClickedEdit()
{
	ASSERT(m_pMainView != NULL);

	// ���õ� ����� ��Ʈ���� �ִ� ���
	CGALThumbCtrl* pThumbCtrl = m_pMainView->GetSelectedThumbCtrl();
	if(pThumbCtrl)
	{
		// ���� ��ȭ���ڸ� ���� ������ �Ѵ�.
		CGALEditDialog dlg;
		dlg.SetOriginThumb(pThumbCtrl);
		dlg.DoModal();

		pThumbCtrl->Invalidate();
	}
}


void CGALMenuView::OnBnClickedDelete()
{
	ASSERT(m_pMainView != NULL);

	// ���õ� ����� ��Ʈ���� �ִ� ���
	CGALThumbCtrl* pThumbCtrl = m_pMainView->GetSelectedThumbCtrl();
	if(pThumbCtrl)
	{
		// ���� ��ũ�� ��ġ�� ���Ѵ�.
		CPoint scrollPos = m_pMainView->GetScrollPosition();

		// ���õ� ����� ��Ʈ���� ��� ����� ��Ʈ�� �迭���� �����Ѵ�.
		int idx = m_pMainView->GetSelectedThumbCtrlIndex();
		m_pMainView->m_arrOutputThumb.RemoveAt(idx);

		// ���õ� ����� ��Ʈ���� ���� ����� ��Ʈ�� �迭���� �����ϰ� �޸𸮸� �����Ѵ�.
		int id = pThumbCtrl->GetDlgCtrlID() - 1;
		m_pMainView->m_arrOriginThumb.RemoveAt(id);
		pThumbCtrl->DestroyWindow();
		delete pThumbCtrl;

		// ���ŵ� ����� ��Ʈ�� ������ ��Ʈ�� ID���� �ٽ� �����Ѵ�.
		for(int i = id; i < m_pMainView->m_arrOriginThumb.GetSize(); i++)
			((CGALThumbCtrl*)m_pMainView->m_arrOriginThumb[i])->SetDlgCtrlID(i+1);

		// ��� ����� ��Ʈ�� �迭�� �����Ѵ�.
		m_pMainView->UpdateOutputThumbArray();

		// ���ŵ� ����� ��Ʈ���� �������� �ƴ� ���
		// �ش� �ε����� ��ü�Ǵ� ����� ��Ʈ���� �����Ѵ�.
		if(!m_pMainView->m_arrOutputThumb.IsEmpty())
		{
			if(idx == m_pMainView->m_arrOutputThumb.GetSize())
				idx--;

			m_pMainView->SelectThumbCtrl(idx);
		}

		// ����� ��Ʈ���� ������ ���� ��ũ���� �����Ѵ�.
		int height = m_pMainView->UpdateScroll();

		// ���� ��ġ�� ��ũ���Ѵ�.
		CRect rect;
		m_pMainView->GetClientRect(&rect);
		if(height > rect.Height())
			m_pMainView->ScrollToPosition(scrollPos);
	}
}


void CGALMenuView::OnBnClickedDeleteall()
{
	ASSERT(m_pMainView != NULL);

	// ���� ����� ��Ʈ�� �迭�� ������� ���� ���, ��� ��Ʈ���� �����Ѵ�.
	if(!m_pMainView->m_arrOriginThumb.IsEmpty())
	{
		CGALThumbCtrl* pThumbCtrl;
		for(int i = 0; i < m_pMainView->m_arrOriginThumb.GetSize(); i++)
		{
			pThumbCtrl = (CGALThumbCtrl*)m_pMainView->m_arrOriginThumb[i];
			pThumbCtrl->DestroyWindow();
			delete pThumbCtrl;
		}

		// ����� ��Ʈ�� �迭�� ����.
		m_pMainView->m_arrOriginThumb.RemoveAll();
		m_pMainView->m_arrOutputThumb.RemoveAll();

		// ����� ��Ʈ���� �������� �����Ƿ� ��ũ���� �����Ѵ�.
		m_pMainView->UpdateScroll();
	}
}


void CGALMenuView::OnBnClickedExit()
{
	AfxGetMainWnd()->SendMessage(WM_CLOSE);
}
