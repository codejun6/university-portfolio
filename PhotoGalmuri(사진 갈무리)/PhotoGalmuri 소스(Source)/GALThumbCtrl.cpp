// GALThumbCtrl.cpp : ���� �����Դϴ�.
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



// CGALThumbCtrl �޽��� ó�����Դϴ�.




void CGALThumbCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	ASSERT(lpDrawItemStruct->CtlType == ODT_BUTTON);

	// ����� �̹����� �������� ������ ����� ��Ʈ���� �׸��� �ʴ´�.
	if(!m_imgOrigin.IsNull())
	{
		CRect clientRect;
		GetClientRect(&clientRect);

		CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

		// ���� ���۸��� ���� �޸�DC�� �غ��Ѵ�.
		CDC memDC;
		memDC.CreateCompatibleDC(pDC);
		SetStretchBltMode(memDC, COLORONCOLOR);
		CBitmap memBitmap;
		memBitmap.CreateCompatibleBitmap(pDC, clientRect.Width(), clientRect.Height());
		CBitmap* pOldBitmap = memDC.SelectObject(&memBitmap);
		
		// �޸�DC�� ����� ĥ�Ѵ�.
		// ���õǾ� ���� �ʴٸ�, �������, 
		// ���õǾ� �ִٸ�, ���������� ĥ�Ѵ�.
		if(!m_bIsSelected)
			memDC.FillSolidRect(&clientRect, RGB(255, 255, 255));
		else
			memDC.FillSolidRect(&clientRect, RGB(0, 0, 0));

		// �׸��� ���� ������ �غ��Ѵ�.
		CRect rect(clientRect);
		rect.left += 2;
		rect.top += 2;
		rect.right -= 1;
		rect.bottom -= 1;

		// ���õǾ� ���� �ʴٸ�, ����� �̹����� Ʋ�� �׸���.
		if(!m_bIsSelected)
		{
			CPen framePen(PS_SOLID, 4, RGB(0, 0, 0));
			CPen* pOldPen = memDC.SelectObject(&framePen);
			memDC.Rectangle(&rect);
			memDC.SelectObject(pOldPen);
		}

		// �̹����� �׷��� ����(���� 240px, ���� 200px)
		CRect imageRect(rect.left + 22, rect.top + 27, rect.right - 23, rect.bottom - 58);

		// �̹����� �̹��� �������� ũ�ٸ� ��ҽ�Ų��.
		int width = m_imgOrigin.GetWidth();
		int height = m_imgOrigin.GetHeight();
		if(width > imageRect.Width() || height > imageRect.Height())
		{
			double ratio = (width > height)? (double)imageRect.Width() / width : (double)imageRect.Height() / height;
			width = (int)(width * ratio);
			height = (int)(height * ratio);
		}

		// ��ҵ� �̹����� �̹��� ������ �߾ӿ� ���Ľ�Ų��.
		int x = imageRect.left;
		int y = imageRect.top;
		if(width != imageRect.Width())
			x += (imageRect.Width() - width) / 2;
		if(height != imageRect.Height())
			y += (imageRect.Height() - height) / 2;

		// ����� �̹����� �׸���.
		m_imgOrigin.StretchBlt(memDC.GetSafeHdc(), x, y, width, height);

		// ���ϸ� �ؽ�Ʈ�� �׸���.
		CRect fileNameRect(rect.left + 22, rect.top + 237, rect.right - 23, rect.bottom - 14);
		CFont font;
		font.CreatePointFont(90, "���� ���");
		CFont* pOldFont = memDC.SelectObject(&font);
		if(m_bIsSelected)  // ���õǾ� �ִٸ�, �ؽ�Ʈ ���� ������� �����Ѵ�.
			memDC.SetTextColor(RGB(255, 255, 255));
		memDC.DrawText(m_pImageFeature->m_strFileName, fileNameRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
		memDC.SelectObject(pOldFont);

		// �޸�DC�� ������ ȭ��DC�� ����Ѵ�.
		pDC->BitBlt(0, 0, clientRect.right, clientRect.bottom, &memDC, 0, 0, SRCCOPY);

		// �޸𸮸� �����Ѵ�.
		memDC.SelectObject(pOldBitmap);
		memDC.DeleteDC();
	}
}
