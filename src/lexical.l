%{
	#include "stdio.h"
%}
%option yylineno

ws		[ \t\n\r]+
digit		[0-9]
sixteenBit	[0-9a-fA-F]
eightBit	[0-7]
letter		[_a-zA-Z]			
integer		0|[1-9]{digit}{0,32}|0[xX]{sixteenBit}+|0{eightBit}+
float		(0|([1-9]{digit}*))\.{digit}+|({digit}*\.{digit}+|{digit}+\.)[eE][+-]?{digit}+
id		({letter})({letter}|{digit})*
relop		">"|"<"|">="|"<="|"=="|"!="
type		"int"|"float"

%%
{ws}		{}
"if"		{return 2;}
"else"		{return 3;}
"struct"	{return 4;}
"return"	{return 5;}
"while"		{return 6;}
{type}		{return 7;}
{id}		{return 8;}
{integer}	{return 9;}  
{float}		{return 10;}  
{relop}		{return 11;}
"+"		{return 12;}
"="		{return 13;}
";"		{return 14;}
"("		{return 15;}
")"		{return 16;}
"["		{return 17;}
"]"		{return 18;}
"{"		{return 19;}
"}"		{return 20;}
","		{return 21;}
"-"		{return 22;}
"*"		{return 23;}
"/"		{return 24;}
"&&"		{return 25;}
"||"		{return 26;}
"."		{return 27;}
"!"		{return 28;}
"//"		{     
			char c=input();
			while(c!='\n') c=input();
		}
"/*"		{
			char c=input();
			while(1){
				char d=input();
				if(c=='*'&&d=='/') break;
				c=d;
			}
		}
.		{return 100;}

%%

