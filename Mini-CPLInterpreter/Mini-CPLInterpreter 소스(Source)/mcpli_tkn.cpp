/************************************************************************/
/*    Filename: mcpli_tkn.cpp                                           */
/*              토큰 처리(어휘 분석)                                    */
/*    Mini-CPL Interpreter (Ver. 1.00) Copyright (c) 2015 Minsu Jeon    */
/************************************************************************/
#include "mcpli.h"
#include "mcpli_prot.h"

/* 키워드 대응을 관리 */
struct Keyword {
	const char *keyName;  /* 키워드 문자열   */
	TknKind keyKind;      /* 대응하는 키워드 */
};

/* 키워드 대응 테이블 */
Keyword g_keyTbl[] = {
	{"program", ProgDecl }, {"begin" , ProgBegin}, 
	{"end"    , ProgEnd  }, {"var"   , VarDecl  },
	{"int"    , IntDecl  }, {"double", DblDecl  },
	{"print"  , Print    },
	{"("      , Lparen   }, {")"     , Rparen   },
	{"+"      , Plus     }, {"-"     , Minus    },
	{"*"      , Multi    }, {"/"     , Divi     },
	{"="      , Assign   }, {","     , Comma    },
	{";"      , Semicolon},
	{"@dummy", END_KeyList}
};

TknKind g_chtyp[256];   /* 문자 종류표 배열           */
ifstream g_fin;         /* 입력 스트림                */
int g_srcLineno;        /* 소스의 행 번호             */
bool g_fEndOfFile;      /* 파일 종료 플래그           */
char g_buf[LIN_SIZ+5];  /* 소스 한 줄을 읽어들일 버퍼 */
char *g_pBuf;           /* 토큰 분석용 버퍼 포인터    */
#define MAX_LINE 2000   /* 최대 프로그램 행수         */

/* 문자 종류표 설정 */
void init_chtyp()
{                 
	for(int i = 0; i < 256; i++) { g_chtyp[i] = Others; }
	for(int i = '0'; i <= '9'; i++) { g_chtyp[i] = Digit;  }
	for(int i = 'A'; i <= 'Z'; i++) { g_chtyp[i] = Letter; }
	for(int i = 'a'; i <= 'z'; i++) { g_chtyp[i] = Letter; }
	g_chtyp['_'] = Letter;
	g_chtyp['('] = Lparen;  g_chtyp[')'] = Rparen;
	g_chtyp['+'] = Plus;    g_chtyp['-'] = Minus;
	g_chtyp['*'] = Multi;   g_chtyp['/'] = Divi;
	g_chtyp['='] = Assign;	g_chtyp[','] = Comma;
	g_chtyp[';'] = Semicolon;
}

/* 파일 열기 */
void fileopen(char *fname)
{
	g_srcLineno = 0;
	g_fEndOfFile = false;
	g_fin.open(fname);
	if(!g_fin) { 
		cout << fname << "을(를) 열 수 없습니다.\n"; 
		system("PAUSE");
		exit(1); 
	}
}

/* 다음 행 읽기 */
void nextline()
{
	string s;

	if (g_fEndOfFile) return;
	g_fin.getline(g_buf, LIN_SIZ+5);  // 한 행 읽기
	if (g_fin.eof()) {  // 파일 종료
		g_fin.clear(); 
		g_fin.close();
		g_fEndOfFile = true; 
		return;
	}

	if (strlen(g_buf) > LIN_SIZ)
		err_exit("프로그램은 1행 ", LIN_SIZ, " 문자이내로 기술해 주세요");
	if (++g_srcLineno > MAX_LINE)
		err_exit("프로그램이 ", MAX_LINE, " 을(를) 넘었습니다.");

	g_pBuf = g_buf;  // 토큰 분석용 포인터를 버퍼의 맨 앞에 위치시킨다
}

/* 다음 토큰을 반환 */
#define CH (*g_pBuf)          // 현재 문자 읽기
#define C2 (*(g_pBuf + 1))    // 다음 문자 읽기
#define NEXT_CH() (++g_pBuf)  // 다음 문자를 가리킨다
Token nexttkn()
{
	TknKind kd;
	string txt = "";

	if(g_fEndOfFile) return Token(EofProg);  // 파일 종료
	while(isspace(CH)) NEXT_CH();            // 공백 무시
	if(CH == '\0') return Token(EofLine);    // 행의 끝

	switch(g_chtyp[CH]) {
	// 문자('_' 포함)
	case Letter:
		txt += CH; NEXT_CH();
		while(g_chtyp[CH] == Letter || g_chtyp[CH] == Digit) { txt += CH; NEXT_CH(); }
		break;

	// 수치상수
	case Digit:
		kd = IntLitr;
		while(g_chtyp[CH] == Digit) { txt += CH; NEXT_CH(); }
		if(CH == '.') { kd = DblLitr; txt += CH; NEXT_CH(); }
		while(g_chtyp[CH] == Digit) { txt += CH; NEXT_CH(); }
		return Token(kd, txt, atof(txt.c_str()));  // IntLitr도 double형으로 저장

	default:
		if(CH=='/' && C2=='/') return Token(EofLine);  // 주석
		else { txt += CH; NEXT_CH(); }
		break;
	}
	kd = get_kind(txt);  // 종류별 설정

	if (kd == Others) err_exit("잘못된 토큰입니다: ", txt);
	return Token(kd, txt);
}

/* 토큰을 확인 후 반환 */
Token chk_nexttkn(const Token& tk, int kind)
{
	if(tk.kind != kind){
		err_exit(err_msg(tk.text, kind_to_s(kind)));
	}
	return nexttkn();
}

/* 다음 행을 읽고, 다음 토큰을 반환 */
Token nextline_tkn()
{
	nextline();
	return nexttkn();
}

/* 키워드 문자열의 토큰 종류를 반환 */
TknKind get_kind(const string& s)
{
	for(int i = 0; g_keyTbl[i].keyKind != END_KeyList; i++) {
		if(s == g_keyTbl[i].keyName) return g_keyTbl[i].keyKind;
	}
	if(g_chtyp[s[0]] == Letter) return Ident;
	if(g_chtyp[s[0]] == Digit) return DblLitr;
	return Others;  // 없다
}

/* 종류별 키워드의 문자열을 반환 */
string kind_to_s(int kd)
{
	for(int i = 0; ; i++) {
		if(g_keyTbl[i].keyKind == END_KeyList) break;
		if(g_keyTbl[i].keyKind == kd) return g_keyTbl[i].keyName;
	}
	return "";
}

/*  종류별 키워드의 문자열을 반환 */
string kind_to_s(const Code& cd)
{
	switch (cd.kind) {
	// 변수
	case Ident: 
		return get_symiter(cd)->name;

	// 수치상수
	case IntLitr: case DblLitr: 
		return dbl_to_s(cd.dblVal);

	// 행의 끝
	case EofLine: 
		return "";
	}
	return kind_to_s(cd.kind);
}

/* 토큰 분석용 포인터 설정 */
void set_bufptr(char *p)
{
	g_pBuf = p;
}

/* 분석하거나 실행 중인 행 번호를 반환 */
int get_lineno()
{
	extern int g_pc;
	return (g_pc == -1)? g_srcLineno : g_pc;  // 분석 중 : 실행 중
}

