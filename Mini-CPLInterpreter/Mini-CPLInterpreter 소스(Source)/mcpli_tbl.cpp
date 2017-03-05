/************************************************************************/
/*    Filename: mcpli_tbl.cpp                                           */
/*              심볼 테이블 처리                                        */
/*    Mini-CPL Interpreter (Ver. 1.00) Copyright (c) 2015 Minsu Jeon    */
/************************************************************************/
#include "mcpli.h"
#include "mcpli_prot.h"

vector<SymElem> g_symTbl;  /* 심볼 테이블 */

/* 심볼 테이블에 심볼을 등록 */
int enter(SymElem& elem)
{
	extern MyMemory g_dmem;  // 메모리
	int n, mem_size;

	mem_size = 1;

	n = search_name(elem.name);  // 중복 확인
	if(n != -1) {
		err_exit("이름이 중복되었습니다: ", elem.name);
	}

	elem.adrs = g_dmem.size();                // 주소 설정
	g_dmem.resize(g_dmem.size() + mem_size);  // 메모리 영역 확보

	n = g_symTbl.size();
	g_symTbl.push_back(elem);  // 심볼 테이블에 저장
	return n;  // 저장 위치 반환
}

/* 심볼 테이블 내 변수명 검색 */
int search_name(const string& s)
{
	// 심볼 테이블 검색
	for(int n = 0; n < (int)g_symTbl.size(); n++) {
		if(g_symTbl[n].name == s) return n;
	}

	return -1;  // 발견되지 않음
}

/* 심볼 테이블의 반복자 획득 */
vector<SymElem>::iterator get_symiter(const Code& cd)
{
	return g_symTbl.begin() + cd.symIdx;
}

