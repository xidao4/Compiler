%{
    //void yyerror(const char* msg);
    int yyparse(void);
    //#include "lex.yy.c"
	#include <stdio.h>
	#include<unistd.h>
%}


/* declared types */
%union{
	int type_int;
	//float type_float;
	double type_double;
}
/* declared tokens */
%token <type_double> SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR DIV AND OR DOT NOT TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE ID INT FLOAT ERROR
%type <type_double> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier OptTag Tag VarDec FunDec VarList ParamDec CompSt StmtList Stmt DefList Def DecList Dec Exp Args

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT 
%left LP RP LB RB DOT

%%
/*High-Level Definations*/
Program:ExtDefList 
    ;
ExtDefList:ExtDef ExtDefList
	| 
	;
ExtDef:Specifier ExtDecList SEMI  
	|Specifier SEMI	
	|Specifier FunDec CompSt	
	;
ExtDecList:VarDec
	|VarDec COMMA ExtDecList
	;
/*Specifier*/
Specifier:TYPE 
	|StructSpecifier 
	;
StructSpecifier:STRUCT OptTag LC DefList RC 
	|STRUCT Tag 
	;
OptTag:ID
	|
	;
Tag:ID 
	;


/*Declaratots*/
VarDec:ID 
	| VarDec LB INT RB 
	;
FunDec:ID LP VarList RP 
	|ID LP RP 
	;
VarList:ParamDec COMMA VarList 
	|ParamDec
	;
ParamDec:Specifier VarDec 
    ;

/*Statement*/
CompSt:LC DefList StmtList RC 
	;
StmtList:Stmt StmtList
	| 
	;
Stmt:Exp SEMI
	|CompSt
	|RETURN Exp SEMI 
	|IF LP Exp RP Stmt 
	|IF LP Exp RP Stmt ELSE Stmt 
	|WHILE LP Exp RP Stmt 
	;


/*Local Definations*/
DefList:Def DefList
	| 
	;
Def:Specifier DecList SEMI 
	;
DecList:Dec 
	|Dec COMMA DecList 
	;
Dec:VarDec
	|VarDec ASSIGNOP Exp 
	;
/*Expressions*/
Exp:Exp ASSIGNOP Exp
        |Exp AND Exp
        |Exp OR Exp
        |Exp RELOP Exp
        |Exp PLUS Exp
        |Exp MINUS Exp
        |Exp STAR Exp
        |Exp DIV Exp
        |LP Exp RP
        |MINUS Exp 
        |NOT Exp 
        |ID LP Args RP
        |ID LP RP 
        |Exp LB Exp RB
        |Exp DOT ID 
        |ID 
        |INT
        |FLOAT
        ;
Args:Exp COMMA Args
        |Exp 
        ;



%%
#include "lex.yy.c"
// void yyerror(const char* msg){
// 	fprintf(stderr,"error: %s\n",msg);
// }

// int main(){
// 	yyparse();
// }
