/************************************************************************/
/*    Filename: mcpli_code.cpp                                          */
/*              메모리 관리, 구문 검사, 실행                            */
/*    Mini-CPL Interpreter (Ver. 1.00) Copyright (c) 2015 Minsu Jeon    */
/************************************************************************/
#include "mcpli.h"
#include "mcpli_prot.h"

Code g_code;                      /* 현재 처리중인 코드                */
int g_startPc;                    /* 프로그램 실행 시작행              */
int g_pc = -1;                    /* 프로그램 카운터(-1: 실행 중 아님) */
int g_maxLine;                    /* 프로그램 끝행                     */
bool g_fSyntaxChkMode = false;    /* 구문 검사 모드 플래그             */
vector<char*> g_intercode;        /* 변환을 마친 내부 코드 저장        */
char *g_pCode;                    /* 내부 코드 분석용 포인터           */
MyMemory g_dmem;                  /* 데이터 메모리                     */
vector<double> g_litrTbl;         /* 상수 테이블                       */
extern vector<SymElem> g_symTbl;  /* 글로벌 심볼 테이블                */

/* stack<double>의 랩퍼 클래스 */
class MyStack {
private: 
	stack<double> stk;
public:
	void push(double n) { stk.push(n); }
	int size() { return (int)stk.size(); }
	bool empty() { return stk.empty(); }
	double pop() {  // 읽고, 삭제
		if(stk.empty()) err_exit("스택 Underflow");
		double d = stk.top();
		stk.pop(); 
		return d;
	}
};
MyStack g_oprStk;  /* 오퍼랜드 스택 */

/* 구문 검사 */
void syntax_check()
{
	g_fSyntaxChkMode = true;
	for(g_pc = 1; g_pc < (int)g_intercode.size(); g_pc++) {
		g_code = firstcode(g_pc);

		switch(g_code.kind) {
		// 이미 검사를 완료한 구문
		case ProgDecl: case VarDecl: case ProgBegin: 
		case ProgEnd:  case IntDecl: case DblDecl:
			break;

		// 행의 끝은 무시
		case EofLine:
			break;

		// 대입문
		case Ident:
			(void)get_varadrs(g_code);             // 좌변 주소
			(void)get_expression('=', Semicolon);  // 우변식 값
			break;

		// print 내장함수
		case Print:
			chk_sysfnc(g_code.kind);
			break;

		// 행의 첫 코드로 다른 코드가 올 수 없다.
		default:
			err_exit("잘못된 기술입니다: ", kind_to_s(g_code.kind));
		}
	}
	g_fSyntaxChkMode = false;
}

/* 프로그램 시작행 설정 */
void set_startpc(int n)
{
	g_startPc = n;
}

/* 프로그램 실행 */
void execute()
{
	g_dmem.resize(g_dmem.size() + 1000);  // 메모리 영역 초기 확보
	g_pc = g_startPc;
	g_maxLine = g_intercode.size() - 1;

	extern string g_progTitle;
	cout << "[" << g_progTitle << "]" << endl;

	while(g_pc <= g_maxLine) {
		statement();
	}
	g_pc = -1;  // 비실행 상태로 설정
}

/* 문 처리 */
void statement()
{
	char typ;
	int varadrs;
	double rst = 0;

	if(g_pc > g_maxLine) return;  // 프로그램 종료
	g_code = firstcode(g_pc);

	switch(g_code.kind) {
	// 실행 시는 무시
	case ProgDecl: case VarDecl: case ProgBegin: 
	case ProgEnd:  case IntDecl: case DblDecl:
	case EofLine:
		g_pc++;
		break;

	// 대입문
	case Ident:
		typ = get_vartype(g_code);
		varadrs = get_varadrs(g_code);
		expression('=', 0);
		rst = g_oprStk.pop();
		if(typ == IntType) rst = (int)rst;
		g_dmem.set(varadrs, rst);
		g_pc++;
		break;

	// print 내장함수
	case Print:
		exec_sysfnc(g_code.kind);
		g_pc++;
		break;

	// 행의 첫 코드로 다른 코드가 올 수 없다.
	default:
		err_exit("잘못된 기술입니다: ", kind_to_s(g_code.kind));
	}
}

/* 식을 계산하고 결과를 반환 */
double get_expression(int kind1, int kind2)
{
	expression(kind1, kind2); 
	return g_oprStk.pop();
}

/* 조건부 식 처리 */
void expression(int kind1, int kind2)
{
	if(kind1 != 0) g_code = chk_nextcode(g_code, kind1);
	expression();
	if(kind2 != 0) g_code = chk_nextcode(g_code, kind2);
}

/* 일반 식 처리 */
void expression()
{
	term(1);
}

/* 항 처리 */
void term(int pri)
{
	if(pri == 3) {  // 우선순위가 가장 높은 인자부터 처리
		factor(); 
		return; 
	}
	term(pri + 1);  // 우선순위가 높은 것부터 처리되도록 재귀호출

	while(pri == get_op_order(g_code.kind)) {  // 우선순위가 같은 연산자가 연속된다.
		TknKind op = g_code.kind;
		g_code = nextcode();
		term(pri + 1);  // 높은 우선순위를 먼저 처리

		if(g_fSyntaxChkMode) {  // 구문 검사 모드 시
			g_oprStk.pop();
			g_oprStk.pop();
			g_oprStk.push(1.0);
			continue;
		}

		bin_operation(op);  // 이항 연산자를 이용해 이항 연산 수행
	}
}

/* 인자 처리 */
void factor()
{
	TknKind kd = g_code.kind;

	if(g_fSyntaxChkMode) {  // 구문 검사 모드 시
		switch(kd) {
		// '+', '-' 기호가 붙은 경우
		case Plus: case Minus:
			g_code = nextcode();
			factor();
			g_oprStk.pop();
			g_oprStk.push(1.0);
			break;

		// 좌괄호인 경우, 우괄호와 짝이 맞아야 함
		case Lparen:
			expression('(', ')');
			break;

		// 수치상수인 경우
		case IntLitr: case DblLitr:
			g_oprStk.push(1.0); 
			g_code = nextcode();
			break;

		// 변수인 경우, 변수의 주소를 읽어보고 적당한 값을 넣어줌
		case Ident:
			(void)get_varadrs(g_code);
			g_oprStk.push(1.0);
			break;

		case Semicolon: case EofLine:
			err_exit("식이 바르지 않습니다.");

		default:
			err_exit("식 오류:", kind_to_s(g_code));  // a + = 등에서 발생
		}
		return;
	}

	switch(kd) {  // 실행 시
	// '+', '-' 기호가 붙은 경우
	case Plus: case Minus:
		g_code = nextcode();
		factor();
		if(kd == Minus) g_oprStk.push(-g_oprStk.pop());
		break;

	// 좌괄호
	case Lparen:
		expression('(', ')');
		break;

	// 수치상수
	case IntLitr: case DblLitr:
		g_oprStk.push(g_code.dblVal);
		g_code = nextcode();
		break;

	// 변수
	case Ident:
		g_oprStk.push(g_dmem.get(get_varadrs(g_code)));
		break;
	}
}

/* 변수의 자료형을 반환 */
char get_vartype(const Code& cd)
{
	if(cd.kind != Ident) {
		err_exit("변수명이 필요합니다: ", kind_to_s(cd));
	}

	return get_symiter(cd)->dtTyp;
}

/* 변수의 주소를 반환 */
int get_varadrs(const Code& cd)
{
	if(cd.kind != Ident) {
		err_exit("변수명이 필요합니다: ", kind_to_s(cd));
	}

	int adrs = get_symiter(cd)->adrs;
	g_code = nextcode();
	return adrs;
}

/* 이항 연산자의 우선순위를 반환 */
int get_op_order(TknKind kd)
{
	switch(kd) {
	// '*', '/'
	case Multi: case Divi:  
		return 2;

	// '+', '-'
	case Plus: case Minus:
		return 1;

	// 해당 없음
	default:
		return 0;
	}
}

/* 이항 연산 */
void bin_operation(TknKind op)
{
	double rst = 0;
	double d2 = g_oprStk.pop();
	double d1 = g_oprStk.pop();

	if((op == Divi) && d2 == 0) {
		err_exit("0으로 나누었습니다.");
	}

	switch(op) {
	case Plus:  rst = d1 + d2; break;
	case Minus: rst = d1 - d2; break;
	case Multi: rst = d1 * d2; break;
	case Divi:  rst = d1 / d2; break;
	}
	g_oprStk.push(rst);
}

/* 내장함수 검사 */
void chk_sysfnc(TknKind kd)
{
	switch(kd) {
	case Print:
		do {
			g_code = nextcode();
			(void)get_expression();		
		} while(g_code.kind == ',');
		if(g_code.kind != Semicolon) err_exit("잘못된 기술입니다: ", kind_to_s(g_code));
		g_code = nextcode();
		if(g_code.kind != EofLine) err_exit("잘못된 기술입니다: ", kind_to_s(g_code));
		break;
	}
}

/* 내장함수 실행 */
void exec_sysfnc(TknKind kd)
{
	switch(kd) {
	// 모든 매개변수를 출력 후, 개행을 한다.
	case Print:
		do {
			g_code = nextcode();
			cout << get_expression() << " ";  // 수치 출력
		} while(g_code.kind == ',');   // ','라면 매개변수가 계속된다.
		cout << endl;
		break;
	}
}

/* 다음 코드를 반환 */
Code nextcode()
{
	TknKind kd;
	short int tblidx;

	if(*g_pCode == '\0') return Code(EofLine);
	kd = (TknKind)*UCHAR_P(g_pCode++);

	switch(kd) { 
	// 수치상수
	case IntLitr: case DblLitr: 
		tblidx = *SHORT_P(g_pCode); 
		g_pCode += SHORT_SIZ;
		return Code(kd, g_litrTbl[tblidx]);

	// 변수
	case Ident:
		tblidx = *SHORT_P(g_pCode);
		g_pCode += SHORT_SIZ;
		return Code(kd, tblidx);

	// 부속 정보가 없는 코드
	default:                              
		return Code(kd);
	}
}

/* 코드를 확인 후 반환 */
Code chk_nextcode(const Code& cd, int kind)
{
	if(cd.kind != kind) {
		if(kind == EofLine) err_exit("잘못된 기술입니다: ", kind_to_s(cd));
		if(cd.kind == EofLine) err_exit(kind_to_s(kind), "가 필요합니다.");
		err_exit(kind_to_s(kind) + "가(이) " + kind_to_s(cd) + " 앞에 필요합니다.");
	}
	return nextcode();
}

/* 해당 행의 시작 코드를 반환 */
Code firstcode(int line)
{
	g_pCode = g_intercode[line];  // 분석용 포인터를 행의 시작으로 설정
	return nextcode();
}

/* 상수 테이블에 수치상수를 등록 */
int enter_literal(double d)
{
	for(int n = 0; n < (int)g_litrTbl.size(); n++) {
		if(g_litrTbl[n] == d) return n;  // 같은 첨자 위치를 반환
	}

	g_litrTbl.push_back(d);  // 상수 테이블에 저장
	return g_litrTbl.size() - 1;  // 저장 위치 반환
}

