/************************************************************************/
/*    Filename: mcpli_misc.cpp                                          */
/*              기타 함수                                               */
/*    Mini-CPL Interpreter (Ver. 1.00) Copyright (c) 2015 Minsu Jeon    */
/************************************************************************/
#include "mcpli.h"
#include "mcpli_prot.h"

/* 수치상수를 문자열로 변환 */
string dbl_to_s(double d)
{
	ostringstream ostr;  // 출력용 스트림 확보
	ostr << d;           // 출력 스트림에 쓰기
	return ostr.str();   // 버퍼 내용을 반환
}

/* 오류 정보 생성 */
string err_msg(const string& a, const string& b)
{
	if(a == "") return b + "가 필요합니다.";
	if(b == "") return a + "가 바르지 않습니다.";
	return b + "가 " + a + " 앞에 필요합니다.";
}

/* 오류를 표시하고 종료 */
void err_exit(Tobj a, Tobj b, Tobj c, Tobj d)
{
	Tobj ob[4] = {a, b, c, d};
	cerr << get_lineno() << " 라인 - ";

	for(int i = 0; i < 4 && ob[i].s != "\1"; i++) {
		if(ob[i].type == 'd') cout << ob[i].d;  // 수치 정보
		if(ob[i].type == 's') cout << ob[i].s;  // 문자열 정보
	}
	cout << endl;
	system("PAUSE");
	exit(1);
}
