%{
    void yyerror(const char* msg);
    int yyparse(void);
    #include "lex.yy.c"
	#include <stdio.h>
	#include <stdarg.h>
	extern int yylineno;
	int syntaxErr=0;
	
	struct Node* buildSyntaxTree(int faLineno,char* faName,int num_args,...);
	void tree_search(struct Node* cur,int depth);
	struct Node* createSingleNode(int lineno,char* name);
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

%start Program
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
	$$=buildSyntaxTree(@$.first_line,"Program",1,$1);
	root=$$;
}
    ;
ExtDefList:ExtDef ExtDefList{
	$$=buildSyntaxTree(@$.first_line,"ExtDefList",2,$1,$2);
}
	| {
		//$$=NULL;
		$$=createSingleNode(@$.first_line,"ExtDefList");
	}
	;
ExtDef:Specifier ExtDecList SEMI  {
	$$=buildSyntaxTree(@$.first_line,"ExtDef",3,$1,$2,$3);
}
	|Specifier SEMI	{
		$$=buildSyntaxTree(@$.first_line,"ExtDef",2,$1,$2);
	}
	|Specifier FunDec CompSt{
		$$=buildSyntaxTree(@$.first_line,"ExtDef",3,$1,$2,$3);
	}
	|error SEMI{}
    |Specifier error SEMI{}
	|Specifier ExtDecList error SEMI{}	
	;
ExtDecList:VarDec{
	$$=buildSyntaxTree(@$.first_line,"ExtDecList",1,$1);
}
	|VarDec COMMA ExtDecList{
		$$=buildSyntaxTree(@$.first_line,"ExtDecList",3,$1,$2,$3);
	}
	;


/*Specifier*/
Specifier:TYPE {
	$$=buildSyntaxTree(@$.first_line,"Specifier",1,$1);
}
	|StructSpecifier {
		$$=buildSyntaxTree(@$.first_line,"Specifier",1,$1);
	}
	;
StructSpecifier:STRUCT OptTag LC DefList RC {
	$$=buildSyntaxTree(@$.first_line,"StructSpecifier",5,$1,$2,$3,$4,$5);
}
	|STRUCT Tag {
		$$=buildSyntaxTree(@$.first_line,"StructSpecifier",2,$1,$2);
	}
	|STRUCT OptTag LC error RC {}
	|STRUCT OptTag LC DefList error RC{}
	;
OptTag:ID{
	$$=buildSyntaxTree(@$.first_line,"OptTag",1,$1);
}
	|{
		$$=createSingleNode(@$.first_line,"OptTag");
	}
	;
Tag:ID {
	$$=buildSyntaxTree(@$.first_line,"Tag",1,$1);
}
	;


/*Declaratots*/
VarDec:ID {
	$$=buildSyntaxTree(@$.first_line,"VarDec",1,$1);
}
	| VarDec LB INT RB {
		$$=buildSyntaxTree(@$.first_line,"VarDec",4,$1,$2,$3,$4);
	}
	|VarDec LB error RB{}
	| VarDec LB INT error RB{} 
	;
FunDec:ID LP VarList RP {
	$$=buildSyntaxTree(@$.first_line,"FunDec",4,$1,$2,$3,$4);
}
	|ID LP RP {
		$$=buildSyntaxTree(@$.first_line,"FunDec",3,$1,$2,$3);
	}
	|ID LP error RP{ }
	|ID LP VarList error RP{}
	;
VarList:ParamDec COMMA VarList {
	$$=buildSyntaxTree(@$.first_line,"VarList",3,$1,$2,$3);
}
	|ParamDec{
		$$=buildSyntaxTree(@$.first_line,"VarList",1,$1);
	}
	;
ParamDec:Specifier VarDec {
	$$=buildSyntaxTree(@$.first_line,"ParamDec",2,$1,$2);
}
    ;



/*Statement*/
CompSt:LC DefList StmtList RC {
	$$=buildSyntaxTree(@$.first_line,"CompSt",4,$1,$2,$3,$4);
}
	| LC error RC
	|LC DefList error RC
	|LC DefList StmtList error RC
	;
StmtList:Stmt StmtList{
	$$=buildSyntaxTree(@$.first_line,"StmtList",2,$1,$2);
}
	| {
		$$=createSingleNode(@$.first_line,"StmtList");
	}
	;
Stmt:Exp SEMI {
	$$=buildSyntaxTree(@$.first_line,"Stmt",2,$1,$2);
}
	|CompSt{
		$$=buildSyntaxTree(@$.first_line,"Stmt",1,$1);
	}
	|RETURN Exp SEMI {
		$$=buildSyntaxTree(@$.first_line,"Stmt",3,$1,$2,$3);
	}
	|IF LP Exp RP Stmt {
		$$=buildSyntaxTree(@$.first_line,"Stmt",4,$1,$2,$3,$4);
	}
	|IF LP Exp RP Stmt ELSE Stmt {
		$$=buildSyntaxTree(@$.first_line,"Stmt",7,$1,$2,$3,$4,$5,$6,$7);
	}
	|WHILE LP Exp RP Stmt {
		$$=buildSyntaxTree(@$.first_line,"Stmt",5,$1,$2,$3,$4,$5);
	}
	|error SEMI {}
    |Exp error SEMI{}
    |RETURN error SEMI{ }
	|RETURN Exp error SEMI{}
	|IF LP error RP Stmt{}
	|IF LP error RP Stmt ELSE Stmt{}
	|WHILE LP error RP Stmt{}
	;


/*Local Definations*/
DefList:Def DefList{
	$$=buildSyntaxTree(@$.first_line,"DefList",2,$1,$2);
}
	| {
		$$=createSingleNode(@$.first_line,"DefList");
	}
	;
Def:Specifier DecList SEMI {
	$$=buildSyntaxTree(@$.first_line,"Def",3,$1,$2,$3);
}
	|Specifier error SEMI{}
	|error SEMI{}
	|Specifier DecList error SEMI{}
	;
DecList:Dec {
	$$=buildSyntaxTree(@$.first_line,"DecList",1,$1);
}
	|Dec COMMA DecList {
		$$=buildSyntaxTree(@$.first_line,"DecList",3,$1,$2,$3);
	}
	;
Dec:VarDec {
	$$=buildSyntaxTree(@$.first_line,"Dec",1,$1);
}
	|VarDec ASSIGNOP Exp {
		$$=buildSyntaxTree(@$.first_line,"Dec",3,$1,$2,$3);
	}
	;



/*Expressions*/
Exp:Exp ASSIGNOP Exp{
	$$=buildSyntaxTree(@$.first_line,"Exp",3,$1,$2,$3);
}
        |Exp AND Exp{
	$$=buildSyntaxTree(@$.first_line,"Exp",3,$1,$2,$3);
}
        |Exp OR Exp{
	$$=buildSyntaxTree(@$.first_line,"Exp",3,$1,$2,$3);
}
        |Exp RELOP Exp{
	$$=buildSyntaxTree(@$.first_line,"Exp",3,$1,$2,$3);
}
        |Exp PLUS Exp{
	$$=buildSyntaxTree(@$.first_line,"Exp",3,$1,$2,$3);
}
        |Exp MINUS Exp{
	$$=buildSyntaxTree(@$.first_line,"Exp",3,$1,$2,$3);
}
        |Exp STAR Exp{
	$$=buildSyntaxTree(@$.first_line,"Exp",3,$1,$2,$3);
}
        |Exp DIV Exp{
	$$=buildSyntaxTree(@$.first_line,"Exp",3,$1,$2,$3);
}
        |LP Exp RP{
	$$=buildSyntaxTree(@$.first_line,"Exp",3,$1,$2,$3);
}
        |MINUS Exp %prec UMINUS {
	$$=buildSyntaxTree(@$.first_line,"Exp",2,$1,$2);
}
        |NOT Exp  {
	$$=buildSyntaxTree(@$.first_line,"Exp",2,$1,$2);
}
        |ID LP Args RP{
	$$=buildSyntaxTree(@$.first_line,"Exp",4,$1,$2,$3,$4);
}
        |ID LP RP {
	$$=buildSyntaxTree(@$.first_line,"Exp",3,$1,$2,$3);
}
        |Exp LB Exp RB{
	$$=buildSyntaxTree(@$.first_line,"Exp",4,$1,$2,$3,$4);
}
        |Exp DOT ID {
	$$=buildSyntaxTree(@$.first_line,"Exp",3,$1,$2,$3);
}
        |ID {
	$$=buildSyntaxTree(@$.first_line,"Exp",1,$1);
}
        |INT{
	$$=buildSyntaxTree(@$.first_line,"Exp",1,$1);
}
        |FLOAT{
	$$=buildSyntaxTree(@$.first_line,"Exp",1,$1);
}
		|LP error RP{}
        |ID LP error RP{}
        |Exp LB error RB{}
		|Exp LB Exp error RB{}
		|MINUS error{}
        ;
Args:Exp COMMA Args{
	$$=buildSyntaxTree(@$.first_line,"Args",3,$1,$2,$3);
}
        |Exp {
	$$=buildSyntaxTree(@$.first_line,"Args",1,$1);
}
        ;



%%
void yyerror(const char* msg){
	fprintf(stderr,"Error type B at Line %d: %s.\n",yylineno,msg);
}
struct Node* buildSyntaxTree(int faLineno,char* faName,int num_args,...){
	struct Node* fa=(struct Node*)malloc(sizeof(struct Node));
	strcpy(fa->name,faName);
	fa->type=SYNTACTIC_UNIT;
	fa->next_sib=NULL;
	fa->lineno=faLineno;

	va_list sons;
	va_start(sons,num_args);
	struct Node* tmp=va_arg(sons,struct Node*);
	fa->child=tmp;
	//fa->lineno=tmp->lineno;
	for(int i=1;i<num_args;i++){
		tmp->next_sib=va_arg(sons,struct Node*);
		if(tmp->next_sib!=NULL){
			tmp=tmp->next_sib;
		}
	}
	va_end(sons);
	return fa;
}
struct Node* createSingleNode(int lineno,char* name){
	struct Node* node=(struct Node*)malloc(sizeof(struct Node));
	strcpy(node->name,name);
	node->type=SYNTACTIC_UNIT_EMPTY;
	node->next_sib=NULL;
	node->child=NULL;
	node->lineno=lineno;
	return node;
}
void tree_search(struct Node* cur,int depth){
	if(cur==NULL) return;

	if(cur->type!=SYNTACTIC_UNIT_EMPTY) {
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
	}
	tree_search(cur->child,depth+1);
	tree_search(cur->next_sib,depth);
	
}
// int main(){
// 	yyparse();
// }
