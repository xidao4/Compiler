#include<stdio.h>
#include<string>
#include<sstream>
#include<iostream>
//#include "lex.yy.c"
using namespace std;


/*
extern FILE* yyin;
extern int yylineno;
extern char* yytext;
*/
extern struct Node* root;
extern int syntaxErr;
extern "C"{
	int yyparse();
	int yyrestart(FILE*);
	void tree_search(struct Node* cur,int depth);
	//#include "lex.yy.c"
	//#include "syntax.tab.h"
}


int main(int argc,char** argv){
	if(argc<=1) return 1;
	//"/root/Lab/src/test.cmm"
	FILE* f=fopen(argv[1],"r");
	//FILE* f=fopen("/root/LAB2-TESTS/Tests_1_Normal/Tests(normal)/Tests/C_1.cmm","r");
	if(!f){
		perror(argv[1]);
		return 1;
	 }
	// if(!f){
	// 	fprintf(stderr,"cannot open the file!\n");
	// 	return 1;
	// }
	yyrestart(f);
	yyparse();

	if(syntaxErr==0){
		tree_search(root,0);
	}
	return 0;
}

/*
string errOut="";
string corrOut="";
bool wrongFlag=false;
*/
/*
string getNum(char* yytext){
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
	return to_string(num);
}
*/
/*
string getFloat(char* yytext){
	string text=yytext;
	stringstream ss(text);
	double f;
	ss>>f;
	return to_string(f);
}
*/

/*
int main(int argc,char** argv){
	if(argc<=1) return 1;
	FILE* F = fopen("/root/Lab/src/test0.cmm","r");
	if(!f){
		perror(argv[1]);
		return 1;	
	}


	int no;
	string tokens[]={"","","IF","ELSE","STRUCT","RETURN","WHILE","TYPE","ID",
		"","","RELOP","PLUS","ASSIGNOP","SEMI","LP","RP","LB","RB",
		"LC","RC","COMMA","MINUS","STAR","DIV","AND","OR","DOT","NOT"};
	//while(yylex()!=0);
	while(true){
		no=yylex();
		if(no==0) break;
		string token=yytext;
		string msg;
		switch(no){
			case 1://ws
				break;
			case 9://integer
				msg="INT "+getNum(yytext)+" at Line "+to_string(yylineno)+".\n";
				corrOut+=msg;
				break;
			case 10://float
				//msg="FLOAT "+getFloat(yytext)+" at Line "+to_string(yylineno)+".\n";
				msg="FLOAT "+to_string(atof(yytext))+" at Line "+to_string(yylineno)+".\n";
				corrOut+=msg;
				break;
			case 100://err
				msg="Error type A at Line "+to_string(yylineno)+": Mysterious character \""+token+"\".\n";
				wrongFlag=true;
				errOut+=msg;
				break;
			default:
				msg=tokens[no]+" "+token+" at Line "+to_string(yylineno)+".\n";
				corrOut+=msg;
		}
	}
	if(wrongFlag) cerr<<errOut;
	else cerr<<corrOut;
	return 0;
	
}
*/
