%{
    void yyerror(const char* msg);
    int yyparse(void);
    #include "lex.yy.c"
	#include <stdio.h>
	#include <stdarg.h>
	extern int yylineno;
	int syntaxErr=0;
	
	void buildSyntaxTree(char* faName,int num_args,...);
	void tree_search(struct Node* cur,int depth);
	extern struct Node* root; 
%}


/* declared types */
%union{
	struct Node* node;
}

%locations

/* declared tokens */
%token <node> SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR DIV AND OR DOT NOT TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE ID INT FLOAT
%type <node> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier OptTag Tag VarDec FunDec VarList ParamDec CompSt StmtList Stmt DefList Def DecList Dec Exp Args

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right UMINUS NOT	//负号用法
%left LP RP LB RB DOT 
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%
/*High-Level Definations*/
Program:ExtDefList {
	buildSyntaxTree("Program",1,$1);
	root=$$;
}
    ;
ExtDefList:ExtDef ExtDefList{
	buildSyntaxTree("ExtDefList",2,$1,$2);
}
	| {
		$$=NULL;
	}
	;
ExtDef:Specifier ExtDecList SEMI  {
	buildSyntaxTree("ExtDef",3,$1,$2,$3);
}
	|Specifier SEMI	{
		buildSyntaxTree("ExtDef",2,$1,$2);
	}
	|Specifier FunDec CompSt{
		buildSyntaxTree("ExtDef",3,$1,$2,$3);
	}	
	;
ExtDecList:VarDec{
	buildSyntaxTree("ExtDecList",1,$1);
}
	|VarDec COMMA ExtDecList{
		buildSyntaxTree("ExtDecList",3,$1,$2,$3);
	}
	;
/*Specifier*/
Specifier:TYPE {
	buildSyntaxTree("Specifier",1,$1);
}
	|StructSpecifier {
		buildSyntaxTree("Specifier",1,$1);
	}
	;
StructSpecifier:STRUCT OptTag LC DefList RC {
	buildSyntaxTree("StructSpecifier",5,$1,$2,$3,$4,$5);
}
	|STRUCT Tag {
		buildSyntaxTree("StructSpecifier",2,$1,$2);
	}
	;
OptTag:ID{
	buildSyntaxTree("OptTag",1,$1);
}
	|{
		$$=NULL;
	}
	;
Tag:ID {
	buildSyntaxTree("Tag",1,$1);
}
	;


/*Declaratots*/
VarDec:ID {
	buildSyntaxTree("VarDec",1,$1);
}
	| VarDec LB INT RB {
		buildSyntaxTree("VarDec",4,$1,$2,$3,$4);
	}
	;
FunDec:ID LP VarList RP {
	buildSyntaxTree("FunDec",4,$1,$2,$3,$4);
}
	|ID LP RP {
		buildSyntaxTree("FunDec",3,$1,$2,$3);
	}
	;
VarList:ParamDec COMMA VarList {
	buildSyntaxTree("VarList",3,$1,$2,$3);
}
	|ParamDec{
		buildSyntaxTree("VarList",1,$1);
	}
	;
ParamDec:Specifier VarDec {
	buildSyntaxTree("ParamDec",2,$1,$2);
}
    ;

/*Statement*/
CompSt:LC DefList StmtList RC {
	buildSyntaxTree("CompSt",4,$1,$2,$3,$4);
}
	;
StmtList:Stmt StmtList{
	buildSyntaxTree("StmtList",2,$1,$2);
}
	| {
		$$=NULL;
	}
	;
Stmt:Exp SEMI {
	buildSyntaxTree("Stmt",2,$1,$2);
}
	|CompSt{
		buildSyntaxTree("Stmt",1,$1);
	}
	|RETURN Exp SEMI {
		buildSyntaxTree("Stmt",3,$1,$2,$3);
	}
	|IF LP Exp RP Stmt {
		buildSyntaxTree("Stmt",4,$1,$2,$3,$4);
	}
	|IF LP Exp RP Stmt ELSE Stmt {
		buildSyntaxTree("Stmt",7,$1,$2,$3,$4,$5,$6,$7);
	}
	|WHILE LP Exp RP Stmt {
		buildSyntaxTree("Stmt",5,$1,$2,$3,$4,$5);
	}
	;


/*Local Definations*/
DefList:Def DefList{
	buildSyntaxTree("DefList",2,$1,$2);
}
	| {
		$$=NULL;
	}
	;
Def:Specifier DecList SEMI {
	buildSyntaxTree("Def",3,$1,$2,$3);
}
	;
DecList:Dec {
	buildSyntaxTree("DecList",1,$1);
}
	|Dec COMMA DecList {
		buildSyntaxTree("DecList",3,$1,$2,$3);
	}
	;
Dec:VarDec {
	buildSyntaxTree("Dec",1,$1);
}
	|VarDec ASSIGNOP Exp {
		buildSyntaxTree("Dec",3,$1,$2,$3);
	}
	;
/*Expressions*/
Exp:Exp ASSIGNOP Exp{
	buildSyntaxTree("Exp",3,$1,$2,$3);
}
        |Exp AND Exp{
	buildSyntaxTree("Exp",3,$1,$2,$3);
}
        |Exp OR Exp{
	buildSyntaxTree("Exp",3,$1,$2,$3);
}
        |Exp RELOP Exp{
	buildSyntaxTree("Exp",3,$1,$2,$3);
}
        |Exp PLUS Exp{
	buildSyntaxTree("Exp",3,$1,$2,$3);
}
        |Exp MINUS Exp{
	buildSyntaxTree("Exp",3,$1,$2,$3);
}
        |Exp STAR Exp{
	buildSyntaxTree("Exp",3,$1,$2,$3);
}
        |Exp DIV Exp{
	buildSyntaxTree("Exp",3,$1,$2,$3);
}
        |LP Exp RP{
	buildSyntaxTree("Exp",3,$1,$2,$3);
}
        |MINUS Exp {
	buildSyntaxTree("Exp",2,$1,$2);
}
        |NOT Exp  {
	buildSyntaxTree("Exp",2,$1,$2);
}
        |ID LP Args RP{
	buildSyntaxTree("Exp",4,$1,$2,$3,$4);
}
        |ID LP RP {
	buildSyntaxTree("Exp",3,$1,$2,$3);
}
        |Exp LB Exp RB{
	buildSyntaxTree("Exp",4,$1,$2,$3,$4);
}
        |Exp DOT ID {
	buildSyntaxTree("Exp",3,$1,$2,$3);
}
        |ID {
	buildSyntaxTree("Exp",1,$1);
}
        |INT{
	buildSyntaxTree("Exp",1,$1);
}
        |FLOAT{
	buildSyntaxTree("Exp",1,$1);
}
        ;
Args:Exp COMMA Args{
	buildSyntaxTree("Args",3,$1,$2,$3);
}
        |Exp {
	buildSyntaxTree("Args",1,$1);
}
        ;



%%
//#include "lex.yy.c"
void yyerror(const char* msg){
	fprintf(stderr,"Error type B at Line %d: %s.\n",yylineno,msg);
}
void buildSyntaxTree(char* faName,int num_args,...){
	struct Node* fa=(struct Node*)malloc(sizeof(struct Node));
	strcpy(fa->name,faName);
	fa->type=SYNTACTIC_UNIT;
	fa->next_sib=NULL;

	va_list sons;
	va_start(sons,num_args);
	struct Node* tmp=va_arg(sons,struct Node*);
	fa->child=tmp;
	fa->lineno=tmp->lineno;
	for(int i=0;i<num_args;i++){
		tmp->next_sib=va_arg(sons,struct Node*);
		if(tmp->next_sib!=NULL){
			tmp=tmp->next_sib;
		}
	}
}
void tree_search(struct Node* cur,int depth){
	if(cur==NULL) return;
	for(int i=0;i<depth;i++){
		fprintf(stderr,"  ");
  	}
	fprintf(stderr,"%s",cur->name);
	if(cur->type==SYNTACTIC_UNIT){
		fprintf(stderr," (%d)",cur->lineno);
	}else if(cur->type==LEX_INT){
		fprintf(stderr,": %d",cur->int_constant);
	}else if(cur->type==LEX_FLOAT){
		fprintf(stderr,": %f",cur->float_constant);
	}else if(cur->type==LEX_ID||cur->type==LEX_TYPE){
      	fprintf(stderr,": %s",cur->str_constant);
    }else if(cur->type==LEX_OTHER_TOKEN){
      ;
    }
	fprintf(stderr,"\n");
  	tree_search(cur->child,depth+1);
  	tree_search(cur->next_sib,depth);

}
// int main(){
// 	yyparse();
// }
