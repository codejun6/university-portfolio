/************************************************************************/
/*    Filename: mcpli_tkn.cpp                                           */
/*              ��ū ó��(���� �м�)                                    */
/*    Mini-CPL Interpreter (Ver. 1.00) Copyright (c) 2015 Minsu Jeon    */
/************************************************************************/
#include "mcpli.h"
#include "mcpli_prot.h"

/* Ű���� ������ ���� */
struct Keyword {
	const char *keyName;  /* Ű���� ���ڿ�   */
	TknKind keyKind;      /* �����ϴ� Ű���� */
};

/* Ű���� ���� ���̺� */
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

TknKind g_chtyp[256];   /* ���� ����ǥ �迭           */
ifstream g_fin;         /* �Է� ��Ʈ��                */
int g_srcLineno;        /* �ҽ��� �� ��ȣ             */
bool g_fEndOfFile;      /* ���� ���� �÷���           */
char g_buf[LIN_SIZ+5];  /* �ҽ� �� ���� �о���� ���� */
char *g_pBuf;           /* ��ū �м��� ���� ������    */
#define MAX_LINE 2000   /* �ִ� ���α׷� ���         */

/* ���� ����ǥ ���� */
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

/* ���� ���� */
void fileopen(char *fname)
{
	g_srcLineno = 0;
	g_fEndOfFile = false;
	g_fin.open(fname);
	if(!g_fin) { 
		cout << fname << "��(��) �� �� �����ϴ�.\n"; 
		system("PAUSE");
		exit(1); 
	}
}

/* ���� �� �б� */
void nextline()
{
	string s;

	if (g_fEndOfFile) return;
	g_fin.getline(g_buf, LIN_SIZ+5);  // �� �� �б�
	if (g_fin.eof()) {  // ���� ����
		g_fin.clear(); 
		g_fin.close();
		g_fEndOfFile = true; 
		return;
	}

	if (strlen(g_buf) > LIN_SIZ)
		err_exit("���α׷��� 1�� ", LIN_SIZ, " �����̳��� ����� �ּ���");
	if (++g_srcLineno > MAX_LINE)
		err_exit("���α׷��� ", MAX_LINE, " ��(��) �Ѿ����ϴ�.");

	g_pBuf = g_buf;  // ��ū �м��� �����͸� ������ �� �տ� ��ġ��Ų��
}

/* ���� ��ū�� ��ȯ */
#define CH (*g_pBuf)          // ���� ���� �б�
#define C2 (*(g_pBuf + 1))    // ���� ���� �б�
#define NEXT_CH() (++g_pBuf)  // ���� ���ڸ� ����Ų��
Token nexttkn()
{
	TknKind kd;
	string txt = "";

	if(g_fEndOfFile) return Token(EofProg);  // ���� ����
	while(isspace(CH)) NEXT_CH();            // ���� ����
	if(CH == '\0') return Token(EofLine);    // ���� ��

	switch(g_chtyp[CH]) {
	// ����('_' ����)
	case Letter:
		txt += CH; NEXT_CH();
		while(g_chtyp[CH] == Letter || g_chtyp[CH] == Digit) { txt += CH; NEXT_CH(); }
		break;

	// ��ġ���
	case Digit:
		kd = IntLitr;
		while(g_chtyp[CH] == Digit) { txt += CH; NEXT_CH(); }
		if(CH == '.') { kd = DblLitr; txt += CH; NEXT_CH(); }
		while(g_chtyp[CH] == Digit) { txt += CH; NEXT_CH(); }
		return Token(kd, txt, atof(txt.c_str()));  // IntLitr�� double������ ����

	default:
		if(CH=='/' && C2=='/') return Token(EofLine);  // �ּ�
		else { txt += CH; NEXT_CH(); }
		break;
	}
	kd = get_kind(txt);  // ������ ����

	if (kd == Others) err_exit("�߸��� ��ū�Դϴ�: ", txt);
	return Token(kd, txt);
}

/* ��ū�� Ȯ�� �� ��ȯ */
Token chk_nexttkn(const Token& tk, int kind)
{
	if(tk.kind != kind){
		err_exit(err_msg(tk.text, kind_to_s(kind)));
	}
	return nexttkn();
}

/* ���� ���� �а�, ���� ��ū�� ��ȯ */
Token nextline_tkn()
{
	nextline();
	return nexttkn();
}

/* Ű���� ���ڿ��� ��ū ������ ��ȯ */
TknKind get_kind(const string& s)
{
	for(int i = 0; g_keyTbl[i].keyKind != END_KeyList; i++) {
		if(s == g_keyTbl[i].keyName) return g_keyTbl[i].keyKind;
	}
	if(g_chtyp[s[0]] == Letter) return Ident;
	if(g_chtyp[s[0]] == Digit) return DblLitr;
	return Others;  // ����
}

/* ������ Ű������ ���ڿ��� ��ȯ */
string kind_to_s(int kd)
{
	for(int i = 0; ; i++) {
		if(g_keyTbl[i].keyKind == END_KeyList) break;
		if(g_keyTbl[i].keyKind == kd) return g_keyTbl[i].keyName;
	}
	return "";
}

/*  ������ Ű������ ���ڿ��� ��ȯ */
string kind_to_s(const Code& cd)
{
	switch (cd.kind) {
	// ����
	case Ident: 
		return get_symiter(cd)->name;

	// ��ġ���
	case IntLitr: case DblLitr: 
		return dbl_to_s(cd.dblVal);

	// ���� ��
	case EofLine: 
		return "";
	}
	return kind_to_s(cd.kind);
}

/* ��ū �м��� ������ ���� */
void set_bufptr(char *p)
{
	g_pBuf = p;
}

/* �м��ϰų� ���� ���� �� ��ȣ�� ��ȯ */
int get_lineno()
{
	extern int g_pc;
	return (g_pc == -1)? g_srcLineno : g_pc;  // �м� �� : ���� ��
}

