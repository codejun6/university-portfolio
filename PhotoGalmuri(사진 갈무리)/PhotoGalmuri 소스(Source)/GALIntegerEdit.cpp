// GALIntegerEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PhotoGalmuri.h"
#include "GALIntegerEdit.h"


// CGALIntegerEdit

IMPLEMENT_DYNAMIC(CGALIntegerEdit, CEdit)

CGALIntegerEdit::CGALIntegerEdit()
{

}

CGALIntegerEdit::~CGALIntegerEdit()
{
}


BEGIN_MESSAGE_MAP(CGALIntegerEdit, CEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()



// CGALIntegerEdit 메시지 처리기입니다.




void CGALIntegerEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// 정수만 입력할 수 있게 한다.
	if(!iscntrl(nChar))
	{
		TCHAR tsz[] = {nChar, 0};
		if(tsz[0] == '-' && LOWORD(GetSel()) != 0)
			return;
		if(tsz[0] == '.')
			return;

		if(!GetNumberFormat(NULL, 0, tsz, NULL, NULL, 0))
			return;
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);

	// 값이 범위를 넘어서는 경우 보정한다.
	CString str;
	GetWindowText(str);
	if(str != "" && str != "-")
	{
		int value = atoi(str);
		if(value < -100)      value = -100;
		else if(value > 100)  value = 100;
		str.Format("%d", value);
		SetWindowText(str);
		SetSel(0, -1);
		ReplaceSel(str);
	}
}
