#include<stdio.h>
#include<stdlib.h>
#include<string>
//include<hash_map>
#include <unordered_map>
#include<sstream>
#include<iostream>
#include<cstring>
//include "lex.yy.c"
#include <cstdlib>
//include "tsl/ordered_map.h"
#include "SyntaxNode.h"
using namespace std;

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct FuncList_* FuncList;
typedef struct ArrayList_* ArrayList;
#define IS_INT 0
#define IS_FLOAT 1

//tsl::ordered_map<char*,Type> functionMap;
//tsl::ordered_map<char*,Type> decMap;
unordered_map<char*,Type> map;//放基本类型变量、数组变量、结构体变量、形式参数变量、结构体域名变量？？。。
unordered_map<char*,Type> structureMap;//只放结构体的定义（结构体名：类型）
unordered_map<char*,Type> functionMap;

struct Type_{
    enum{BASIC,ARRAY,STRUCTURE,FUNCTION,ERROR} kind;
    union{
        int basic;
        struct {Type elem; int size;} array;//元素类型与数组大小
        FieldList structure;//结构体类型是一个链表（图2-3）
        FuncList myfunc;//函数名
        int error_type;//错误类型
    }u;
};
struct ArrayList_{

};
struct FieldList_{
    char* name;//域名
    Type type;//域的类型
    FieldList tail;//下一个域的指针
};
struct FuncList_{
    char* name;//函数名&参数名
    Type type;//返回类型&参数类型
    FuncList next;
};

/*high-level definitions*/
void Program(Node* n);
void ExtDefList(Node* n);
void ExtDef(Node* n);
Type Specifier(Node* n);
/*struct*/
Type StructSpecifier(Node* n);
char* OptTag(Node* n);
void DefList_in_Struct(Node* n,char* optTag);
void Def_in_Struct(Node* n,char* optTag);
void DecList_in_Struct(Node* n,char* optTag,Type type);
void Dec_in_Struct(Node* n,char* optTag,Type type);
void VarDec_in_Struct(Node* n,char* optTag,Type type);//VarDec 结构体变量的声明
/*high-level definitions*/
void ExtDecList(Node* n,Type type);
/*declarition*/
void VarDec(Node* n,Type type);//VarDec 既可以是全局的变量，又可以在函数中使用来声明变量
/*definition in function*/
void FunDec(Node* n,Type return_type);
FuncList VarList(Node* n);
FuncList ParamDec(Node* n);
FuncList VarDec_in_FuncParams(Node* n,Type type);//VarDec 函数**参数**的变量声明
/*Statements*/
void CompSt(Node *n,Type return_type);
/*definition in function*/
void DefList_in_Function(Node* n);
void Def_in_Function(Node* n);
void DecList_in_Function(Node* n,Type dec_type);
void Dec_in_Function(Node* n,Type dec_type);
/*Statements*/
void StmtList(Node* n,Type return_type);
void Stmt(Node* n,Type return_type);
/*Expression*/
Type Exp(Node* n);
void Exp_ASSIGNOP(Node* n,Type dec_type);
Type Exp_Math(Node* n);
Type Exp_Logic(Node* n);
/*helper*/
bool isSameType(Type t1,Type t2);


void Program(Node* n){
    ExtDefList(n->child);
}
void ExtDefList(Node* n){
    if(n->type==SYNTACTIC_UNIT_EMPTY) return;
    ExtDef(n->child);
    ExtDefList(n->child->next_sib);
}
void ExtDef(Node* n){
    Type type=Specifier(n->child);
    if(strcmp(n->child->name,"ExtDecList")==0){
        // Specifier ExtDecList SEMI
        ExtDecList(n->child->next_sib,type);
    }else if(strcmp(n->child->name,"SEMI")==0){
        // Specifier SEMI
        ;
    }else if(strcmp(n->child->name,"FunDec")==0){
        //ExtDef -> Specifier FunDec CompSt
        FunDec(n->child->next_sib,type);
        CompSt(n->child->next_sib->next_sib,type);
    }
}


Type Specifier(Node* n){
    if(strcmp(n->child->name,"TYPE")==0){
        //Specifier -> TYPE
        Type type=(Type)malloc(sizeof(struct Type_));
        type->kind=Type_::BASIC;
        if(n->child->type==LEX_INT){
            type->u.basic=IS_INT;
        }
        else if(n->child->type==LEX_FLOAT)
            type->u.basic=IS_FLOAT;
        return type;
    }else{
        //Specifier -> StructSpecifier
        return StructSpecifier(n->child);
    }
}


Type StructSpecifier(Node* n){
    if(n->child->next_sib->next_sib!=NULL){
        // STRUCT OptTag LC DefList RC
        char* optTag=OptTag(n->child->next_sib);
        if(structureMap.find(optTag)!=structureMap.end()){
            fprintf(stderr,"Error Type 16 at Line %d: 结构体名\"%s\"与定义过的结构体或变量重复.\n",n->lineno,n->child->next_sib->name);
            Type err=(Type)malloc(sizeof(struct Type_));
            err->kind=Type_::ERROR;
            err->u.error_type=16;
            return err;
        }else{
            Type type=(Type)malloc(sizeof(struct Type_));
            type->kind=Type_::STRUCTURE;
            type->u.structure=NULL;
            structureMap.insert({optTag,type});
            DefList_in_Struct(n->child->next_sib->next_sib->next_sib,optTag);
        }
    }else{
        // STRUCT Tag
        char* tagName=n->child->next_sib->child->name;
        if(structureMap.find(tagName)==structureMap.end()){
            //理解为这样的形式 struct Complex c;
            fprintf(stderr,"Error Type 17 at Line %d: 直接使用未定义过的结构体名\"%s\"来定义变量.\n",n->lineno,tagName);
            Type err=(Type)malloc(sizeof(struct Type_));
            err->kind=Type_::ERROR;
            err->u.error_type=17;
            return err;
        }else{
            return structureMap[tagName];
        }
    } 
}
void DefList_in_Struct(Node* n,char* optTag){
    // DefList -> empty
    if (n->type==SYNTACTIC_UNIT_EMPTY) return;
    // DefList -> Def DefList
    Def_in_Struct(n->child,optTag);
    DefList_in_Struct(n->child->next_sib,optTag);
}
void Def_in_Struct(Node* n,char* optTag){
    //Def -> Specifier DecList SEMI
    Type type=Specifier(n->child);
    DecList_in_Struct(n->child->next_sib,optTag,type);
}
void DecList_in_Struct(Node* n,char* optTag,Type type){
    if(n->child->next_sib==NULL){
        //DecList -> Dec
        Dec_in_Struct(n->child,optTag,type);
    }else{
        //DecList -> Dec COMMA DecList
        Dec_in_Struct(n->child,optTag,type);
        DecList_in_Struct(n->child->next_sib->next_sib,optTag,type);
    }
}
void Dec_in_Struct(Node* n,char* optTag,Type type){
    if(n->child->next_sib==NULL){
        //Dec -> VarDec
        VarDec_in_Struct(n->child,optTag,type);
    }else{
        //Dec -> VarDec ASSIGNOP Exp
        fprintf(stderr,"Error type 15 at line %d: 结构体\"%s\"在定义时对域进行初始化.\n",n->lineno,optTag);
    }  
}
void VarDec_in_Struct(Node* n,char* optTag,Type type){
    if(n->child->next_sib==NULL){

        //VarDec -> ID
        Type type=structureMap[optTag];
        FieldList fieldList=type->u.structure;
        if(fieldList==NULL){//结构体还没有加入任何的域名
            fieldList=(FieldList)malloc(sizeof(struct FieldList_));
            fieldList->name=n->child->name;
            fieldList->type=type;
            fieldList->tail=NULL;
        }else{
            while(fieldList->tail!=NULL){
                fieldList=fieldList->tail;
            }
            fieldList->tail=(FieldList)malloc(sizeof(struct FieldList_));
            fieldList->tail->name=n->child->name;
            fieldList->tail->type=type;
            fieldList->tail->tail=NULL;
        }
    


    }else{


        //VarDec -> VarDec LB INT RB
        Type newtype=(Type)malloc(sizeof (struct Type_));
        newtype->kind=Type_::ARRAY;
        newtype->u.array.elem=type;
        VarDec_in_Struct(n->child,optTag,newtype);
    }
}
char* OptTag(Node* n){
    if(n->type==SYNTACTIC_UNIT_EMPTY){
        return "";
    }else{
        return n->child->name;
    }
}


void ExtDecList(Node* n,Type type){
    if(n->child->next_sib==NULL){
        //ExtDecList -> VarDec
        VarDec(n->child,type);
    }else{
        //VarDec COMMA ExtDecList
        VarDec(n->child,type);//VarDec是全局或函数内部
        ExtDecList(n->child->next_sib->next_sib,type);
    }
}
//正常的变量声明Variable Declarator    既可以是全局的变量，又可以在函数中使用来声明变量
void VarDec(Node* n,Type type){//VarDec是全局或函数内部
    if(n->child->next_sib==NULL){
        // VarDec -> ID
        Node* id=n->child;
        if(map.find(id->name)!=map.end()){
            //重复定义
            fprintf(stderr,"Error Type 3 at Line %d: 变量\"%s\"出现重复定义或变量与前面定义过的结构体名字重复.\n",n->lineno,id->name);
        }else{
            map.insert({id->name,type});
        }
    }else{
        // VarDec -> VarDec LB INT RB
        Type newtype=(Type)malloc(sizeof (struct Type_));
        newtype->kind=Type_::ARRAY;
        newtype->u.array.elem=type;
        VarDec(n->child,newtype);//VarDec是全局或函数内部
    }
}


void FunDec(Node* n,Type return_type){
    FuncList function=(FuncList)malloc(sizeof(struct FuncList_));
    strcpy(function->name,n->child->name);
    function->type=return_type;
   
    if(n->child->next_sib->next_sib->name=="RP"){
        //inc()的形式，只有三个子节点
        //FunDec -> ID LP RP
        function->next=NULL;//没有参数
    }else{
        function->next=VarList(n->child->next_sib->next_sib);
    }
    
    if(map.find(function->name)!=map.end()){
        //只要函数名重复定义就是错误类型4,参数不同也是错
        fprintf(stderr,"Error type 4 at line %d: 相同的函数名\"%s\"被多次定义.\n",n->lineno,function->name);
    }else{
        Type type=(Type)malloc(sizeof(struct Type_));
        type->kind=Type_::FUNCTION;
        type->u.myfunc=function;
        map.insert({function->name,type});
    }
}
FuncList VarList(Node* n){
    if(n->child->next_sib!=NULL){
        //VarList -> ParamDec COMMA VarList
        FuncList funcList=ParamDec(n->child);
        funcList->next=VarList(n->child->next_sib->next_sib);
        return funcList;
    }else{
        //VarList -> ParamDec
        return ParamDec(n->child);
    }
}
FuncList ParamDec(Node* n){
    //ParamDec -> Specifier VarDec
    Type type=Specifier(n->child);
    return VarDec_in_FuncParams(n->child->next_sib,type);
}
//函数参数部分
FuncList VarDec_in_FuncParams(Node* n,Type type){
    if(n->child->next_sib==NULL){
        // VarDec -> ID
        FuncList funcList=(FuncList)malloc(sizeof(struct FuncList_));
        funcList->name=n->child->name;
        funcList->type=type;
        funcList->next=NULL;
        return funcList;
    }else{
        // VarDec -> VarDec LB INT RB
        Type newType=(Type)malloc(sizeof (struct Type_));
        newType->kind=Type_::ARRAY;
        newType->u.array.elem=type;
        return VarDec_in_FuncParams(n->child,newType);
    }
}


void CompSt(Node *n,Type return_type){
    // CompSt -> LC DefList StmtList RC
    DefList_in_Function(n->child->next_sib);
    StmtList(n->child->next_sib->next_sib,return_type);
}
void DefList_in_Function(Node* n){
    // DefList -> empty
    if(n->type==SYNTACTIC_UNIT_EMPTY) return;
    // DefList -> Def DefList
    Def_in_Function(n->child);
    DefList_in_Function(n->child->next_sib);
}
void Def_in_Function(Node* n){
    // Def -> Specifier DecList SEMI
    Type dec_type=Specifier(n->child);
    DecList_in_Function(n->child->next_sib,dec_type);
}
void DecList_in_Function(Node* n,Type dec_type){
    if(n->child->next_sib==NULL){
        // DecList -> Dec
        Dec_in_Function(n->child,dec_type);
    }else{
        // DecList -> Dec COMMA DecList
        Dec_in_Function(n->child,dec_type);
        DecList_in_Function(n->child->next_sib->next_sib,dec_type);
    }
}
void Dec_in_Function(Node* n,Type dec_type){
    if(n->child->next_sib==NULL){
        // Dec -> VarDec
        VarDec(n->child,dec_type);//VarDec是全局或函数内部
    }else{
        // Dec -> VarDec ASSIGNOP EXP
        VarDec(n->child,dec_type);//VarDec是全局或函数内部
        Exp_ASSIGNOP(n->child->next_sib->next_sib,dec_type);
    }
}


void StmtList(Node* n,Type return_type){
    // StmtList -> empty
    if(n->type==SYNTACTIC_UNIT_EMPTY) return;
    // StmtList -> Stmt StmtList
    Stmt(n->child,return_type);
    StmtList(n->child->next_sib,return_type);
}
void Stmt(Node* n,Type return_type){
    if(n->child->next_sib==NULL){
        // Stmt -> CompSt        //?还需要return_type吗
        CompSt(n->child,return_type);
    }else if(n->child->next_sib->name=="SEMI"){
        // Stmt -> Exp SEMI
        Exp(n->child);
    }else if(n->child->name=="RETURN"){
        // Stmt -> RETURN Exp SEMI
        Type type_in_reality=Exp(n->child->next_sib);
        if(isSameType(return_type,type_in_reality)){
            fprintf(stderr,"Error Type 8 at Line %d: return语句返回类型与函数定义的返回类型不匹配.\n",n->lineno);
        }
    }else if(n->child->name=="WHILE"){
        //      -> WHILE LP Exp RP Stmt
        Type while_condition=Exp(n->child->next_sib->next_sib);
        if(while_condition->kind!=Type_::BASIC || while_condition->u.basic!=IS_INT){
            fprintf(stderr,"Error Type ? at Line %d: 违反假设2：只有INT才能作为while的条件.\n",n->lineno);
        }
        Stmt(n->child->next_sib->next_sib->next_sib->next_sib,return_type);
    }else if(n->child->next_sib->next_sib->next_sib->next_sib->next_sib==NULL){
        Type if_condition=Exp(n->child->next_sib->next_sib);
        if(if_condition->kind!=Type_::BASIC || if_condition->u.basic!=IS_INT){
            fprintf(stderr,"Error Type ? at Line %d: 违反假设2：只有INT才能作为if的条件.\n",n->lineno);
        }
        Stmt(n->child->next_sib->next_sib->next_sib->next_sib,return_type);
        //      -> IF LP Exp RP Stmt
    }else{
        //      -> IF LP Exp RP Stmt ELSE Stmt
        Type if_condition=Exp(n->child->next_sib->next_sib);
        if(if_condition->kind!=Type_::BASIC || if_condition->u.basic!=IS_INT){
            fprintf(stderr,"Error Type ? at Line %d: 违反假设2：只有INT才能作为if的条件.\n",n->lineno);
        }
        Stmt(n->child->next_sib->next_sib->next_sib->next_sib,return_type);
        Stmt(n->child->next_sib->next_sib->next_sib->next_sib->next_sib->next_sib,return_type);
    }
}


Type Exp(Node* n){
    if(n->child->next_sib->name=="ASSIGNOP"){
        //Exp ASSIGNOP Exp
        Type ret_type=Exp(n->child);
        Exp_ASSIGNOP(n->child->next_sib->next_sib,ret_type);
        return ret_type;
    }else if(n->child->name=="ID"){
        //ID
        if(map.find(n->child->name)!=map.end()){
            fprintf(stderr,"Error Type 1 at Line %d: 变量\"%s\"在使用时未经定义.\n",n->lineno,n->child->name);
        }else{
            return map[n->child->name];
        }
    }else if(n->child->name=="Exp"){
        Node * secChild=n->child->next_sib;
        if(secChild->name=="PLUS" || secChild->name=="MINUS"||secChild->name=="STAR"||secChild->name=="DIV"){
            //算数运算Exp PLUS|MINUS|STAR|DIV Exp
            return Exp_Math(n);
        }else if(secChild->name=="AND"||secChild->name=="OR"||secChild->name=="RELOP"){
            //逻辑运算Exp AND|OR|RELOP Exp
            return Exp_Logic(n);
        }
    }
    
    //LP Exp RP
    //Minus Exp
    //NOT Exp
    
    //INT | FLOT
}
void Exp_ASSIGNOP(Node* n,Type left_type){
    Type retType=Exp(n);
    if (isSameType(retType,left_type)){
        ;
    }else{
        fprintf(stderr,"Error Type 5 at Line %d: 赋值号两边的表达式类型不匹配.\n",n->lineno);
    }
}
Type Exp_Math(Node* n){
    //Exp PLUS|MINUS|STAR|DIV Exp
    Type opLeft=Exp(n->child);
    Type opRight=Exp(n->child->next_sib->next_sib);
    if(opLeft->kind!=Type_::BASIC || opRight->kind!=Type_::BASIC){
        fprintf(stderr,"Error Type 7 at Line %d: 操作数类型与操作符不匹配（只有BASIC类型可以算数运算）（例如数组（或结构体）变量与数组（或结构体）变量相加减量）.\n",n->lineno);
        Type errType=(Type)malloc(sizeof(struct Type_));
        errType->kind=Type_::ERROR;
        errType->u.error_type=7;
        return errType;
    }else if (isSameType(opLeft,opRight)){
        return opLeft;
    }else{
        fprintf(stderr,"Error Type 7 at Line %d: 操作数类型不匹配（例如整型变量与数组变量相加减）.\n",n->lineno);
        Type errType=(Type)malloc(sizeof(struct Type_));
        errType->kind=Type_::ERROR;
        errType->u.error_type=7;
        return errType;
    }
}




bool isSameType(Type t1,Type t2){
    //function 
    if(t1->kind!=t2->kind){
        return false;
    }
    //array 
    //structure 
    //basic:int|float
    if(t1->kind==Type_::BASIC){
        return t1->u.basic==t2->u.basic;
    }
    //error
}