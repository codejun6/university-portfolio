
// GALGalmuriDoc.cpp : CGALGalmuriDoc Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
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


// CGALGalmuriDoc ����/�Ҹ�

CGALGalmuriDoc::CGALGalmuriDoc()
{
	// TODO: ���⿡ ��ȸ�� ���� �ڵ带 �߰��մϴ�.
}

CGALGalmuriDoc::~CGALGalmuriDoc()
{
}

BOOL CGALGalmuriDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// ���� �� ������ ������ �о� �̹��� ����� �ҷ��´�.
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
	// ���� �κ�
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
	// �ε� �κ�
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

// ����� �׸��� �����մϴ�.
void CGALGalmuriDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// ������ �����͸� �׸����� �� �ڵ带 �����Ͻʽÿ�.
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

// �˻� ó���⸦ �����մϴ�.
void CGALGalmuriDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ������ �����Ϳ��� �˻� �������� �����մϴ�.
	// ������ �κ��� ";"�� ���еǾ�� �մϴ�.

	// ��: strSearchContent = _T("point;rectangle;circle;ole object;");
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

// CGALGalmuriDoc ����

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


// CGALGalmuriDoc ���
