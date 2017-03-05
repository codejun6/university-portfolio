
// GALGalmuriDoc.cpp : CGALGalmuriDoc 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "PhotoGalmuri.h"
#endif

#include "GALGalmuriDoc.h"
#include "GALImageFeature.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CGALGalmuriDoc

IMPLEMENT_DYNCREATE(CGALGalmuriDoc, CDocument)

BEGIN_MESSAGE_MAP(CGALGalmuriDoc, CDocument)
END_MESSAGE_MAP()


// CGALGalmuriDoc 생성/소멸

CGALGalmuriDoc::CGALGalmuriDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.
}

CGALGalmuriDoc::~CGALGalmuriDoc()
{
}

BOOL CGALGalmuriDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// 시작 시 데이터 파일을 읽어 이미지 목록을 불러온다.
	CFile file;
	if(file.Open("phogaldata.dat", CFile::modeRead))
	{
		CArchive ar(&file, CArchive::load);
		Serialize(ar);

		ar.Close();
		file.Close();
	}

	return TRUE;
}




// CGALGalmuriDoc serialization

void CGALGalmuriDoc::Serialize(CArchive& ar)
{
	// 저장 부분
	if (ar.IsStoring())
	{
		ar << m_arrFeature.GetSize();
		for(int i = 0; i < m_arrFeature.GetSize(); i++)
		{
			CGALImageFeature* pFeature = (CGALImageFeature*)m_arrFeature[i];
			pFeature->Serialize(ar);
			delete pFeature;
		}
	}
	// 로딩 부분
	else
	{
		int count = 0;
		ar >> count;
		for(int i = 0; i < count; i++)
		{
			CGALImageFeature* pFeature = new CGALImageFeature();
			pFeature->Serialize(ar);
			m_arrFeature.Add(pFeature);
		}
	}
}

#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CGALGalmuriDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 검색 처리기를 지원합니다.
void CGALGalmuriDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CGALGalmuriDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CGALGalmuriDoc 진단

#ifdef _DEBUG
void CGALGalmuriDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGALGalmuriDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CGALGalmuriDoc 명령
