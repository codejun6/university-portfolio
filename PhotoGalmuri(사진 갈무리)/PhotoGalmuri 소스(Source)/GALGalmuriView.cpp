
// GALGalmuriView.cpp : CGALGalmuriView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "PhotoGalmuri.h"
#endif

#include "GALGalmuriDoc.h"
#include "GALGalmuriView.h"
#include "GALThumbCtrl.h"
#include "GALImageFeature.h"
#include "GALEditDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGALGalmuriView

IMPLEMENT_DYNCREATE(CGALGalmuriView, CScrollView)

BEGIN_MESSAGE_MAP(CGALGalmuriView, CScrollView)
	ON_WM_LBUTTONDOWN()
	ON_WM_DESTROY()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

// CGALGalmuriView ����/�Ҹ�

CGALGalmuriView::CGALGalmuriView()
{

}

CGALGalmuriView::~CGALGalmuriView()
{
	// ���� ����� ��Ʈ�� �迭�� �����Ѵ�.
	// (��� ����� ��Ʈ�� �迭�� ���� �׸��� �����͸����� �����Ǿ� �ֱ� ������ �޸𸮸� ������ �ʿ䰡 ����.)
	for(int i = 0; i < m_arrOriginThumb.GetSize(); i++)
	{
		if(m_arrOriginThumb[i])
			delete (CGALThumbCtrl*)m_arrOriginThumb[i];
	}
}

BOOL CGALGalmuriView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.
	//cs.style |= WS_VSCROLL;

	return CView::PreCreateWindow(cs);
}

void CGALGalmuriView::GetThumbCtrlRect(CRect& rect, int index)
{
	int row = index / m_nColCount;  // ��
	int col = index % m_nColCount;  // ��

	// �ε����� �ش��ϴ� ������ �����Ѵ�.
	rect.SetRect(m_nMargin + (m_nColGap * col), m_nMargin + (m_nRowGap * row), 
			343 + (m_nColGap * col), 343 + (m_nRowGap * row));
}

CGALThumbCtrl* CGALGalmuriView::GetThumbCtrlByCtrlID(int id)
{
	// ��Ʈ�� ID�� ��ġ�ϴ� ����� ��Ʈ���� ��ȯ�Ѵ�.
	CGALThumbCtrl* pThumbCtrl;
	for(int i = 0; i < m_arrOriginThumb.GetSize(); i++)
	{
		pThumbCtrl = (CGALThumbCtrl*)m_arrOriginThumb[i];
		if(pThumbCtrl->GetDlgCtrlID() == id)
			return pThumbCtrl;
	}

	// ��ġ�ϴ� ���� ���ٸ� NULL�� ��ȯ�Ѵ�.
	return NULL;
}

CGALThumbCtrl* CGALGalmuriView::GetSelectedThumbCtrl()
{
	// ���õ� ����� ��Ʈ���� ��ȯ�Ѵ�.
	CGALThumbCtrl* pThumbCtrl;
	for(int i = 0; i < m_arrOutputThumb.GetSize(); i++)
	{
		pThumbCtrl = (CGALThumbCtrl*)m_arrOutputThumb[i];
		if(pThumbCtrl->IsSelected())
			return pThumbCtrl;
	}

	// ���õ� ���� ���ٸ� NULL�� ��ȯ�Ѵ�.
	return NULL;
}

int CGALGalmuriView::GetSelectedThumbCtrlIndex()
{
	// ���õ� ����� ��Ʈ���� �ε����� ��ȯ�Ѵ�.
	CGALThumbCtrl* pThumbCtrl;
	for(int i = 0; i < m_arrOutputThumb.GetSize(); i++)
	{
		pThumbCtrl = (CGALThumbCtrl*)m_arrOutputThumb[i];
		if(pThumbCtrl->IsSelected())
			return i;
	}

	// ���õ� ���� ���ٸ� -1�� ��ȯ�Ѵ�.
	return -1;
}

void CGALGalmuriView::SelectThumbCtrl(int index)
{
	// ��� ����� ��Ʈ���� �����ϴ� ���
	if(!m_arrOutputThumb.IsEmpty())
	{
		// ���õ� ����� ��Ʈ���� ������ �����Ѵ�.
		// (���� ������ ������� �ʰ�, �ϳ��� ���ø� �����Ѵ�.)
		CGALThumbCtrl* pThumbCtrl;
		for(int i = 0; i < m_arrOutputThumb.GetSize(); i++)
		{
			pThumbCtrl = (CGALThumbCtrl*)m_arrOutputThumb[i];
			if(pThumbCtrl->IsSelected() && i != index)
			{
				pThumbCtrl->SetSelected(FALSE);
				pThumbCtrl->Invalidate();
			}
		}

		// �ش� �ε����� ����� ��Ʈ���� �����Ѵ�.
		pThumbCtrl = (CGALThumbCtrl*)m_arrOutputThumb[index];
		if(!pThumbCtrl->IsSelected())
		{
			pThumbCtrl->SetSelected(TRUE);
			pThumbCtrl->Invalidate();
		}
	}
}

void CGALGalmuriView::UpdateOutputThumbArray()
{
	// �ùٸ� ������ ���ϱ� ���� ���� ��ġ�� ��ũ���Ѵ�.
	// (�������� ���ϴ� ��ġ�� ��ũ���ؾ� �Ѵ�.)
	ScrollToPosition(CPoint(0, 0));

	// �Ϻ� ����� ��Ʈ�Ѹ� ����ϱ� ���� �ϴ� ��� ����� ��Ʈ�ѵ��� �����.
	CGALThumbCtrl* pThumbCtrl;
	for(int i = 0; i < m_arrOriginThumb.GetSize(); i++)
	{
		pThumbCtrl = (CGALThumbCtrl*)m_arrOriginThumb[i];
		if(pThumbCtrl->IsWindowVisible())
			pThumbCtrl->ShowWindow(SW_HIDE);
	}

	// ��� ����� ��Ʈ�� �迭�� ���� ��Ʈ�ѵ��� ��ġ�� �����ϰ� �����ش�.
	CRect rect;
	for(int i = 0; i < m_arrOutputThumb.GetSize(); i++)
	{
		GetThumbCtrlRect(rect, i);
		pThumbCtrl = (CGALThumbCtrl*)m_arrOutputThumb[i];
		pThumbCtrl->MoveWindow(&rect);
		if(!pThumbCtrl->IsWindowVisible())
			pThumbCtrl->ShowWindow(SW_SHOW);
	}
}

int CGALGalmuriView::UpdateScroll()
{
	// ��� ����� ��Ʈ���� ������ ���� ��ũ���� �����Ѵ�.
	int rows = ((m_arrOutputThumb.GetSize() - 1) / m_nColCount) + 1;
	int height = m_nMargin + (m_nRowGap * rows);
	CRect rect;
	GetClientRect(&rect);

	// ����� ����� ��Ʈ���� ȭ���� �Ѿ�ٸ�, ��ũ���� �ø���.
	// �Ѿ�� �ʴ´ٸ�, ��ũ���� ���ŵǱ� ������ ���� ��ġ�� ��ũ���Ѵ�.
	if(height > rect.Height())
	{
		rect.bottom = height;
	}
	else
	{
		ScrollToPosition(CPoint(0, 0));
	}
	rect.right = rect.Width() - GetSystemMetrics(SM_CXVSCROLL) - 1;
	SetScrollSizes(MM_TEXT, rect.Size());

	// ���� �� ��ü ������ ���̸� ��ȯ�Ѵ�.
	return rect.Height();
}

// CGALGalmuriView �׸���

void CGALGalmuriView::OnDraw(CDC* pDC)
{
	CGALGalmuriDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
}


// CGALGalmuriView ����

#ifdef _DEBUG
void CGALGalmuriView::AssertValid() const
{
	CView::AssertValid();
}

void CGALGalmuriView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGALGalmuriDoc* CGALGalmuriView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGALGalmuriDoc)));
	return (CGALGalmuriDoc*)m_pDocument;
}
#endif //_DEBUG


// CGALGalmuriView �޽��� ó����


BOOL CGALGalmuriView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// ����� ��Ʈ���� ID�� �ش��ϴ� ���
	int id = LOWORD(wParam);
	if(id > 0 && id <= m_arrOriginThumb.GetSize())
	{
		CGALThumbCtrl* pThumbCtrl = GetThumbCtrlByCtrlID(id);
		CGALThumbCtrl* pSelThumbCtrl = GetSelectedThumbCtrl();

		switch(HIWORD(wParam))
		{
		// ����Ŭ�� �����޽����� ���
		case BN_DBLCLK:
			if(pThumbCtrl)
			{
				// ���� ��ȭ���ڸ� ���� ������ �Ѵ�.
				CGALEditDialog dlg;
				dlg.SetOriginThumb(pThumbCtrl);
				dlg.DoModal();

				pThumbCtrl->Invalidate();
			}
			break;

		// Ŭ�� �����޽����� ���
		case BN_CLICKED:
			// �ٸ� ����� ��Ʈ���� �����߰ų� � �͵� �������� �ʾҴ� ���
			// (���� ����� ��Ʈ���� �ٽ� ������ ���, ������ �����Ѵ�.)
			if(pThumbCtrl != pSelThumbCtrl)
			{
				// ���õ� ����� ��Ʈ���� ������ �����Ѵ�.
				// (���� ������ ������� �ʰ�, �ϳ��� ���ø� �����Ѵ�.)
				if(pSelThumbCtrl)
				{
					pSelThumbCtrl->SetSelected(FALSE);
					pSelThumbCtrl->Invalidate();
				}

				// �ش� ����� ��Ʈ���� �����Ѵ�.
				pThumbCtrl->SetSelected(TRUE);
				pThumbCtrl->Invalidate();
			}
			break;
		}
	}

	return CView::OnCommand(wParam, lParam);
}


void CGALGalmuriView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	// �ʱ� �� ��üũ�⸦ Ŭ���̾�Ʈ ������ ũ��� �����Ѵ�.
	CRect rect;
	GetClientRect(&rect);
	SetScrollSizes(MM_TEXT, rect.Size());

	CStringArray failPathArr;

	// ��ť��Ʈ���� �����͸� �ҷ��� ���� ����� ��Ʈ�� �迭�� �����Ѵ�.
	CGALGalmuriDoc* pDoc = GetDocument();
	for(int i = 0; i < pDoc->m_arrFeature.GetSize(); i++)
	{
		CGALImageFeature* pFeature = (CGALImageFeature*)pDoc->m_arrFeature[i];
		CGALThumbCtrl* pThumbCtrl = new CGALThumbCtrl();
		pThumbCtrl->LoadOriginImage(pFeature->m_strFilePath);
		pThumbCtrl->SetImageFeature(pFeature);

		// �̹����� �ҷ����� ���ߴٸ� ����� ��Ʈ���� �������� �ʴ´�.
		if(pThumbCtrl->IsNull())
		{
			failPathArr.Add(pFeature->m_strFilePath);
			pDoc->m_arrFeature.RemoveAt(i);
			delete pFeature;
			i--;
			continue;
		}

		CRect thumbRect;
		GetThumbCtrlRect(thumbRect, i);
		pThumbCtrl->Create("CGALThumbCtrl", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW,
				thumbRect, this, i + 1);
		m_arrOriginThumb.Add(pThumbCtrl);
	}

	// ������ �׸��� ��� ����� ��Ʈ�� �迭�� �����ϰ� ������ ȭ�鿡 ����Ѵ�.
	m_arrOutputThumb.Copy(m_arrOriginThumb);
	UpdateOutputThumbArray();
	SelectThumbCtrl(0);
	UpdateScroll();

	// ���� ������ ���, �޽��� �ڽ��� �����ش�.
	if(!failPathArr.IsEmpty())
	{
		CString str = "�ش� ������ ���� �����Դϴ� :";
		for(int i = 0; i < failPathArr.GetSize(); i++)
			str.Append("\n" + failPathArr[i]);
		AfxMessageBox(str);
	}
}


void CGALGalmuriView::OnDestroy()
{
	CScrollView::OnDestroy();

	// ������ ������ ������ �̹��� ��ϰ� Ư¡ ���� �����͸� �����Ѵ�.
	CFile file;
	if(file.Open("phogaldata.dat", CFile::modeCreate | CFile::modeWrite))
	{
		CArchive ar(&file, CArchive::store);

		CGALGalmuriDoc* pDoc = GetDocument();
		pDoc->m_arrFeature.RemoveAll();
		for(int i = 0; i < m_arrOriginThumb.GetSize(); i++)
		{
			CGALThumbCtrl* pThumbCtrl = (CGALThumbCtrl*)m_arrOriginThumb[i];
			pDoc->m_arrFeature.Add(pThumbCtrl->GetImageFeature());
		}
		pDoc->Serialize(ar);

		ar.Close();
		file.Close();
	}
}


void CGALGalmuriView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// ���õǾ� �ִ� ����� ��Ʈ���� �ִ� ���, �ش� ����� ��Ʈ���� ������ �����Ѵ�.
	CGALThumbCtrl* pSelThumbCtrl = GetSelectedThumbCtrl();
	if(pSelThumbCtrl)
	{
		pSelThumbCtrl->SetSelected(FALSE);
		pSelThumbCtrl->Invalidate();
	}

	CView::OnLButtonDown(nFlags, point);
}


BOOL CGALGalmuriView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// ���콺 ���� �̿��� ȭ���� ��ũ���Ѵ�.
	if(zDelta > 0)
		SendMessage(WM_VSCROLL, SB_LINEUP, 0); 
	else
		SendMessage(WM_VSCROLL, SB_LINEDOWN, 0); 

	return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}
