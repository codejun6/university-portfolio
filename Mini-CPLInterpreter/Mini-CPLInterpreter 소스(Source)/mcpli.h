/************************************************************************/
/*    Filename: mcpli.h                                                 */
/*              ���� ���                                               */
/*    Mini-CPL Interpreter (Ver. 1.00) Copyright (c) 2015 Minsu Jeon    */
/************************************************************************/
#include <iostream>
#include <fstream>    // ���� ó����
#include <sstream>    // ���ڿ� ��Ʈ�� 
#include <string>
#include <vector>
#include <stack>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cctype>

using namespace std;

#define SHORT_SIZ   sizeof(short int)     /* short int���� ũ��            */
#define SHORT_P(p)  (short int *)(p)      /* short int�� �����ͷ� ��ȯ     */
#define UCHAR_P(p)  (unsigned char *)(p)  /* unsigned char�� �����ͷ� ��ȯ */
#define LIN_SIZ     255                   /* �ҽ� �� ���� �ִ� ũ��        */

/* ��ū ���� */
enum TknKind {
	Lparen='(', Rparen=')', Plus='+', Minus='-', Multi='*', Divi='/', 
	Assign='=', Comma=',', Semicolon=';',
	ProgDecl=150, ProgBegin, ProgEnd, VarDecl, 
	IntDecl, DblDecl, Print, END_KeyList,
	Letter, Digit, Ident, IntLitr, DblLitr,
	EofLine, EofProg, Others
};

/* ��ū ���� */
struct Token {
	TknKind kind;    /* ��ū ����           */
	string  text;    /* ��ū ���ڿ�         */
	double  dblVal;  /* ��ġ ����� ���� �� */
	Token() { kind = Others; text = ""; dblVal = 0.0; }
	Token(TknKind k) { kind = k; text = ""; dblVal = 0.0; }
	Token(TknKind k, double d) { kind = k; text = ""; dblVal = d; }
	Token(TknKind k, const string& s) { kind = k; text = s; dblVal = 0.0; }
	Token(TknKind k, const string& s, double d) { kind = k; text = s; dblVal = d; }
};

/* �ڷ��� �̸� */
enum DtType { 
	NonType, IntType, DblType
};

/* �ɺ� ���̺� ��� ���� */
struct SymElem {
	string  name;    /* ������ �̸�   */
	char    dtTyp;   /* ������ �ڷ��� */
	int     adrs;    /* ������ �ּ�   */
	SymElem() { clear(); }
	void clear() { name = ""; dtTyp = NonType; adrs = 0; }
};

/* �ڵ� ���� */
struct Code {
	TknKind kind;    /* �ڵ� ����               */
	double  dblVal;  /* ��ġ ����� ���� ��     */
	int     symIdx;  /* �ɺ� ���̺��� ÷�� ��ġ */
	Code() { clear(); }
	Code(TknKind k) { clear(); kind = k; }
	Code(TknKind k, double d) { clear(); kind = k; dblVal = d; }
	Code(TknKind k, int idx) { clear(); kind = k; symIdx = idx;	}
	void clear() { kind = Others; dblVal = 0.0; symIdx = -1; }
};

/* �� ���� ��ü */
struct Tobj {
	char type;  /* ������ 'd':double, 's':string, '-':���� */
	double d;
	string s;
	Tobj()                 { type = '-'; d = 0.0; s = ""; }
	Tobj(double dt)        { type = 'd'; d = dt;  s = ""; }
	Tobj(const string& st) { type = 's'; d = 0.0; s = st; }
	Tobj(const char *st)   { type = 's'; d = 0.0; s = st; }
};

/* ������ �޸� */
class MyMemory {
private:
	vector<double> mem;
public:
	void auto_resize(int n) {  /* ��Ȯ�� Ƚ�� ������ ���� Ȯ�� */
		if(n >= (int)mem.size()) { 
			n = (n/256 + 1) * 256; 
			mem.resize(n); 
		}
	}
	void set(int adrs, double dt) {	mem[adrs] = dt; }   /* �޸� ����     */
	void add(int adrs, double dt) { mem[adrs] += dt; }  /* �޸� ���ϱ�   */
	double get(int adrs) { return mem[adrs]; }          /* �޸� �о�� */
	int size() { return (int)mem.size(); }              /* ���� ũ��       */
	void resize(unsigned int n) { mem.resize(n); }      /* ũ�� Ȯ��       */
};

