/************************************************************************/
/*    Filename: mcpli_pars.cpp                                          */
/*              구문 분석                                               */
/*    Mini-CPL Interpreter (Ver. 1.00) Copyright (c) 2015 Minsu Jeon    */
/************************************************************************/
#include "mcpli.h"
#include "mcpli_prot.h"

Token g_token;                     /* 현재 처리중인 토큰           */
SymElem g_tmpSym;                  /* 임시 보관용 심볼             */
bool g_fVarDecl;                   /* 변수 선언부인지 여부 플래그  */
string g_progTitle;                /* 프로그램명 */
char g_codebuf[LIN_SIZ+1];         /* 내부 코드 생성용 버퍼        */
char *g_pCodebuf;                  /* 내부 코드 생성용 버퍼 포인터 */
extern vector<char*> g_intercode;  /* 변환을 마친 내부 코드 저장   */

/* 구문 분석을 위한 초기화 */
void init()
{
	init_chtyp();  // 문자 종류표 초기화
	g_pCodebuf = g_codebuf;
	g_fVarDecl = false;
}

/* 내부 코드로 변환 */
void convert_to_intercode(char *fname)
{
	init();  // 초기화

	push_intercode();  // 0행은 필요 없으므로 매운다.
	fileopen(fname);   // 파일 읽기

	g_token = nextline_tkn();
	while(g_token.kind != EofProg) {
		convert();  // 내부 코드로 변환 처리
	}

	set_startpc(1);  // 1행부터 프로그램 시작
}

/* 한 행의 맨 앞에 있는 코드를 변환 처리 */
void convert()
{
	switch (g_token.kind) {
	// 프로그램 헤더부
	case ProgDecl:
		set_code(g_token.kind);
		g_token = nexttkn();
		g_progTitle = g_token.text;  // 프로그램명 설정
		g_token = nexttkn();
		set_code_eofline();
		break;

	// 변수 선언부
	case VarDecl:
		g_fVarDecl = true;
		set_code_single();
		break;

	// 프로그램 처리부
	case ProgBegin:
		g_fVarDecl = false;
		set_code_single();
		break;

	// 프로그램 종료부
	case ProgEnd:
		g_fVarDecl = false;
		set_code_single();
		while(g_token.kind != EofProg) {  // 프로그램 종료부 이후의 코드는 무시한다.
			g_token = nextline_tkn();
		}
		break;

	// 자료형 선언
	case IntDecl:  // int형 선언
	case DblDecl:  // double형 선언
		vardecl();
		break;

	// print 내장함수
	case Print:
		set_code(g_token.kind);
		g_token = nexttkn();
		convert_rest();
		break;

	// 나머지 부분
	default: 
		convert_rest();
		break;
	}
}

/* 나머지 부분의 변환 처리 */
void convert_rest()
{
	int tblidx;

	while(1) {
		if(g_token.kind == Semicolon) {  // 반드시 이 행의 끝은 세미콜론으로 끝나야 한다.
			set_code(g_token.kind);
			g_token = nexttkn();

			// 세미콜론 다음에 행이 끝나지 않으면 안 된다.
			if(g_token.kind != EofLine) {
				err_exit("잘못된 기술입니다: ", g_token.text);
			}
			break;
		}
		if(g_token.kind == EofLine) {  // 세미콜론이 오지 않고 행이 끝나면 안 된다.
			err_exit("잘못된 기술입니다: ", g_token.text);
		}

		switch(g_token.kind) {
		// 이 행에서 다음과 같은 토큰이 나타나면 안 된다.
		case ProgDecl: case VarDecl: case ProgBegin: 
		case ProgEnd:  case Print:
			err_exit("잘못된 기술입니다: ", g_token.text);
			break;

		// 변수
		case Ident:
			set_tmpsym_name();
			if((tblidx = search_name(g_tmpSym.name)) == -1) {  // 등록된 변수가 없음
				err_exit("선언되지 않은 변수입니다: ", g_token.text);
			}
			set_code(Ident, tblidx);
			continue;

		// 수치상수
		case IntLitr: case DblLitr:
			set_code(g_token.kind, enter_literal(g_token.dblVal));
			break;

		// 연산자 등
		default:
			set_code(g_token.kind);
			break;
		}
		g_token = nexttkn();
	}

	push_intercode();
	g_token = nextline_tkn();
}

/* 변수 선언부 */
void vardecl()
{
	// 변수 선언부에서만 변수 선언이 가능하다.
	if(!g_fVarDecl) {
		err_exit("변수 선언은 Var영역에서만 가능합니다.");
	}

	char typ = DblType;
	if(g_token.kind == IntDecl) typ = IntType;

	set_code(g_token.kind);
	set_code_rest();  // 나머지는 그대로 저장
	while(1) {
		g_token = nexttkn();
		chk_varname();                  // 변수명 확인
		set_tmpsym(typ);                // 이름, 자료형 설정
		enter(g_tmpSym);                // 심볼 테이블에 변수 등록
		if(g_token.kind != ',') break;  // 선언 종료
	}
	chk_semicolon();  // 행이 세미콜론으로 끝나는지 확인
	set_code_eofline();
}

/* 이름 확인 */
void chk_varname()
{
	if(g_token.kind != Ident) {
		err_exit(err_msg(g_token.text, "변수"));
	}
	if(search_name(g_token.text) != -1) {
		err_exit("변수가 중복되었습니다: ", g_token.text);
	}
}

/* 세미콜론 확인 */
void chk_semicolon()
{
	if(g_token.kind != Semicolon) {
		err_exit("잘못된 기술입니다: ", g_token.text);
	}

	g_token = nexttkn();
}

/* 임시 보관용 심볼의 이름 설정 */
void set_tmpsym_name()
{
	if(g_token.kind != Ident) {
		err_exit("변수가 필요합니다: ", g_token.text);
	}

	g_tmpSym.clear();
	g_tmpSym.name = g_token.text;
	g_token = nexttkn();
}

/* 임시 보관용 심볼 설정 */
void set_tmpsym(char typ)
{
	set_tmpsym_name();
	g_tmpSym.dtTyp = typ;
}

/* 코드 저장 */
void set_code(int cd)
{
	*g_pCodebuf++ = (char)cd;
}

/* 코드와 short값 저장 */
void set_code(int cd, int nbr)
{
	*g_pCodebuf++ = (char)cd;
	*SHORT_P(g_pCodebuf) = (short)nbr;
	g_pCodebuf += SHORT_SIZ;
}

/* 단일 코드 저장 */
void set_code_single()
{
	set_code(g_token.kind);
	g_token = nexttkn();
	set_code_eofline();
}

/* 나머지 텍스트를 그대로 저장 */
void set_code_rest()
{
	extern char *g_pBuf;
	strcpy(g_pCodebuf, g_pBuf);
	g_pCodebuf += strlen(g_pBuf) + 1;
}

/* 최종 저장 처리 */
void set_code_eofline()
{
	if(g_token.kind != EofLine) {
		err_exit("잘못된 기술입니다: ", g_token.text);
	}

	push_intercode();
	g_token = nextline_tkn();  // 다음 행으로 진행
}

/* 변환한 내부 코드를 저장 */
void push_intercode()
{
	int len;
	char *p;

	*g_pCodebuf++ = '\0';
	if((len = g_pCodebuf - g_codebuf) >= LIN_SIZ) {
		err_exit("변환 후의 내부 코드가 너무 깁니다. 식을 줄여주세요.");
	}

	try {
		p = new char[len];	        // 내부 코드 저장을 위한 메모리 확보
		memcpy(p, g_codebuf, len);  // 내부 코드 생성용 버퍼의 내용을 복사
		g_intercode.push_back(p);   // 내부 코드 저장
	}
	catch(bad_alloc) {
		err_exit("메모리를 확보할 수 없습니다"); 
	}

	g_pCodebuf = g_codebuf;  // 다음 처리를 대비해 저장할 곳 맨 앞으로 위치 지정
}

