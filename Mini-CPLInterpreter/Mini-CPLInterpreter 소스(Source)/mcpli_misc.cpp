/************************************************************************/
/*    Filename: mcpli_misc.cpp                                          */
/*              ��Ÿ �Լ�                                               */
/*    Mini-CPL Interpreter (Ver. 1.00) Copyright (c) 2015 Minsu Jeon    */
/************************************************************************/
#include "mcpli.h"
#include "mcpli_prot.h"

/* ��ġ����� ���ڿ��� ��ȯ */
string dbl_to_s(double d)
{
	ostringstream ostr;  // ��¿� ��Ʈ�� Ȯ��
	ostr << d;           // ��� ��Ʈ���� ����
	return ostr.str();   // ���� ������ ��ȯ
}

/* ���� ���� ���� */
string err_msg(const string& a, const string& b)
{
	if(a == "") return b + "�� �ʿ��մϴ�.";
	if(b == "") return a + "�� �ٸ��� �ʽ��ϴ�.";
	return b + "�� " + a + " �տ� �ʿ��մϴ�.";
}

/* ������ ǥ���ϰ� ���� */
void err_exit(Tobj a, Tobj b, Tobj c, Tobj d)
{
	Tobj ob[4] = {a, b, c, d};
	cerr << get_lineno() << " ���� - ";

	for(int i = 0; i < 4 && ob[i].s != "\1"; i++) {
		if(ob[i].type == 'd') cout << ob[i].d;  // ��ġ ����
		if(ob[i].type == 's') cout << ob[i].s;  // ���ڿ� ����
	}
	cout << endl;
	system("PAUSE");
	exit(1);
}
