%{
	#include "stdio.h"
	#include "syntax.tab.h"

	typedef enum{
		LEX_INT=0,
		LEX_FLOAT=1,
		LEX_ID=2,
		LEX_TYPE=3,
		LEX_OTHER_TOKEN=4,
		SYNTACTIC_UNIT
	}NODE_TYPE;

	struct Node{
		struct Node* child;
		struct Node* next_sib;
		char name[32];
		union{
			int int_constant;
			float float_constant;
			char str_constant[32];
		};
		int lineno;
		NODE_TYPE type;
	};

	struct Node* root;

	void createTokenNode(char* name,NODE_TYPE type);
	int getNum(char* yytext);

	int yycolumn=1;//列号，碰到\n要恢复成1
	#define YY_USER_ACTION \
		yylloc.first_line=yylloc.last_line=yylineno; \
		yylloc.first_column=yycolumn; \
		yylloc.last_column=yycolumn+yyleng-1; \
		yycolumn+=yyleng;

%}

%option yylineno

ws			[ \t\r]+
digit		[0-9]
sixteenBit	[0-9a-fA-F]
eightBit	[0-7]
letter		[_a-zA-Z]			
integer		0|[1-9]{digit}{0,32}|0[xX]{sixteenBit}+|0{eightBit}+
float		(0|([1-9]{digit}*))\.{digit}+|({digit}*\.{digit}+|{digit}+\.)[eE][+-]?{digit}+
id			({letter})({letter}|{digit})*
relop		">"|"<"|">="|"<="|"=="|"!="
type		"int"|"float"

%%
"\n"		{yycolumn=1;}
{ws}		{}
"if"		{createTokenNode("IF",LEX_OTHER_TOKEN);
			return IF;}
"else"		{createTokenNode("ELSE",LEX_OTHER_TOKEN);
			return ELSE;}
"struct"	{createTokenNode("STRUCT",LEX_OTHER_TOKEN);
			return STRUCT;}
"return"	{createTokenNode("RETURN",LEX_OTHER_TOKEN);
			return RETURN;}
"while"		{createTokenNode("WHILE",LEX_OTHER_TOKEN);
			return WHILE;}
{type}		{createTokenNode("TPYE",LEX_TYPE);
			strcpy(yylval.node->str_constant,yytext);
			return TYPE;}
{id}		{createTokenNode("ID",LEX_ID);
			strcpy(yylval.node->str_constant,yytext);
			return ID;}
{integer}	{createTokenNode("INT",LEX_INT);
			yylval.node->int_constant=4;
			return INT;	}  
{float}		{createTokenNode("FLOAT",LEX_FLOAT);
			yylval.node->float_constant=0.4;
			return FLOAT;
			}  
{relop}		{createTokenNode("RELOP",LEX_OTHER_TOKEN);
			return RELOP;}
"+"		{createTokenNode("PLUS",LEX_OTHER_TOKEN);
		return PLUS;}
"="		{createTokenNode("ASSIGNOP",LEX_OTHER_TOKEN);
		return ASSIGNOP;}
";"		{createTokenNode("SEMI",LEX_OTHER_TOKEN);
		return SEMI;}
"("		{createTokenNode("LP",LEX_OTHER_TOKEN);
		return LP;}
")"		{createTokenNode("RP",LEX_OTHER_TOKEN);
		return RP;}
"["		{createTokenNode("LB",LEX_OTHER_TOKEN);
		return LB;}
"]"		{createTokenNode("RB",LEX_OTHER_TOKEN);
		return RB;}
"{"		{createTokenNode("LC",LEX_OTHER_TOKEN);
		return LC;}
"}"		{createTokenNode("RC",LEX_OTHER_TOKEN);
		return RC;}
","		{createTokenNode("COMMA",LEX_OTHER_TOKEN);
		return COMMA;}
"-"		{createTokenNode("MINUS",LEX_OTHER_TOKEN);
	return MINUS;}
"*"		{createTokenNode("STAR",LEX_OTHER_TOKEN);
	return STAR;}
"/"		{createTokenNode("DIV",LEX_OTHER_TOKEN);
	return DIV;}
"&&"	{createTokenNode("AND",LEX_OTHER_TOKEN);
	return AND;}
"||"	{createTokenNode("OR",LEX_OTHER_TOKEN);
	return OR;}
"."		{createTokenNode("DOT",LEX_OTHER_TOKEN);
	return DOT;}
"!"		{createTokenNode("NOT",LEX_OTHER_TOKEN);
	return NOT;}
"//"	{     
			char c=input();
			while(c!='\n') c=input();
		}
"/*"	{
			char c=input();
			while(1){
				char d=input();
				if(c=='*'&&d=='/') break;
				c=d;
			}
		}
.		{
			fprintf(stderr,"Error type A at Line %d: Mystery character \'%s\'.\n",yylineno,yytext);
		}

%%
void createTokenNode(char* name,NODE_TYPE type){
	yylval.node=(struct Node*)malloc(sizeof(struct Node));
	yylval.node->child=NULL;
	yylval.node->next_sib=NULL;
	yylval.node->lineno=yylineno;
	strcpy(yylval.node->name,name);
	yylval.node->type=type;
}

int getNum(char* yytext){
	unsigned int num=0;
	if(*yytext=='0'&&(*(yytext+1)=='x'||*(yytext+1)=='X')){
		for(int i=2;;i++){
			char c=*(yytext+i);
			if(c=='\0') break;
			num*=16;
			int bit=(c>='a')?(c-'a'+10):((c>='A')?(c-'A'+10):(c-'0'));
			num+=bit;
		}	
	}else if(*yytext=='0'){
		for(int i=1;;i++){
			if(*(yytext+i)=='\0') break;
			num*=8;
			num+=*(yytext+i)-'0';
		}
	}else{
		for(int i=0;;i++){
			if(*(yytext+i)=='\0') break;
			num*=10;
			num+=*(yytext+i)-'0';
		}
	}
	return num;
}
// double getFloat(char* yytext){
// 	string text=yytext;
// 	stringstream ss(text);
// 	double f;
// 	ss>>f;
// 	return f;
// }

