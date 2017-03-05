/************************************************************************/
/*    Filename: mcpli_prot.h                                            */
/*              모든 함수의 프로토타입                                  */
/*    Mini-CPL Interpreter (Ver. 1.00) Copyright (c) 2015 Minsu Jeon    */
/************************************************************************/

/* mcpli_pars.cpp: 구문 분석 */
void init();
void convert_to_intercode(char *fname);
void convert();
void convert_rest();
void vardecl();
void chk_varname();
void chk_semicolon();
void set_tmpsym_name();
void set_tmpsym(char typ);
void set_code(int cd);
void set_code(int cd, int nbr);
void set_code_single();
void set_code_rest();
void set_code_eofline();
void push_intercode();

/* mcpli_tkn.cpp: 토큰 처리(어휘 분석) */
void init_chtyp();
void fileopen(char *fname);
void nextline();
Token nexttkn();
Token chk_nexttkn(const Token& tk, int kind);
Token nextline_tkn();
TknKind get_kind(const string& s);
string kind_to_s(int kd);
string kind_to_s(const Code& cd);
void set_bufptr(char *p);
int get_lineno();

/* mcpli_tbl.cpp: 심볼 테이블 처리 */
int enter(SymElem& elem);
int search_name(const string& s);
vector<SymElem>::iterator get_symiter(const Code& cd);

/* mcpli_code.cpp: 메모리 관리, 구문 검사, 실행 */
void syntax_check();
void set_startpc(int n);
void execute();
void statement();
double get_expression(int kind1=0, int kind2=0);
void expression(int kind1, int kind2);
void expression();
void term(int pri);
void factor();
char get_vartype(const Code& cd);
int get_varadrs(const Code& cd);
int get_op_order(TknKind kd);
void bin_operation(TknKind op);
void chk_sysfnc(TknKind kd);
void exec_sysfnc(TknKind kd);
Code nextcode();
Code chk_nextcode(const Code& cd, int kind);
Code firstcode(int line);
int enter_literal(double d);

/* mcpli_misc.cpp: 기타 함수 */
string dbl_to_s(double d);
string err_msg(const string& a, const string& b);
void err_exit(Tobj a="\1", Tobj b="\1", Tobj c="\1", Tobj d="\1");

