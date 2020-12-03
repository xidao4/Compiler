%{
	#include "stdio.h"
	#include "syntax.tab.h"
%}
%option yylineno

ws			[ \t\n\r]+
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
{ws}		{}
"if"		{return IF;}
"else"		{return ELSE;}
"struct"	{return STRUCT;}
"return"	{return RETURN;}
"while"		{return WHILE;}
{type}		{return TYPE;}
{id}		{return ID;}
{integer}	{
				yylval.type_int = atoi(yytext);
				return INT;	}  
{float}		{
				yylval.type_double=atof(yytext);
				return FLOAT;
			}  
{relop}		{return RELOP;}
"+"		{return PLUS;}
"="		{return ASSIGNOP;}
";"		{return SEMI;}
"("		{return LP;}
")"		{return RP;}
"["		{return LB;}
"]"		{return RB;}
"{"		{return LC;}
"}"		{return RC;}
","		{return COMMA;}
"-"		{return MINUS;}
"*"		{return STAR;}
"/"		{return DIV;}
"&&"	{return AND;}
"||"	{return OR;}
"."		{return DOT;}
"!"		{return NOT;}
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
.		{return 100;}

%%
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

