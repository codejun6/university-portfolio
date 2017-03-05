/************************************************************************/
/*    Filename: mcpli.h                                                 */
/*              공통 헤더                                               */
/*    Mini-CPL Interpreter (Ver. 1.00) Copyright (c) 2015 Minsu Jeon    */
/************************************************************************/
#include <iostream>
#include <fstream>    // 파일 처리용
#include <sstream>    // 문자열 스트림 
#include <string>
#include <vector>
#include <stack>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cctype>

using namespace std;

#define SHORT_SIZ   sizeof(short int)     /* short int형의 크기            */
#define SHORT_P(p)  (short int *)(p)      /* short int형 포인터로 변환     */
#define UCHAR_P(p)  (unsigned char *)(p)  /* unsigned char형 포인터로 변환 */
#define LIN_SIZ     255                   /* 소스 한 줄의 최대 크기        */

/* 토큰 종류 */
enum TknKind {
	Lparen='(', Rparen=')', Plus='+', Minus='-', Multi='*', Divi='/', 
	Assign='=', Comma=',', Semicolon=';',
	ProgDecl=150, ProgBegin, ProgEnd, VarDecl, 
	IntDecl, DblDecl, Print, END_KeyList,
	Letter, Digit, Ident, IntLitr, DblLitr,
	EofLine, EofProg, Others
};

/* 토큰 정보 */
struct Token {
	TknKind kind;    /* 토큰 종류           */
	string  text;    /* 토큰 문자열         */
	double  dblVal;  /* 수치 상수일 때의 값 */
	Token() { kind = Others; text = ""; dblVal = 0.0; }
	Token(TknKind k) { kind = k; text = ""; dblVal = 0.0; }
	Token(TknKind k, double d) { kind = k; text = ""; dblVal = d; }
	Token(TknKind k, const string& s) { kind = k; text = s; dblVal = 0.0; }
	Token(TknKind k, const string& s, double d) { kind = k; text = s; dblVal = d; }
};

/* 자료형 이름 */
enum DtType { 
	NonType, IntType, DblType
};

/* 심볼 테이블 요소 정보 */
struct SymElem {
	string  name;    /* 변수의 이름   */
	char    dtTyp;   /* 변수의 자료형 */
	int     adrs;    /* 변수의 주소   */
	SymElem() { clear(); }
	void clear() { name = ""; dtTyp = NonType; adrs = 0; }
};

/* 코드 정보 */
struct Code {
	TknKind kind;    /* 코드 종류               */
	double  dblVal;  /* 수치 상수일 때의 값     */
	int     symIdx;  /* 심볼 테이블의 첨자 위치 */
	Code() { clear(); }
	Code(TknKind k) { clear(); kind = k; }
	Code(TknKind k, double d) { clear(); kind = k; dblVal = d; }
	Code(TknKind k, int idx) { clear(); kind = k; symIdx = idx;	}
	void clear() { kind = Others; dblVal = 0.0; symIdx = -1; }
};

/* 형 정보 객체 */
struct Tobj {
	char type;  /* 저장형 'd':double, 's':string, '-':미정 */
	double d;
	string s;
	Tobj()                 { type = '-'; d = 0.0; s = ""; }
	Tobj(double dt)        { type = 'd'; d = dt;  s = ""; }
	Tobj(const string& st) { type = 's'; d = 0.0; s = st; }
	Tobj(const char *st)   { type = 's'; d = 0.0; s = st; }
};

/* 데이터 메모리 */
class MyMemory {
private:
	vector<double> mem;
public:
	void auto_resize(int n) {  /* 재확보 횟수 억제를 위해 확보 */
		if(n >= (int)mem.size()) { 
			n = (n/256 + 1) * 256; 
			mem.resize(n); 
		}
	}
	void set(int adrs, double dt) {	mem[adrs] = dt; }   /* 메모리 쓰기     */
	void add(int adrs, double dt) { mem[adrs] += dt; }  /* 메모리 더하기   */
	double get(int adrs) { return mem[adrs]; }          /* 메모리 읽어내기 */
	int size() { return (int)mem.size(); }              /* 저장 크기       */
	void resize(unsigned int n) { mem.resize(n); }      /* 크기 확보       */
};

