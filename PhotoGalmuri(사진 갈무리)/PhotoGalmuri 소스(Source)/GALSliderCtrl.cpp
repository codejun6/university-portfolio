// GALSliderCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PhotoGalmuri.h"
#include "GALSliderCtrl.h"


// CGALSliderCtrl

IMPLEMENT_DYNAMIC(CGALSliderCtrl, CSliderCtrl)

CGALSliderCtrl::CGALSliderCtrl()
{
}

CGALSliderCtrl::~CGALSliderCtrl()
{
}


BEGIN_MESSAGE_MAP(CGALSliderCtrl, CSliderCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CGALSliderCtrl::OnNMCustomdraw)
END_MESSAGE_MAP()



// CGALSliderCtrl 메시지 처리기입니다.




void CGALSliderCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMCUSTOMDRAW lpcd = (LPNMCUSTOMDRAW)pNMHDR;
	UINT drawStage = lpcd->dwDrawStage;
	UINT itemSpec = lpcd->dwItemSpec;
	
	switch(drawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT;
		break;
	
	case CDDS_PREERASE:
	case CDDS_POSTERASE: 
	case CDDS_ITEMPREERASE:	
	case CDDS_ITEMPOSTERASE:
		*pResult = CDRF_DODEFAULT;
		break;

	case CDDS_ITEMPREPAINT:
		switch(itemSpec)
		{
		case TBCD_CHANNEL:
			*pResult = CDRF_DODEFAULT|CDRF_NOTIFYPOSTPAINT;
			break;

		case TBCD_TICS:
			*pResult = CDRF_DODEFAULT;
			break;

		case TBCD_THUMB:
			CDC *pDC = CDC::FromHandle(lpcd->hdc);
			pDC->FillSolidRect(&lpcd->rc, RGB(255, 255, 255));
			*pResult = CDRF_SKIPDEFAULT;
			break;
		}
		break;
	
	case CDDS_ITEMPOSTPAINT:
		switch(itemSpec)
		{
		case TBCD_CHANNEL:
			CDC *pDC = CDC::FromHandle(lpcd->hdc);
			CPen pen(PS_SOLID, 1, RGB(255, 255, 255));
			pDC->SelectObject(&pen);
			pDC->Rectangle(&lpcd->rc);
			*pResult = CDRF_SKIPDEFAULT;
			break;
		}
		break;
		
	case CDDS_POSTPAINT:
		CDC *pDC = CDC::FromHandle(lpcd->hdc);
		CFont font;
		font.CreatePointFont(90, "맑은 고딕");
		pDC->SelectObject(&font);
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(255, 255, 255));
		CRect rect;
		GetClientRect(&rect);
		pDC->TextOut(rect.left, rect.bottom-16, "-100");
		pDC->TextOut(rect.right-30, rect.bottom-16, "100");
		break;
	}
}
