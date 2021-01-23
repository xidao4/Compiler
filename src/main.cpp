#include<stdio.h>
#include<string>
#include<sstream>
#include<iostream>
#include<fstream>
#include "SyntaxNode.h"
#include "InterCode.h"
#include "SemanticAnalysis.h"
#include <unordered_map>
using namespace std;

extern void Program(Node* n);//SemanticAnalysis.cpp
extern void printIR(InterCode* head);//InterCode.cpp
extern "C"{
	int yyparse();//extern from syntax.y Bison
	int yyrestart(FILE*);
	void tree_search(struct Node* cur,int depth);//Lab2
}
extern struct Node* root;//define in lexical.l, used in main.cpp and syntax.y
extern unordered_map<string, Type> functionMap;

InterCode code_head=NULL;
InterCode code_tail=NULL;
int Temp_Num=0;
int Label_Num=0;
//FILE* fp;
string outFileName;

void init_table(){
	Type retType=new struct Type_;
	retType->kind=Type_::BASIC;
	retType->u.basic=IS_INT;

	FuncList myread=new struct FuncList_;
	myread->name="read";
	myread->type=retType;
	myread->next=NULL;

	Type func_read=new struct Type_;
	func_read->kind=Type_::FUNCTION;
	func_read->u.myfunc=myread;

	functionMap.insert({"read",func_read});



	Type retType2=new struct Type_;
	retType2->kind=Type_::BASIC;
	retType2->u.basic=IS_INT;

	Type paramType=new struct Type_;
	paramType->kind=Type_::BASIC;
	paramType->u.basic=IS_INT;

	FuncList paramFuncList=new struct FuncList_;
	paramFuncList->name="output";
	paramFuncList->type=paramType;
	paramFuncList->next=NULL;

	FuncList mywrite=new struct FuncList_;
	mywrite->name="write";
	mywrite->type=retType2;
	mywrite->next=paramFuncList;

	Type func_write=new struct Type_;
	func_write->kind=Type_::FUNCTION;
	func_write->u.myfunc=mywrite;

	functionMap.insert({"write",func_write});
}
int main(int argc,char** argv){
	init_table();
	if(argc<=2) return 1;//"/root/Lab/src/test.cmm"     //./parser test1.cmm out1.ir
	FILE* f=fopen(argv[1],"r");  //FILE* f=fopen("/root/LAB2-TESTS/Tests_1_Normal/Tests(normal)/Tests/C_1.cmm","r");
	string tar(argv[2]);
	string outFileName=tar;
	
	//fp=fopen(argv[2],"w+");
	if(!f){
		perror(argv[1]);
		fprintf(stderr,"cannot open the file!\n");
		return 1;
	}
	yyrestart(f);
	yyparse();
	/*
	if(syntaxErr==0){
		tree_search(root,0);
	}*/
	//从ROOT开始调用Program开始符号进行遍历
	Program(root);
	cout<<endl<<endl<<endl<<endl;
	Trans_Program(root);
	printIR(code_head);//写到out1.ir中
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