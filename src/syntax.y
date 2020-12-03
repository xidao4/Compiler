%{
    void yyerror(const char* msg);
    //int yyparse(void);
    #include "lex.yy.c"
	#include <stdio.h>
%}


/* declared types */
%union{
	int type_int;
	//float type_float;
	double type_double;
}
/* declared tokens */
%token <tpye_int> INT
%token <type_double> FLOAT
%token ADD SUB MUL DIV
/* declared non-terminals */
%type <type_double> Exp Factor Term

%%

Calc: /* empty */
	| Exp { printf( "= %lf\n", $1); }
	;
Exp: Factor
	| Exp ADD Factor { $$ = $1 + $3; }
	| Exp SUB Factor { $$ = $1 - $3; }
	;
Factor: Term
	| Factor MUL Term { $$ = $1 * $3; }
	| Factor DIV Term { $$ = $1 / $3; }
	;
Term: INT
	| FLOAT
	;



%%

// void yyerror(const char* msg){
// 	fprintf(stderr,"error: %s\n",msg);
// }

// int main(){
// 	yyparse();
// }





