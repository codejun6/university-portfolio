
// GALGalmuriView.cpp : CGALGalmuriView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
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

// CGALGalmuriView 생성/소멸

CGALGalmuriView::CGALGalmuriView()
{

}

CGALGalmuriView::~CGALGalmuriView()
{
	// 원본 썸네일 컨트롤 배열을 제거한다.
	// (출력 썸네일 컨트롤 배열은 원본 항목의 포인터만으로 구성되어 있기 때문에 메모리를 해제할 필요가 없다.)
	for(int i = 0; i < m_arrOriginThumb.GetSize(); i++)
	{
		if(m_arrOriginThumb[i])
			delete (CGALThumbCtrl*)m_arrOriginThumb[i];
	}
}

BOOL CGALGalmuriView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	//cs.style |= WS_VSCROLL;

	return CView::PreCreateWindow(cs);
}

void CGALGalmuriView::GetThumbCtrlRect(CRect& rect, int index)
{
	int row = index / m_nColCount;  // 행
	int col = index % m_nColCount;  // 열

	// 인덱스에 해당하는 영역을 설정한다.
	rect.SetRect(m_nMargin + (m_nColGap * col), m_nMargin + (m_nRowGap * row), 
			343 + (m_nColGap * col), 343 + (m_nRowGap * row));
}

CGALThumbCtrl* CGALGalmuriView::GetThumbCtrlByCtrlID(int id)
{
	// 컨트롤 ID가 일치하는 썸네일 컨트롤을 반환한다.
	CGALThumbCtrl* pThumbCtrl;
	for(int i = 0; i < m_arrOriginThumb.GetSize(); i++)
	{
		pThumbCtrl = (CGALThumbCtrl*)m_arrOriginThumb[i];
		if(pThumbCtrl->GetDlgCtrlID() == id)
			return pThumbCtrl;
	}

	// 일치하는 것이 없다면 NULL을 반환한다.
	return NULL;
}

CGALThumbCtrl* CGALGalmuriView::GetSelectedThumbCtrl()
{
	// 선택된 썸네일 컨트롤을 반환한다.
	CGALThumbCtrl* pThumbCtrl;
	for(int i = 0; i < m_arrOutputThumb.GetSize(); i++)
	{
		pThumbCtrl = (CGALThumbCtrl*)m_arrOutputThumb[i];
		if(pThumbCtrl->IsSelected())
			return pThumbCtrl;
	}

	// 선택된 것이 없다면 NULL을 반환한다.
	return NULL;
}

int CGALGalmuriView::GetSelectedThumbCtrlIndex()
{
	// 선택된 썸네일 컨트롤의 인덱스를 반환한다.
	CGALThumbCtrl* pThumbCtrl;
	for(int i = 0; i < m_arrOutputThumb.GetSize(); i++)
	{
		pThumbCtrl = (CGALThumbCtrl*)m_arrOutputThumb[i];
		if(pThumbCtrl->IsSelected())
			return i;
	}

	// 선택된 것이 없다면 -1을 반환한다.
	return -1;
}

void CGALGalmuriView::SelectThumbCtrl(int index)
{
	// 출력 썸네일 컨트롤이 존재하는 경우
	if(!m_arrOutputThumb.IsEmpty())
	{
		// 선택된 썸네일 컨트롤의 선택을 해제한다.
		// (다중 선택을 허용하지 않고, 하나의 선택만 유지한다.)
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

		// 해당 인덱스의 썸네일 컨트롤을 선택한다.
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
	// 올바른 영역을 구하기 위해 시작 위치로 스크롤한다.
	// (마지막에 원하는 위치로 스크롤해야 한다.)
	ScrollToPosition(CPoint(0, 0));

	// 일부 썸네일 컨트롤만 출력하기 위해 일단 모든 썸네일 컨트롤들을 숨긴다.
	CGALThumbCtrl* pThumbCtrl;
	for(int i = 0; i < m_arrOriginThumb.GetSize(); i++)
	{
		pThumbCtrl = (CGALThumbCtrl*)m_arrOriginThumb[i];
		if(pThumbCtrl->IsWindowVisible())
			pThumbCtrl->ShowWindow(SW_HIDE);
	}

	// 출력 썸네일 컨트롤 배열에 맞춰 컨트롤들의 위치를 조절하고 보여준다.
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
	// 출력 썸네일 컨트롤의 개수에 따라 스크롤을 설정한다.
	int rows = ((m_arrOutputThumb.GetSize() - 1) / m_nColCount) + 1;
	int height = m_nMargin + (m_nRowGap * rows);
	CRect rect;
	GetClientRect(&rect);

	// 출력할 썸네일 컨트롤이 화면을 넘어간다면, 스크롤을 늘린다.
	// 넘어가지 않는다면, 스크롤이 제거되기 때문에 시작 위치로 스크롤한다.
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

	// 메인 뷰 전체 영역의 높이를 반환한다.
	return rect.Height();
}

// CGALGalmuriView 그리기

void CGALGalmuriView::OnDraw(CDC* pDC)
{
	CGALGalmuriDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
}


// CGALGalmuriView 진단

#ifdef _DEBUG
void CGALGalmuriView::AssertValid() const
{
	CView::AssertValid();
}

void CGALGalmuriView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGALGalmuriDoc* CGALGalmuriView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGALGalmuriDoc)));
	return (CGALGalmuriDoc*)m_pDocument;
}
#endif //_DEBUG


// CGALGalmuriView 메시지 처리기


BOOL CGALGalmuriView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// 썸네일 컨트롤의 ID에 해당하는 경우
	int id = LOWORD(wParam);
	if(id > 0 && id <= m_arrOriginThumb.GetSize())
	{
		CGALThumbCtrl* pThumbCtrl = GetThumbCtrlByCtrlID(id);
		CGALThumbCtrl* pSelThumbCtrl = GetSelectedThumbCtrl();

		switch(HIWORD(wParam))
		{
		// 더블클릭 통지메시지의 경우
		case BN_DBLCLK:
			if(pThumbCtrl)
			{
				// 편집 대화상자를 열어 편집을 한다.
				CGALEditDialog dlg;
				dlg.SetOriginThumb(pThumbCtrl);
				dlg.DoModal();

				pThumbCtrl->Invalidate();
			}
			break;

		// 클릭 통지메시지의 경우
		case BN_CLICKED:
			// 다른 썸네일 컨트롤을 선택했거나 어떤 것도 선택하지 않았던 경우
			// (현재 썸네일 컨트롤을 다시 선택한 경우, 선택을 유지한다.)
			if(pThumbCtrl != pSelThumbCtrl)
			{
				// 선택된 썸네일 컨트롤의 선택을 해제한다.
				// (다중 선택을 허용하지 않고, 하나의 선택만 유지한다.)
				if(pSelThumbCtrl)
				{
					pSelThumbCtrl->SetSelected(FALSE);
					pSelThumbCtrl->Invalidate();
				}

				// 해당 썸네일 컨트롤을 선택한다.
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

	// 초기 뷰 전체크기를 클라이언트 영역의 크기로 설정한다.
	CRect rect;
	GetClientRect(&rect);
	SetScrollSizes(MM_TEXT, rect.Size());

	CStringArray failPathArr;

	// 도큐먼트에서 데이터를 불러와 원본 썸네일 컨트롤 배열을 구성한다.
	CGALGalmuriDoc* pDoc = GetDocument();
	for(int i = 0; i < pDoc->m_arrFeature.GetSize(); i++)
	{
		CGALImageFeature* pFeature = (CGALImageFeature*)pDoc->m_arrFeature[i];
		CGALThumbCtrl* pThumbCtrl = new CGALThumbCtrl();
		pThumbCtrl->LoadOriginImage(pFeature->m_strFilePath);
		pThumbCtrl->SetImageFeature(pFeature);

		// 이미지를 불러오지 못했다면 썸네일 컨트롤을 생성하지 않는다.
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

	// 원본의 항목을 출력 썸네일 컨트롤 배열에 복사하고 갱신해 화면에 출력한다.
	m_arrOutputThumb.Copy(m_arrOriginThumb);
	UpdateOutputThumbArray();
	SelectThumbCtrl(0);
	UpdateScroll();

	// 없는 파일인 경우, 메시지 박스를 보여준다.
	if(!failPathArr.IsEmpty())
	{
		CString str = "해당 사진은 없는 사진입니다 :";
		for(int i = 0; i < failPathArr.GetSize(); i++)
			str.Append("\n" + failPathArr[i]);
		AfxMessageBox(str);
	}
}


void CGALGalmuriView::OnDestroy()
{
	CScrollView::OnDestroy();

	// 데이터 파일을 생성해 이미지 목록과 특징 정보 데이터를 저장한다.
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
	// 선택되어 있는 썸네일 컨트롤이 있는 경우, 해당 썸네일 컨트롤의 선택을 해제한다.
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
	// 마우스 휠을 이용해 화면을 스크롤한다.
	if(zDelta > 0)
		SendMessage(WM_VSCROLL, SB_LINEUP, 0); 
	else
		SendMessage(WM_VSCROLL, SB_LINEDOWN, 0); 

	return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}
