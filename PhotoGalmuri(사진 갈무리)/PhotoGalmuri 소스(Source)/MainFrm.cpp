
// MainFrm.cpp : CMainFrame Ŭ������ ����
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

// CMainFrame ����/�Ҹ�

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
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	cs.style = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

	SetTitle("���� ������");  // Ÿ��Ʋ�� �����Ѵ�.
	cs.hMenu = NULL;          // �޴��� �����Ѵ�.

	// ������ ��ǥ�� ũ�⸦ �����Ѵ�.
	// �ػ� 1920x1080�� ȭ�鿡 ũ�⸦ �����.
	cs.x = 0;
	cs.y = 4;
	cs.cx = GetSystemMetrics(SM_CXSCREEN);
	cs.cy = GetSystemMetrics(SM_CYSCREEN) - 56;

	return TRUE;
}

// CMainFrame ����

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


// CMainFrame �޽��� ó����


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	if(!m_pSplitterWnd->CreateStatic(this, 1, 3))
	{
		AfxMessageBox("Failed to create static splitter");
		return FALSE;
	}

	// �並 ������ ������ �гο� ��ġ�Ѵ�.
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

	// ���� �並 �޴� ��� ����Ʈ�ɼ� �信 ����Ѵ�.
	CGALGalmuriView* pMainView = (CGALGalmuriView*)m_pSplitterWnd->GetPane(0, 1);
	((CGALMenuView*)m_pSplitterWnd->GetPane(0, 0))->m_pMainView = pMainView;
	((CGALListOptionView*)m_pSplitterWnd->GetPane(0, 2))->m_pMainView = pMainView;

	return TRUE;
}
