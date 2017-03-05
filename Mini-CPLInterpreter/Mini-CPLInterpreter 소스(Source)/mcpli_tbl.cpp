/************************************************************************/
/*    Filename: mcpli_tbl.cpp                                           */
/*              �ɺ� ���̺� ó��                                        */
/*    Mini-CPL Interpreter (Ver. 1.00) Copyright (c) 2015 Minsu Jeon    */
/************************************************************************/
#include "mcpli.h"
#include "mcpli_prot.h"

vector<SymElem> g_symTbl;  /* �ɺ� ���̺� */

/* �ɺ� ���̺� �ɺ��� ��� */
int enter(SymElem& elem)
{
	extern MyMemory g_dmem;  // �޸�
	int n, mem_size;

	mem_size = 1;

	n = search_name(elem.name);  // �ߺ� Ȯ��
	if(n != -1) {
		err_exit("�̸��� �ߺ��Ǿ����ϴ�: ", elem.name);
	}

	elem.adrs = g_dmem.size();                // �ּ� ����
	g_dmem.resize(g_dmem.size() + mem_size);  // �޸� ���� Ȯ��

	n = g_symTbl.size();
	g_symTbl.push_back(elem);  // �ɺ� ���̺� ����
	return n;  // ���� ��ġ ��ȯ
}

/* �ɺ� ���̺� �� ������ �˻� */
int search_name(const string& s)
{
	// �ɺ� ���̺� �˻�
	for(int n = 0; n < (int)g_symTbl.size(); n++) {
		if(g_symTbl[n].name == s) return n;
	}

	return -1;  // �߰ߵ��� ����
}

/* �ɺ� ���̺��� �ݺ��� ȹ�� */
vector<SymElem>::iterator get_symiter(const Code& cd)
{
	return g_symTbl.begin() + cd.symIdx;
}

