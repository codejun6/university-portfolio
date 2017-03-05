
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "PhotoGalmuri.h"

#include "MainFrm.h"
#include "GALSplitterWnd.h"
#include "GALGalmuriView.h"
#include "GALMenuView.h"
#include "GALListOptionView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
END_MESSAGE_MAP()

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	m_pSplitterWnd = new CGALSplitterWnd();
}

CMainFrame::~CMainFrame()
{
	if(m_pSplitterWnd) 
		delete m_pSplitterWnd;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	cs.style = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

	SetTitle("사진 갈무리");  // 타이틀을 설정한다.
	cs.hMenu = NULL;          // 메뉴를 제거한다.

	// 윈도우 좌표와 크기를 설정한다.
	// 해상도 1920x1080의 화면에 크기를 맞춘다.
	cs.x = 0;
	cs.y = 4;
	cs.cx = GetSystemMetrics(SM_CXSCREEN);
	cs.cy = GetSystemMetrics(SM_CYSCREEN) - 56;

	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	if(!m_pSplitterWnd->CreateStatic(this, 1, 3))
	{
		AfxMessageBox("Failed to create static splitter");
		return FALSE;
	}

	// 뷰를 생성해 분할한 패널에 배치한다.
	if(!m_pSplitterWnd->CreateView(0, 0, RUNTIME_CLASS(CGALMenuView), CSize(127, 0), pContext))
	{
		AfxMessageBox("Failed to create GALMenuView pane");
		return FALSE;
	}
	if(!m_pSplitterWnd->CreateView(0, 1, RUNTIME_CLASS(CGALGalmuriView), CSize(1430, 0), pContext))
	{
		AfxMessageBox("Failed to create GALGalmuriView pane");
		return FALSE;
	}
	if(!m_pSplitterWnd->CreateView(0, 2, RUNTIME_CLASS(CGALListOptionView), CSize(0, 0), pContext))
	{
		AfxMessageBox("Failed to create GALListOptionView pane");
		return FALSE;
	}

	// 메인 뷰를 메뉴 뷰와 리스트옵션 뷰에 등록한다.
	CGALGalmuriView* pMainView = (CGALGalmuriView*)m_pSplitterWnd->GetPane(0, 1);
	((CGALMenuView*)m_pSplitterWnd->GetPane(0, 0))->m_pMainView = pMainView;
	((CGALListOptionView*)m_pSplitterWnd->GetPane(0, 2))->m_pMainView = pMainView;

	return TRUE;
}
