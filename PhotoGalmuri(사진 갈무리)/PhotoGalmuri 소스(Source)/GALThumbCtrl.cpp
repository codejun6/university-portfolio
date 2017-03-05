// GALThumbCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PhotoGalmuri.h"
#include "GALThumbCtrl.h"
#include "GALImageFeature.h"


// CGALThumbCtrl

IMPLEMENT_DYNAMIC(CGALThumbCtrl, CButton)

CGALThumbCtrl::CGALThumbCtrl()
	: m_pImageFeature(NULL)
	, m_bIsSelected(FALSE)
{
}

CGALThumbCtrl::CGALThumbCtrl(const CString& filePath)
	: m_bIsSelected(FALSE)
{
	if(PathFileExists(filePath))
	{
		LoadOriginImage(filePath);
		m_pImageFeature = new CGALImageFeature(filePath, m_imgOrigin);
	}
}

CGALThumbCtrl::~CGALThumbCtrl()
{
	if(!m_imgOrigin.IsNull()) 
		m_imgOrigin.Destroy();
	if(m_pImageFeature)
		delete m_pImageFeature;
}

BOOL CGALThumbCtrl::IsNull()
{
	return m_imgOrigin.IsNull();
}

void CGALThumbCtrl::SetSelected(BOOL isSelected)
{
	m_bIsSelected = isSelected;
}

BOOL CGALThumbCtrl::IsSelected()
{
	return m_bIsSelected;
}

void CGALThumbCtrl::LoadOriginImage(const CString& filePath)
{
	if(!m_imgOrigin.IsNull())
		m_imgOrigin.Destroy();

	m_imgOrigin.Load(filePath);
}

CImage* CGALThumbCtrl::GetOriginImagePtr()
{
	return &m_imgOrigin;
}

void CGALThumbCtrl::SetImageFeature(CGALImageFeature* pFeature)
{
	m_pImageFeature= pFeature;
}

CGALImageFeature* CGALThumbCtrl::GetImageFeature()
{
	if(m_imgOrigin.IsNull())
		return NULL;

	return m_pImageFeature;
}


BEGIN_MESSAGE_MAP(CGALThumbCtrl, CButton)
END_MESSAGE_MAP()



// CGALThumbCtrl 메시지 처리기입니다.




void CGALThumbCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	ASSERT(lpDrawItemStruct->CtlType == ODT_BUTTON);

	// 썸네일 이미지가 존재하지 않으면 썸네일 컨트롤을 그리지 않는다.
	if(!m_imgOrigin.IsNull())
	{
		CRect clientRect;
		GetClientRect(&clientRect);

		CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

		// 더블 버퍼링을 위한 메모리DC를 준비한다.
		CDC memDC;
		memDC.CreateCompatibleDC(pDC);
		SetStretchBltMode(memDC, COLORONCOLOR);
		CBitmap memBitmap;
		memBitmap.CreateCompatibleBitmap(pDC, clientRect.Width(), clientRect.Height());
		CBitmap* pOldBitmap = memDC.SelectObject(&memBitmap);
		
		// 메모리DC의 배경을 칠한다.
		// 선택되어 있지 않다면, 흰색으로, 
		// 선택되어 있다면, 검은색으로 칠한다.
		if(!m_bIsSelected)
			memDC.FillSolidRect(&clientRect, RGB(255, 255, 255));
		else
			memDC.FillSolidRect(&clientRect, RGB(0, 0, 0));

		// 그리기 위한 영역을 준비한다.
		CRect rect(clientRect);
		rect.left += 2;
		rect.top += 2;
		rect.right -= 1;
		rect.bottom -= 1;

		// 선택되어 있지 않다면, 썸네일 이미지의 틀을 그린다.
		if(!m_bIsSelected)
		{
			CPen framePen(PS_SOLID, 4, RGB(0, 0, 0));
			CPen* pOldPen = memDC.SelectObject(&framePen);
			memDC.Rectangle(&rect);
			memDC.SelectObject(pOldPen);
		}

		// 이미지가 그려질 영역(가로 240px, 세로 200px)
		CRect imageRect(rect.left + 22, rect.top + 27, rect.right - 23, rect.bottom - 58);

		// 이미지가 이미지 영역보다 크다면 축소시킨다.
		int width = m_imgOrigin.GetWidth();
		int height = m_imgOrigin.GetHeight();
		if(width > imageRect.Width() || height > imageRect.Height())
		{
			double ratio = (width > height)? (double)imageRect.Width() / width : (double)imageRect.Height() / height;
			width = (int)(width * ratio);
			height = (int)(height * ratio);
		}

		// 축소된 이미지를 이미지 영역의 중앙에 정렬시킨다.
		int x = imageRect.left;
		int y = imageRect.top;
		if(width != imageRect.Width())
			x += (imageRect.Width() - width) / 2;
		if(height != imageRect.Height())
			y += (imageRect.Height() - height) / 2;

		// 썸네일 이미지를 그린다.
		m_imgOrigin.StretchBlt(memDC.GetSafeHdc(), x, y, width, height);

		// 파일명 텍스트를 그린다.
		CRect fileNameRect(rect.left + 22, rect.top + 237, rect.right - 23, rect.bottom - 14);
		CFont font;
		font.CreatePointFont(90, "맑은 고딕");
		CFont* pOldFont = memDC.SelectObject(&font);
		if(m_bIsSelected)  // 선택되어 있다면, 텍스트 색을 흰색으로 설정한다.
			memDC.SetTextColor(RGB(255, 255, 255));
		memDC.DrawText(m_pImageFeature->m_strFileName, fileNameRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
		memDC.SelectObject(pOldFont);

		// 메모리DC의 내용을 화면DC에 출력한다.
		pDC->BitBlt(0, 0, clientRect.right, clientRect.bottom, &memDC, 0, 0, SRCCOPY);

		// 메모리를 정리한다.
		memDC.SelectObject(pOldBitmap);
		memDC.DeleteDC();
	}
}
