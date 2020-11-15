#include<stdio.h>
#include<string>
using namespace std;
extern FILE* yyin;
extern "C"{
	int yylex();
}
int main(int argc,char** argv){
	if(argc>1){
		if(!(yyin=fopen("/root/Lab/src/test0.cmm","r"))){
			perror(argv[1]);
			return 1;
		}
	}
	while(yylex()!=0);
	return 0;
}
