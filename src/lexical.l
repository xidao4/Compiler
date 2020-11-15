%{
	#include "stdio.h"
%}
%option yylineno

ws		[ \t\n\r]+
digit		[0-9]
sixteenBit	[0-9a-fA-F]
eightBit	[0-7]
letter		[a-zA-Z]			
integer		0|[1-9]{digit}{0,32}|0[xX]{sixteenBit}+|0{eightBit}+
float		(0|([1-9]{digit}*))\.{digit}+|({digit}*\.{digit}+|{digit}+\.)[eE][+-]?{digit}+
id		({letter})({letter}|{digit})*
relop		">"|"<"|">="|"<="|"=="|"!="
type		"int"|"float"
%%
{ws}			{}
"if"			{return 2;}
"else"			{return 3;}
"struct"	{printf("STRUCT %s at Line %d.\n",yytext,yylineno);}
"return"	{printf("RETURN %s at Line %d.\n",yytext,yylineno);}
"while"		{printf("WHILE %s at Line %d.\n",yytext,yylineno);}
{type}			{printf("TYPE %s at Line %d.\n",yytext,yylineno);}
{id}			{printf("ID %s at Line %d.\n",yytext,yylineno);}
{integer}		{return 9;}  
{float}		{return 10;}  
{relop}			{printf("RELOP %s at Line %d.\n",yytext,yylineno);}
"+"			{printf("PLUS %s at Line %d.\n",yytext,yylineno);}
"="			{printf("ASSIGNOP %s at Line %d.\n",yytext,yylineno);}
";"			{printf("SEMI %s at Line %d.\n",yytext,yylineno);}
"("		{printf("LP %s at Line %d.\n",yytext,yylineno);}
")"		{printf("RP %s at Line %d.\n",yytext,yylineno);}
"["		{printf("LB %s at Line %d.\n",yytext,yylineno);}
"]"		{printf("RB %s at Line %d.\n",yytext,yylineno);}
"{"		{printf("LC %s at Line %d.\n",yytext,yylineno);}
"}"		{printf("RC %s at Line %d.\n",yytext,yylineno);}
","		{printf("COMMA %s at Line %d.\n",yytext,yylineno);}
"-"		{printf("MINUS %s at Line %d.\n",yytext,yylineno);}
"*"		{printf("STAR %s at Line %d.\n",yytext,yylineno);}
"/"		{printf("DIV %s at Line %d.\n",yytext,yylineno);}
"&&"		{printf("AND %s at Line %d.\n",yytext,yylineno);}
"||"		{printf("OR %s at Line %d.\n",yytext,yylineno);}
"."		{printf("DOT %s at Line %d.\n",yytext,yylineno);}
"!"		{printf("NOT %s at Line %d.\n",yytext,yylineno);}

.			{return 100;}

%%

