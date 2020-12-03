%{
    void yyerror(const char* msg);
    //int yyparse(void);
    #include "lex.yy.c"
	#include <stdio.h>
%}

/* declared tokens */
%token INT
%token ADD SUB MUL DIV


%%

Calc: /* empty */
	| Exp { printf( "= %d\n", $1); }
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
	;



%%

// void yyerror(const char* msg){
// 	fprintf(stderr,"error: %s\n",msg);
// }

// int main(){
// 	yyparse();
// }





