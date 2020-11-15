#include<stdio.h>
#include<string>
#include<sstream>
#include<iostream>
using namespace std;
extern FILE* yyin;
extern int yylineno;
extern char* yytext;
extern "C"{
	int yylex();
}
string errOut="";
string corrOut="";
bool wrongFlag=false;
string getNum(char* yytext){
	int num=0;
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
	return to_string(num);
}
string getFloat(char* yytext){
	string text=yytext;
	stringstream ss(text);
	float f;
	ss>>f;
	return to_string(f);
}
int main(int argc,char** argv){
	if(argc>1){
		if(!(yyin=fopen("/root/Lab/src/test0.cmm","r"))){
			perror(argv[1]);
			return 1;
		}
	}
	int no;
	//while(yylex()!=0);
	while(true){
		no=yylex();
		if(no==0) break;
		string token=yytext;
		string msg;
		switch(no){
			case 1://ws
				break;
			case 2:	//if
				msg="IF "+token+" at Line "+to_string(yylineno)+".\n";
				corrOut+=msg;
				break;
			case 3://else
				msg="ELSE "+token+" at Line "+to_string(yylineno)+".\n";
				corrOut+=msg;
				break;
			case 9://integer
				msg="INT "+getNum(yytext)+" at Line "+to_string(yylineno)+".\n";
				corrOut+=msg;
				break;
			case 10://float
				msg="FLOAT "+getFloat(yytext)+" at Line "+to_string(yylineno)+".\n";
				corrOut+=msg;
				break;
			case 100://err
				msg="Error type A at Line "+to_string(yylineno)+": Mysterious character:\""+token+"\".\n";
				wrongFlag=true;
				errOut+=msg;
				break;
			default:
				cout<<"token代号错误～"<<endl;
		}
	}
	if(wrongFlag) cerr<<errOut<<endl;
	else cerr<<corrOut<<endl;
	return 0;
}
