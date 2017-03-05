/************************************************************************/
/*    Filename: mcpli_pars.cpp                                          */
/*              ���� �м�                                               */
/*    Mini-CPL Interpreter (Ver. 1.00) Copyright (c) 2015 Minsu Jeon    */
/************************************************************************/
#include "mcpli.h"
#include "mcpli_prot.h"

Token g_token;                     /* ���� ó������ ��ū           */
SymElem g_tmpSym;                  /* �ӽ� ������ �ɺ�             */
bool g_fVarDecl;                   /* ���� ��������� ���� �÷���  */
string g_progTitle;                /* ���α׷��� */
char g_codebuf[LIN_SIZ+1];         /* ���� �ڵ� ������ ����        */
char *g_pCodebuf;                  /* ���� �ڵ� ������ ���� ������ */
extern vector<char*> g_intercode;  /* ��ȯ�� ��ģ ���� �ڵ� ����   */

/* ���� �м��� ���� �ʱ�ȭ */
void init()
{
	init_chtyp();  // ���� ����ǥ �ʱ�ȭ
	g_pCodebuf = g_codebuf;
	g_fVarDecl = false;
}

/* ���� �ڵ�� ��ȯ */
void convert_to_intercode(char *fname)
{
	init();  // �ʱ�ȭ

	push_intercode();  // 0���� �ʿ� �����Ƿ� �ſ��.
	fileopen(fname);   // ���� �б�

	g_token = nextline_tkn();
	while(g_token.kind != EofProg) {
		convert();  // ���� �ڵ�� ��ȯ ó��
	}

	set_startpc(1);  // 1����� ���α׷� ����
}

/* �� ���� �� �տ� �ִ� �ڵ带 ��ȯ ó�� */
void convert()
{
	switch (g_token.kind) {
	// ���α׷� �����
	case ProgDecl:
		set_code(g_token.kind);
		g_token = nexttkn();
		g_progTitle = g_token.text;  // ���α׷��� ����
		g_token = nexttkn();
		set_code_eofline();
		break;

	// ���� �����
	case VarDecl:
		g_fVarDecl = true;
		set_code_single();
		break;

	// ���α׷� ó����
	case ProgBegin:
		g_fVarDecl = false;
		set_code_single();
		break;

	// ���α׷� �����
	case ProgEnd:
		g_fVarDecl = false;
		set_code_single();
		while(g_token.kind != EofProg) {  // ���α׷� ����� ������ �ڵ�� �����Ѵ�.
			g_token = nextline_tkn();
		}
		break;

	// �ڷ��� ����
	case IntDecl:  // int�� ����
	case DblDecl:  // double�� ����
		vardecl();
		break;

	// print �����Լ�
	case Print:
		set_code(g_token.kind);
		g_token = nexttkn();
		convert_rest();
		break;

	// ������ �κ�
	default: 
		convert_rest();
		break;
	}
}

/* ������ �κ��� ��ȯ ó�� */
void convert_rest()
{
	int tblidx;

	while(1) {
		if(g_token.kind == Semicolon) {  // �ݵ�� �� ���� ���� �����ݷ����� ������ �Ѵ�.
			set_code(g_token.kind);
			g_token = nexttkn();

			// �����ݷ� ������ ���� ������ ������ �� �ȴ�.
			if(g_token.kind != EofLine) {
				err_exit("�߸��� ����Դϴ�: ", g_token.text);
			}
			break;
		}
		if(g_token.kind == EofLine) {  // �����ݷ��� ���� �ʰ� ���� ������ �� �ȴ�.
			err_exit("�߸��� ����Դϴ�: ", g_token.text);
		}

		switch(g_token.kind) {
		// �� �࿡�� ������ ���� ��ū�� ��Ÿ���� �� �ȴ�.
		case ProgDecl: case VarDecl: case ProgBegin: 
		case ProgEnd:  case Print:
			err_exit("�߸��� ����Դϴ�: ", g_token.text);
			break;

		// ����
		case Ident:
			set_tmpsym_name();
			if((tblidx = search_name(g_tmpSym.name)) == -1) {  // ��ϵ� ������ ����
				err_exit("������� ���� �����Դϴ�: ", g_token.text);
			}
			set_code(Ident, tblidx);
			continue;

		// ��ġ���
		case IntLitr: case DblLitr:
			set_code(g_token.kind, enter_literal(g_token.dblVal));
			break;

		// ������ ��
		default:
			set_code(g_token.kind);
			break;
		}
		g_token = nexttkn();
	}

	push_intercode();
	g_token = nextline_tkn();
}

/* ���� ����� */
void vardecl()
{
	// ���� ����ο����� ���� ������ �����ϴ�.
	if(!g_fVarDecl) {
		err_exit("���� ������ Var���������� �����մϴ�.");
	}

	char typ = DblType;
	if(g_token.kind == IntDecl) typ = IntType;

	set_code(g_token.kind);
	set_code_rest();  // �������� �״�� ����
	while(1) {
		g_token = nexttkn();
		chk_varname();                  // ������ Ȯ��
		set_tmpsym(typ);                // �̸�, �ڷ��� ����
		enter(g_tmpSym);                // �ɺ� ���̺� ���� ���
		if(g_token.kind != ',') break;  // ���� ����
	}
	chk_semicolon();  // ���� �����ݷ����� �������� Ȯ��
	set_code_eofline();
}

/* �̸� Ȯ�� */
void chk_varname()
{
	if(g_token.kind != Ident) {
		err_exit(err_msg(g_token.text, "����"));
	}
	if(search_name(g_token.text) != -1) {
		err_exit("������ �ߺ��Ǿ����ϴ�: ", g_token.text);
	}
}

/* �����ݷ� Ȯ�� */
void chk_semicolon()
{
	if(g_token.kind != Semicolon) {
		err_exit("�߸��� ����Դϴ�: ", g_token.text);
	}

	g_token = nexttkn();
}

/* �ӽ� ������ �ɺ��� �̸� ���� */
void set_tmpsym_name()
{
	if(g_token.kind != Ident) {
		err_exit("������ �ʿ��մϴ�: ", g_token.text);
	}

	g_tmpSym.clear();
	g_tmpSym.name = g_token.text;
	g_token = nexttkn();
}

/* �ӽ� ������ �ɺ� ���� */
void set_tmpsym(char typ)
{
	set_tmpsym_name();
	g_tmpSym.dtTyp = typ;
}

/* �ڵ� ���� */
void set_code(int cd)
{
	*g_pCodebuf++ = (char)cd;
}

/* �ڵ�� short�� ���� */
void set_code(int cd, int nbr)
{
	*g_pCodebuf++ = (char)cd;
	*SHORT_P(g_pCodebuf) = (short)nbr;
	g_pCodebuf += SHORT_SIZ;
}

/* ���� �ڵ� ���� */
void set_code_single()
{
	set_code(g_token.kind);
	g_token = nexttkn();
	set_code_eofline();
}

/* ������ �ؽ�Ʈ�� �״�� ���� */
void set_code_rest()
{
	extern char *g_pBuf;
	strcpy(g_pCodebuf, g_pBuf);
	g_pCodebuf += strlen(g_pBuf) + 1;
}

/* ���� ���� ó�� */
void set_code_eofline()
{
	if(g_token.kind != EofLine) {
		err_exit("�߸��� ����Դϴ�: ", g_token.text);
	}

	push_intercode();
	g_token = nextline_tkn();  // ���� ������ ����
}

/* ��ȯ�� ���� �ڵ带 ���� */
void push_intercode()
{
	int len;
	char *p;

	*g_pCodebuf++ = '\0';
	if((len = g_pCodebuf - g_codebuf) >= LIN_SIZ) {
		err_exit("��ȯ ���� ���� �ڵ尡 �ʹ� ��ϴ�. ���� �ٿ��ּ���.");
	}

	try {
		p = new char[len];	        // ���� �ڵ� ������ ���� �޸� Ȯ��
		memcpy(p, g_codebuf, len);  // ���� �ڵ� ������ ������ ������ ����
		g_intercode.push_back(p);   // ���� �ڵ� ����
	}
	catch(bad_alloc) {
		err_exit("�޸𸮸� Ȯ���� �� �����ϴ�"); 
	}

	g_pCodebuf = g_codebuf;  // ���� ó���� ����� ������ �� �� ������ ��ġ ����
}

