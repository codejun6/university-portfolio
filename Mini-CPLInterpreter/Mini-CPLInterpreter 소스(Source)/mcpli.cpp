/************************************************************************/
/*    Filename: mcpli.cpp                                               */
/*              메인 처리                                               */
/*    Mini-CPL Interpreter (Ver. 1.00) Copyright (c) 2015 Minsu Jeon    */
/************************************************************************/
#include "mcpli.h"
#include "mcpli_prot.h"

int main(int argc, char *argv[])
{
	char filename[65] = {0,};

	cout << "Write Input Filename: ";
	cin >> filename;

	convert_to_intercode(filename);
	syntax_check();
	execute();

	system("PAUSE");
	return 0;
}
