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
//typedef struct ArrayList_* ArrayList;
#define IS_INT 0
#define IS_FLOAT 1

//tsl::ordered_map<char*,Type> functionMap;
//tsl::ordered_map<char*,Type> decMap;
unordered_map<char*,Type> map;
//放基本类型变量、数组变量、结构体变量、函数形式参数变量、结构体域名变量
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
//void Exp_ASSIGNOP_in_VarDec(Node* n,Type dec_type);
/*Statements*/
void StmtList(Node* n,Type return_type);
void Stmt(Node* n,Type return_type);
/*Expression*/
Type Exp(Node* n);
Type Exp_ASSIGNOP(Node* n);
Type Exp_Math(Node* n);
Type Exp_Logic(Node* n);
FuncList Args(Node* n);
/*helper*/
bool isSameType(Type t1,Type t2);
Type genErrType(int type);
bool isArrayEqual(Type t1,Type t2);
bool isStructEqual(Type t1,Type t2);


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
    if(strcmp(n->child->next_sib->name,"ExtDecList")==0){
        // Specifier ExtDecList SEMI
        ExtDecList(n->child->next_sib,type);
    }else if(strcmp(n->child->next_sib->name,"SEMI")==0){
        // Specifier SEMI
        ;
    }else if(string(n->child->next_sib->name)=="FunDec"){
        //ExtDef -> Specifier FunDec CompSt
        FunDec(n->child->next_sib,type);
        CompSt(n->child->next_sib->next_sib,type);
    }else{
        cout<<"wrong with ExtDef"<<endl;
    }
}


Type Specifier(Node* n){
    if(strcmp(n->child->name,"TYPE")==0){
        //Specifier -> TYPE
        cout<<"Specifier -> TYPE"<<endl;
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
            fprintf(stderr,"Error Type 16 at Line %d: 结构体名\"%s\"与定义过的结构体或变量重复.\n",n->lineno,n->child->next_sib->str_constant);
            return genErrType(16);

        }else if(map.find(optTag)!=map.end()){
            fprintf(stderr,"Error Type 16 at Line %d: 结构体名\"%s\"与定义过的变量重复.\n",n->lineno,n->child->next_sib->str_constant);
            return genErrType(16);

        }else{
            Type type=(Type)malloc(sizeof(struct Type_));
            type->kind=Type_::STRUCTURE;
            type->u.structure=NULL;
            structureMap.insert({optTag,type});
            DefList_in_Struct(n->child->next_sib->next_sib->next_sib,optTag);
            return structureMap[optTag];
        }


    }else{
        // STRUCT Tag
        char* tagName=n->child->next_sib->child->str_constant;
        if(structureMap.find(tagName)==structureMap.end()){
            //理解为这样的形式 struct Complex c;
            fprintf(stderr,"Error Type 17 at Line %d: 直接使用未定义过的结构体名\"%s\"来定义变量.\n",n->lineno,tagName);
            return genErrType(17);
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


        //1. 将域名加入map中
        if(map.find(n->child->str_constant)!=map.end()){
            fprintf(stderr,"Error Type 15 at Line %d: 结构体\"%s\"中域名重复定义（同一结构体中/不同结构体中）.\n",n->lineno,optTag);
            return;
            //不采用：如果结构体定义错误，则该结构体不加入符号表。
            //采用：如果结构体定义中域名重复，则仍然该结构体名加入符号表，但是重复的定义并不加入该结构体域。
        }else{
            map.insert({n->child->str_constant,type});
        }

        //2. 结构体名已经在前面加入了structureMap
        //   将域名加入structureMap的结构体定义中
        Type type=structureMap[optTag];
        FieldList fieldList=type->u.structure;
        if(fieldList==NULL){//结构体还没有加入任何的域名
            fieldList=(FieldList)malloc(sizeof(struct FieldList_));
            fieldList->name=n->child->str_constant;
            fieldList->type=type;
            fieldList->tail=NULL;
        }else{
            while(fieldList->tail!=NULL){
                fieldList=fieldList->tail;
            }
            fieldList->tail=(FieldList)malloc(sizeof(struct FieldList_));
            fieldList->tail->name=n->child->str_constant;
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
        // char ret[1];
        // ret[1]='\0';
        char* ret;
        *ret='\0';
        return ret;
    }else{
        return n->child->str_constant;
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
//正常的变量声明Variable Declarator    
//VarDec 既可以是全局的变量，又可以在函数中使用来声明变量
void VarDec(Node* n,Type type){
    if(n->child->next_sib==NULL){
        // VarDec -> ID

        Node* id=n->child;
        if(map.find(id->str_constant)!=map.end()){
            fprintf(stderr,"Error Type 3 at Line %d: 变量\"%s\"出现重复定义.\n",n->lineno,id->str_constant);
        }else if(structureMap.find(id->str_constant)!=map.end()){
            fprintf(stderr,"Error Type 3 at Line %d: 变量\"%s\"与前面定义过的结构体名字重复.\n",n->lineno,id->str_constant);
        }else{
            map.insert({id->str_constant,type});
            for(auto x:map)
                cout<<x.first<<" "<<x.second->kind<<endl;
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
    //如果函数定义（函数返回参数）有问题，则不将该函数加入函数表
    cout<<"in FunDec"<<endl;
    if(return_type->kind==Type_::ERROR) return;
    FuncList function=(FuncList)malloc(sizeof(struct FuncList_));
    function->name=n->child->str_constant;
    printf("%s\n",n->child->next_sib->next_sib->name);
    function->type=return_type;
    if(string(n->child->next_sib->next_sib->name)=="RP"){
        //inc()的形式，只有三个子节点
        //FunDec -> ID LP RP
        cout<<"FunDec -> ID LP RP"<<endl;
        function->next=NULL;//没有参数
    }else{
        //FunDec -> ID LP VarList RP
        function->next=VarList(n->child->next_sib->next_sib);
    }
    
    if(functionMap.find(function->name)!=functionMap.end()){
        //只要函数名重复定义就是错误类型4,参数不同也是错
        //直接丢弃这个函数
        fprintf(stderr,"Error type 4 at line %d: 相同的函数名\"%s\"被多次定义.\n",n->lineno,function->name);
    }else{
        Type type=(Type)malloc(sizeof(struct Type_));
        type->kind=Type_::FUNCTION;
        type->u.myfunc=function;
        functionMap.insert({function->name,type});
    }
}
FuncList VarList(Node* n){
    if(n->child->next_sib!=NULL){
        //VarList -> ParamDec COMMA VarList
        FuncList f1=ParamDec(n->child);
        FuncList f2=VarList(n->child->next_sib->next_sib);
        if(f1==NULL) return f2;//如果函数参数出问题，该函数还是加入函数符号表，只是舍弃出错的参数
        f1->next=f2;
        return f1;
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
//VarDec 函数参数
FuncList VarDec_in_FuncParams(Node* n,Type type){
    //如果链接函数链表时发现有函数参数类型错误，则？？直接丢弃这个函数？
    //用例中可能只有重复定义这种错误，不会出现type是没定义过的struct这种情况。暂且返回NULL
    if(type->kind==Type_::ERROR) return NULL;

    if(n->child->next_sib==NULL){
        // VarDec -> ID
        if(map.find(n->child->str_constant)!=map.end()){
            fprintf(stderr,"Error Type 3 at Line %d: 函数参数名与其他变量名相同.\n",n->lineno);
            return NULL;
        }else if(structureMap.find(n->child->str_constant)!=structureMap.end()){
            fprintf(stderr,"Error Type 3 at Line %d: 函数参数与结构体名相同.\n",n->lineno); 
            return NULL;
        }else{
            FuncList funcList=(FuncList)malloc(sizeof(struct FuncList_));
            funcList->name=n->child->str_constant;
            funcList->type=type;
            funcList->next=NULL;
            map.insert({n->child->str_constant,type});
            return funcList;
        }

       
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
        Type right=Exp(n->child->next_sib->next_sib);
        if(!isSameType(dec_type,right)){
            fprintf(stderr,"Error Type 5 at Line %d: int a=1.1或者int a=ErrorType 这种形式.\n",n->lineno);
        }else{
            VarDec(n->child,dec_type);//VarDec是全局或函数内部
        } 
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
    }else if(string(n->child->next_sib->name)=="SEMI"){
        // Stmt -> Exp SEMI
        Exp(n->child);
    }else if(string(n->child->name)=="RETURN"){
        // Stmt -> RETURN Exp SEMI
        Type type_in_reality=Exp(n->child->next_sib);
        if(isSameType(return_type,type_in_reality)){
            fprintf(stderr,"Error Type 8 at Line %d: return语句返回类型与函数定义的返回类型不匹配.\n",n->lineno);
        }
    }else if(string(n->child->name)=="WHILE"){
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
    if(string(n->child->name)=="ID"){
        //ID
        printf("%s\n",n->child->str_constant);
        for(auto x:map)
            cout<<x.first<<" "<<x.second->kind<<endl;
        if(map.find(n->child->str_constant)!=map.end()){
            fprintf(stderr,"Error Type 1 at Line %d: 变量\"%s\"在使用时未经定义.\n",n->lineno,n->child->str_constant);
            return genErrType(1);
        }else{
            char* tmp=n->child->str_constant;
            if(map.at(tmp)==NULL) printf("确实这里产生了NULL.\n");
            return map.at(tmp);
        }
    }
    else if(string(n->child->name)=="INT"){
        //INT
        Type t=(Type)malloc(sizeof(struct Type_));
        t->kind=Type_::BASIC;
        t->u.basic=IS_INT;
        return t;
    }
    else if(string(n->child->name)=="FLOAT"){
        //FLOAT
        Type t=(Type)malloc(sizeof(struct Type_));
        t->kind=Type_::BASIC;
        t->u.basic=IS_FLOAT;
        return t;
    }
    else if(string(n->child->name)=="NOT" || string(n->child->next_sib->name)=="AND" || string(n->child->next_sib->name)=="OR" || string(n->child->next_sib->name)=="RELOP" ){
        //逻辑运算
        //NOT Exp
        //Exp AND|OR|RELOP Exp
        return Exp_Logic(n);
    }
    else if(string(n->child->next_sib->name)=="PLUS"||string(n->child->next_sib->name)=="MINUS"||string(n->child->next_sib->name)=="STAR"||string(n->child->next_sib->name)=="DIV"){
        //算数运算Exp PLUS|MINUS|STAR|DIV Exp
        return Exp_Math(n);
    }
    else if(string(n->child->name)=="LP" || string(n->child->name)=="MINUS"){
        //LP Exp RP
        //Minus Exp
        return Exp(n->child->next_sib);
    }
    else if(string(n->child->next_sib->name)=="ASSIGNOP"){
        //Exp ASSIGNOP Exp
        return Exp_ASSIGNOP(n);
    }
    else if(string(n->child->next_sib->name)=="DOT"){
        //Exp DOT ID
        Type t=Exp(n->child);
        if(t->kind!=Type_::STRUCTURE){
            fprintf(stderr,"Error Type 13 at Line %d: 对非结构体变量使用DOT.\n",n->lineno);
            return genErrType(13);
        }
        FieldList f=t->u.structure;
        //int found=0;
        char* target=n->child->next_sib->next_sib->str_constant;
        while(f!=NULL){
            if(strcmp(f->name,target)==0) break;
            f=f->tail;
        }
        if(f==NULL){
            fprintf(stderr,"Error Type 14 at Line %d: 访问结构体中未定义过的域.\n",n->lineno);
            return genErrType(14); 
        }
        return f->type;
    }
    else if(string(n->child->next_sib->name)=="LB"){
        //Exp LB Exp RB
        Type t=Exp(n->child);
        if(t->kind!=Type_::ARRAY){
            fprintf(stderr,"Error Type 10 at Line %d: 对非数组变量使用[].\n",n->lineno);
            return genErrType(10);
        }
        t=Exp(n->child->next_sib->next_sib);
        if(t->kind!=Type_::BASIC || t->u.basic==IS_INT){
            fprintf(stderr,"Error Type 12 at Line %d: 数组访问操作符[]中出现非整数.\n",n->lineno);
            return genErrType(12);
        }
        return t->u.array.elem;
    }
    else if(string(n->child->next_sib->next_sib->name)=="RP"){
        //ID LP RP
        if(map.find(n->child->str_constant)!=map.end()){
            fprintf(stderr,"Error Type 11 at Line %d: 对普通变量使用().\n",n->lineno);
            return genErrType(2);
        }
        if(functionMap.find(n->child->str_constant)==functionMap.end()){
            fprintf(stderr,"Error Type 2 at Line %d: 函数在调用时未经定义.\n",n->lineno);
            return genErrType(2);
        }
        return functionMap[n->child->str_constant]->u.myfunc->type;
    }
    else{
        //ID LP Args RP
        if(map.find(n->child->str_constant)!=map.end()){
            fprintf(stderr,"Error Type 11 at Line %d: 对普通变量使用().\n",n->lineno);
            return genErrType(2);
        }
        if(functionMap.find(n->child->str_constant)==functionMap.end()){
            fprintf(stderr,"Error Type 2 at Line %d: 函数在调用时未经定义.\n",n->lineno);
            return genErrType(2);
        }
        Type f=functionMap[n->child->str_constant];
        FuncList f1=f->u.myfunc->next;//第一个是返回类型的节点
        FuncList f2=Args(n->child->next_sib->next_sib);
        //bool typeEqual=true;
        while(f1!=NULL && f2!=NULL){
            if(!isSameType(f1->type,f2->type)) break;
            f1=f1->next;
            f2=f2->next;
        }
        if(f1==NULL && f2==NULL) return f->u.myfunc->type;
        fprintf(stderr,"Error Type 9 at Line %d: 函数实参与形参不匹配.\n",n->lineno);
        return genErrType(9);
    }
}
FuncList Args(Node* n){
    if(n->child->next_sib!=NULL){
        //Args -> Exp COMMA Args
        FuncList left=(FuncList)malloc(sizeof(struct FuncList_));
        left->type=Exp(n->child);
        left->next=Args(n->child->next_sib->next_sib);
        return left;
    }else{
        //Args -> Exp
        FuncList funcList=(FuncList)malloc(sizeof(struct FuncList_));
        funcList->type=Exp(n->child);
        funcList->next=NULL;
        return funcList;
    } 
}
Type Exp_ASSIGNOP(Node* n){
    //Exp ASSIGNOP Exp
    printf("前面：等号左边\n");
    Type left_type=Exp(n->child);
    if(left_type==NULL){
        printf("等号左边是空指针\n");
    }else{
        //左边是  右值
        if(left_type->kind==Type_::BASIC||left_type->kind==Type_::FUNCTION||left_type->kind==Type_::ERROR ){
            fprintf(stderr,"Error Type 6 at Line %d: 赋值号左边出现一个只有右值的表达式.\n",n->lineno);
            return genErrType(6);
        }
    }
    

    printf("前面：等号右边\n");
    Type right_type=Exp(n->child->next_sib->next_sib);
    if(right_type==NULL){
        printf("等号右边是空指针\n");
    }


    if (!isSameType(right_type,left_type)){
        fprintf(stderr,"Error Type 5 at Line %d: 赋值号两边的表达式类型不匹配.\n",n->lineno);
        return genErrType(5);
    }

    return left_type;
}
Type Exp_Math(Node* n){
    //Exp PLUS|MINUS|STAR|DIV Exp
    Type opLeft=Exp(n->child);
    Type opRight=Exp(n->child->next_sib->next_sib);
    if(opLeft->kind!=Type_::BASIC || opRight->kind!=Type_::BASIC){
        fprintf(stderr,"Error Type 7 at Line %d: 操作数类型与操作符不匹配（只有BASIC类型可以算数运算）（例如数组（或结构体）变量与数组（或结构体）变量相加减量）.\n",n->lineno);
        return genErrType(7);
    }else if (isSameType(opLeft,opRight)){
        return opLeft;
    }else{
        fprintf(stderr,"Error Type 7 at Line %d: 操作数类型不匹配（例如整型变量与数组变量相加减）.\n",n->lineno);      
        return genErrType(7);
    }
}
Type Exp_Logic(Node* n){
    //逻辑运算
    //NOT Exp
    //Exp AND|OR|RELOP Exp
    if(string(n->child->name)=="NOT"){
        Type t=Exp(n->child->next_sib);
        if(t->kind!=Type_::BASIC || t->u.basic!=IS_INT)
            fprintf(stderr,"Error Type 7 at Line %d: 操作数类型与操作符不匹配，只有int可以做逻辑运算.\n",n->lineno);
        return genErrType(7);
    }else{
        Type t1=Exp(n->child->next_sib);
        Type t2=Exp(n->child->next_sib->next_sib);
        if(t1->kind!=Type_::BASIC || t1->u.basic!=IS_INT ||t2->kind!=Type_::BASIC || t2->u.basic!=IS_INT )
            fprintf(stderr,"Error Type 7 at Line %d: 操作数类型与操作符不匹配，只有int可以做逻辑运算.\n",n->lineno);
        return genErrType(7);
    }
    Type type=(Type)malloc(sizeof(struct Type_));
    type->kind=Type_::BASIC;
    type->u.basic=IS_INT;
    return type;
}


Type genErrType(int type){
    Type err=(Type)malloc(sizeof(struct Type_));
    err->kind=Type_::ERROR;
    err->u.error_type=type;
    return err;
}
bool isArrayEqual(Type t1,Type t2){
    //array
    int dimen1=1;
    Type tmp1=t1->u.array.elem;
    while(tmp1->kind==Type_::ARRAY){
        dimen1++;
        tmp1=tmp1->u.array.elem;
    }
    int dimen2=1;
    Type tmp2=t2->u.array.elem;
    while(tmp2->kind==Type_::ARRAY){
        dimen2++;
        tmp2=tmp2->u.array.elem;
    }
    if(dimen1!=dimen2) return false;
    if(tmp1->kind==Type_::BASIC) return tmp1->u.basic==tmp2->u.basic;
    else return isStructEqual(tmp1,tmp2);//if(tmp1->kind==Type_::STRUCTURE) 
}
bool isStructEqual(Type t1,Type t2){
    FieldList f1=t1->u.structure;
    FieldList f2=t2->u.structure;
    while(f1!=NULL && f2!=NULL){
        if(!isSameType(f1->type,f2->type)) return false;
        f1=f1->tail;
        f2=f2->tail;
    }
    return f1==NULL && f2==NULL;
}
bool isSameType(Type t1,Type t2){
    //function 
    if(t1->kind!=t2->kind){
        return false;
    }
    //array
    if(t1->kind==Type_::ARRAY){
        return isArrayEqual(t1,t2);
    } 
    //structure 
    if(t1->kind==Type_::STRUCTURE){
        return isStructEqual(t1,t2);
    }
    //basic:int|float
    if(t1->kind==Type_::BASIC){
        return t1->u.basic==t2->u.basic;
    }
    fprintf(stderr,"isSameType出错了！");
    return true;
    //error
}