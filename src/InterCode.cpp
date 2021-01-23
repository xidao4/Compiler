#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include "SyntaxNode.h"
#include "InterCode.h"
#include "SemanticAnalysis.h"
using namespace std;

extern FILE* fp;
extern unordered_map<string,Type> map;
extern unordered_map<string,Type> functionMap;
extern InterCode code_head;
extern InterCode code_tail;
extern int Temp_Num;
extern int Label_Num;

Operand new_temp(){
    Temp_Num++;
    Operand ans=(Operand)malloc(sizeof(struct Operand_));
    ans->kind=Operand_::TMP_VAR;
    ans->u.intVal=Temp_Num;
    return ans;
}
Operand new_label(){
    Label_Num++;
    Operand ans=(Operand)malloc(sizeof(struct Operand_));
    ans->kind=Operand_::LABEL;
    ans->u.intVal=Label_Num;
    return ans;
}
Operand create_label(int x){
    Label_Num++;
    Operand ans=(Operand)malloc(sizeof(struct Operand_));
    ans->kind=Operand_::LABEL;
    ans->u.intVal=x;
    return ans;
}
void printOperand(Operand op){
    if(op->kind==Operand_::CONSTANT){
        fprintf(fp,"#%d",op->u.intVal);
    }else if(op->kind==Operand_::TMP_VAR){
        fprintf(fp,"t%d",op->u.intVal);
    }else if(op->kind==Operand_::VARIABLE){
        fprintf(fp,"v_%s",op->u.strVal);
    }else if(op->kind==Operand_::MYSTAR){
        fprintf(fp,"*t%d",op->u.intVal);
    }else if(op->kind==Operand_::VAR_ADDR){
        fprintf(fp,"&v%d",op->u.intVal);
    }else if(op->kind==Operand_::TMP_ADDR){
        fprintf(fp,"&t%d",op->u.intVal);
    }else{
        cout<<"printOperand参数传错了吧"<<endl;
    }
}
void printIR(InterCode head){
    if(head==NULL) cout<<"head=NULL!"<<endl;
    while(head!=NULL){
        if(head->kind==InterCode_::W_LABEL){
            fprintf(fp,"LABEL label%d :\n",head->u.Single.op->u.intVal);
        }
        else if(head->kind==InterCode_::W_FUNCTION){
            fprintf(fp,"FUNCTION %s :\n",head->u.Single.op->u.strVal);
        }
        else if(head->kind==InterCode_::W_ASSIGN){
            printOperand(head->u.Assign.left);
            fprintf(fp," := ");
            printOperand(head->u.Assign.right);
            fprintf(fp,"\n");
        }
        else if(head->kind==InterCode_::W_ADD||head->kind==InterCode_::W_SUB||head->kind==InterCode_::W_MUL||head->kind==InterCode_::W_DIV){
            printOperand(head->u.Double.result);
            fprintf(fp," := ");
            printOperand(head->u.Double.op1);
            if(head->kind==InterCode_::W_ADD)
                fprintf(fp," + ");
            else if(head->kind==InterCode_::W_SUB)
                fprintf(fp," - ");
            else if(head->kind==InterCode_::W_MUL)
                fprintf(fp," * ");
            else if(head->kind==InterCode_::W_DIV)
                fprintf(fp," / ");
            printOperand(head->u.Double.op2);
            fprintf(fp,"\n");
        }
        else if(head->kind==InterCode_::W_GOTO){
            fprintf(fp,"GOTO label%d\n",head->u.Single.op->u.intVal);
        }
        else if(head->kind==InterCode_::W_IFGOTO){
            fprintf(fp,"IF ");
            printOperand(head->u.Three.x);
            fprintf(fp," %s ",head->u.Three.relop);
            printOperand(head->u.Three.y);
            fprintf(fp," GOTO ");
            printOperand(head->u.Three.label);
            fprintf(fp,"\n");
        }
        else if(head->kind==InterCode_::W_RETURN){
            fprintf(fp,"RETURN ");
            printOperand(head->u.Single.op);
            fprintf(fp,"\n");
        }
        else if(head->kind==InterCode_::W_DEC){
            fprintf(fp,"DEC ");
            //printOperand(head->u.Dec.op);
            fprintf(fp,"v_%s",head->u.Dec.op->u.strVal);
            fprintf(fp," %d\n",head->u.Dec.size);
        }
        else if(head->kind==InterCode_::W_ARG){
            fprintf(fp,"ARG ");
            printOperand(head->u.Single.op);
            fprintf(fp,"\n");
        }
        else if(head->kind==InterCode_::W_CALL){
            printOperand(head->u.Assign.left);
            fprintf(fp," := CALL ");
            //printOperand(head->u.Assign.right);
            fprintf(fp,"%s",head->u.Assign.right->u.strVal);
            fprintf(fp,"\n");
        }
        else if(head->kind==InterCode_::W_PARAM){
            fprintf(fp,"PARAM ");
            //printOperand(head->u.Single.op);
            fprintf(fp,"%s",head->u.Single.op->u.strVal);
            fprintf(fp,"\n");
        }
        else if(head->kind==InterCode_::W_READ){
            fprintf(fp,"READ ");
            //printOperand(head->u.Single.op);
            fprintf(fp,"t%d",head->u.Single.op->u.intVal);
            fprintf(fp,"\n");
        }
        else if(head->kind==InterCode_::W_WRITE){
            fprintf(fp,"WRITE ");
            //printOperand(head->u.Single.op);
            fprintf(fp,"t%d",head->u.Single.op->u.intVal);
            fprintf(fp,"\n");
        }
        head=head->next;
    }
}
int calculateSize(Type type){
    return 4*type->u.array.size;
}
void interInsert(InterCode a){
    cout<<a->kind<<endl;
    if(code_head==NULL){
        code_head=a;
        code_tail=a;
        code_head->next=NULL;
        code_tail->prev=NULL;
    }else{
        code_tail->next=a;
        a->next=NULL;
        a->prev=code_tail;
        code_tail=code_tail->next;
    }
}


void Trans_Program(Node* n){
    cout<<"Program"<<endl;
    Trans_ExtDefList(n->child);
}
void Trans_ExtDefList(Node* n){
    if(n->type==SYNTACTIC_UNIT_EMPTY) return;
    Trans_ExtDef(n->child);
    Trans_ExtDefList(n->child->next_sib);
}
void Trans_ExtDef(Node* n){
    //Specifier ExtDecList SEMI ？？不考虑全局变量
    //Specifier SEMI            ？？
    if(strcmp(n->child->next_sib->name,"FunDec")==0){
        //Specifier FunDec CompSt
        cout<<endl;
        cout<<endl;
        cout<<"ExtDef->Specifier FunDec CompSt"<<endl;

        Trans_FunDec(n->child->next_sib);
        Trans_CompSt(n->child->next_sib->next_sib);
    }
}

void Trans_FunDec(Node* n){
    cout<<"FunDec"<<endl;
    // ID LP RP
    InterCode code=(InterCode)malloc(sizeof(struct InterCode_));
    code->kind=InterCode_::W_FUNCTION;
    Operand op=(Operand)malloc(sizeof(struct Operand_));
    //op->kind=Operand_::VARIABLE;
    string tar(n->child->name);
    op->u.strVal=tar;
    code->u.Single.op=op;
    interInsert(code);
    //ID LP VarList RP
    if(n->child->next_sib->next_sib->next_sib!=NULL){
        Trans_VarList(n->child->next_sib->next_sib);
    }
}
void Trans_VarList(Node* n){
    //ParamDec
    Trans_ParamDec(n->child);
    //ParamDec COMMA VarList
    if(n->child->next_sib!=NULL)
        Trans_VarList(n->next_sib->next_sib);
}
void Trans_ParamDec(Node* n){
    //Specifier VarDec
    Trans_VarDec_in_FuncParam(n->child->next_sib);
}
void Trans_VarDec_in_FuncParam(Node* n){
    if(strcmp(n->child->name,"ID")==0){
        //VarDec->ID
        InterCode code=(InterCode)malloc(sizeof(struct InterCode_));
        code->kind=InterCode_::W_PARAM;
        Operand op=(Operand)malloc(sizeof(struct Operand_));
        //op->kind=Operand_::VARIABLE;
        string tar(n->child->name);
        op->u.strVal=tar;
        code->u.Single.op=op;
        interInsert(code);
    }else{
        cout<<"函数参数不能是数组哦"<<endl;
    }
}


void Trans_CompSt(Node* n){
    // LC DefList StmtList RC
    cout<<"CompSt"<<endl;
    Trans_DefList_in_Function(n->child->next_sib);
    
    Trans_StmtList(n->child->next_sib->next_sib);
}
void Trans_DefList_in_Function(Node* n){
    // DefList -> empty
    if(n->type==SYNTACTIC_UNIT_EMPTY) return;
    // DefList -> Def DefList
    Trans_Def_in_Function(n->child);
    Trans_DefList_in_Function(n->child->next_sib);
}
void Trans_Def_in_Function(Node* n){
    //Specifier DecList SEMI
    cout<<"Def_in_Function"<<endl;
    Trans_DecList_in_Function(n->child->next_sib);
}
void Trans_DecList_in_Function(Node * n){
    if(n->child->next_sib!=NULL){
        //DecList -> Dec
        Trans_Dec_in_Function(n->child);
    }else{
        //DecList -> Dec COMMA DecList
        Trans_Dec_in_Function(n->child);
        Trans_DecList_in_Function(n->child->next_sib->next_sib);
    }
}
void Trans_Dec_in_Function(Node* n){
    cout<<"Dec_in_Function"<<endl;
    if(n->child->next_sib==NULL){
        //Dec -> VarDec
        Trans_VarDec_in_Function(n->child);
    }else{
        //Dec -> VarDec ASSIGNOP Exp
        Operand t1=new_temp();
        Trans_Exp(n->child->next_sib->next_sib,t1);

        InterCode ans=(InterCode)malloc(sizeof(struct InterCode_));
        ans->kind=InterCode_::W_ASSIGN;
        Operand left=(Operand)malloc(sizeof(struct Operand_));
        left->kind=Operand_::VARIABLE;
        string tar(n->child->child->name);//Dec->VarDec->ID
        left->u.strVal=tar;
        ans->u.Assign.left=left;

        ans->u.Assign.right=t1;
        interInsert(ans);
    } 
}
void Trans_VarDec_in_Function(Node* n){
    cout<<"VarDec in function or global"<<endl;
    //VarDec->ID
    //VarDec->ID LB INT RB
    string tar(n->child->str_constant);//node->name=ID  node->str_constant=n;
    //cout<<tar<<endl;
    Type type=map.at(tar);
    //cout<<type->kind<<endl;
    if(type->kind==Type_::ARRAY){
        cout<<"VarDec->ID LB INT RB    arr[4]"<<endl;
        if(type->u.array.elem->kind!=Type_::BASIC){
            cout<<"数组元素不是基本类型"<<endl;
        }else{
            int size=calculateSize(type);
            //size=4*INT即可
            InterCode code=(InterCode)malloc(sizeof(struct InterCode_));
            code->kind=InterCode_::W_DEC;
            Operand op=(Operand)malloc(sizeof(struct Operand_));
            //op->kind=VARIABLE;
            string tar(n->child->name);
            op->u.strVal=tar;
            code->u.Dec.op=op;
            code->u.Dec.size=size;
            interInsert(code);
        }
        
    }       
}


void Trans_StmtList(Node* n){
    // StmtList -> empty
    if(n->type==SYNTACTIC_UNIT_EMPTY) return;
    // StmtList -> Stmt StmtList
    Trans_Stmt(n->child);
    Trans_StmtList(n->child->next_sib);
}
void Trans_Stmt(Node* n){
    cout<<"Stmt"<<endl;
    if(n->child->next_sib==NULL){
        // Stmt -> CompSt        
        Trans_CompSt(n->child);
    }
    else if(strcmp(n->child->next_sib->name,"SEMI")==0){
        // Stmt -> Exp SEMI
        Operand place=(Operand)malloc(sizeof(struct Operand_));
        place->kind=Operand_::NONE;
        Trans_Exp(n->child,place);
    }
    else if(strcmp(n->child->name,"RETURN")==0){
        // Stmt -> RETURN Exp SEMI
        //cout<<"Stmt_RETURN"<<endl;
        Operand t1=new_temp();
        //code1
        Trans_Exp(n->child->next_sib,t1);
        //code2
        InterCode ret=(InterCode)malloc(sizeof(struct InterCode_));
        ret->kind=InterCode_::W_RETURN;
        ret->u.Single.op=t1;
        interInsert(ret);
    }
    else if(strcmp(n->child->name,"WHILE")==0){
        //      -> WHILE LP Exp RP Stmt
        //cout<<"Stmt_WHILE"<<endl;
        Operand label1=new_label();
        Operand label2=new_label();
        Operand label3=new_label();
        //LABEL label1
        InterCode ans1=(InterCode)malloc(sizeof(struct InterCode_));
        ans1->kind=InterCode_::W_LABEL;
        ans1->u.Single.op=label1;
        interInsert(ans1);
        //code1
        int x2=label2->u.intVal;//标签的序号
        int x3=label3->u.intVal;
        Trans_Cond(n->child->next_sib->next_sib,x2,x3);
        //LABEL label2
        InterCode ans2=(InterCode)malloc(sizeof(struct InterCode_));
        ans2->kind=InterCode_::W_LABEL;
        ans2->u.Single.op=label2;
        interInsert(ans2);
        //code2
        Trans_Stmt(n->child->next_sib->next_sib->next_sib->next_sib);
        //GOTO label1
        InterCode ans4=(InterCode)malloc(sizeof(struct InterCode_));
        ans4->kind=InterCode_::W_GOTO;
        ans4->u.Single.op=label1;
        interInsert(ans4);
        //LABEL label3
        InterCode ans3=(InterCode)malloc(sizeof(struct InterCode_));
        ans3->kind=InterCode_::W_LABEL;
        ans3->u.Single.op=label3;
        interInsert(ans3);
    }
    else if(n->child->next_sib->next_sib->next_sib->next_sib->next_sib==NULL){
        //      -> IF LP Exp RP Stmt
        //cout<<"Stmt_IF"<<endl;
        Operand label1=new_label();
        Operand label2=new_label();
        int x1=label1->u.intVal;
        int x2=label2->u.intVal;
        //code1
        Trans_Cond(n->child->next_sib->next_sib,x1,x2);
        //LABEL label1
        InterCode ans1=(InterCode)malloc(sizeof(struct InterCode_));
        ans1->kind=InterCode_::W_LABEL;
        ans1->u.Single.op=label1;
        interInsert(ans1);
        //code2
        Trans_Stmt(n->child->next_sib->next_sib->next_sib->next_sib);
        //LABEL label2
        InterCode ans2=(InterCode)malloc(sizeof(struct InterCode_));
        ans2->kind=InterCode_::W_LABEL;
        ans2->u.Single.op=label2;
        interInsert(ans2);
    }
    else{
        //      -> IF LP Exp RP Stmt ELSE Stmt
        cout<<"Stmt_IF_ELSE"<<endl;

        Operand label1=new_label();
        Operand label2=new_label();
        Operand label3=new_label();
        int x1=label1->u.intVal;
        int x2=label2->u.intVal;
        int x3=label3->u.intVal;
        //code1
        Trans_Cond(n->child->next_sib->next_sib,x1,x2);
        //LABEL label1
        InterCode ans1=(InterCode)malloc(sizeof(struct InterCode_));
        ans1->kind=InterCode_::W_LABEL;
        ans1->u.Single.op=label1;
        interInsert(ans1);
        //code2
        Trans_Stmt(n->child->next_sib->next_sib->next_sib->next_sib);
        //GOTO label3
        InterCode ans4=(InterCode)malloc(sizeof(struct InterCode_));
        ans4->kind=InterCode_::W_GOTO;
        ans4->u.Single.op=label3;
        interInsert(ans4);
        //LABEL label2
        InterCode ans2=(InterCode)malloc(sizeof(struct InterCode_));
        ans2->kind=InterCode_::W_LABEL;
        ans2->u.Single.op=label2;
        interInsert(ans2);
        //code3
        Trans_Stmt(n->child->next_sib->next_sib->next_sib->next_sib->next_sib->next_sib);
        //LABEL label3
        InterCode ans3=(InterCode)malloc(sizeof(struct InterCode_));
        ans3->kind=InterCode_::W_LABEL;
        ans3->u.Single.op=label3;
        interInsert(ans3);
    }
}
void Trans_Cond(Node* n,int label_true,int label_false){
    if(strcmp(n->child->next_sib->name,"RELOP")==0){
        // Exp1 RELOP Exp2
        Operand t1=new_temp();
        Operand t2=new_temp();
        //code1
        Trans_Exp(n->child,t1);
        //code2
        Trans_Exp(n->child->next_sib->next_sib,t2);
        //code3
        InterCode code3=(InterCode)malloc(sizeof(struct InterCode_));//InterCode code3=new struct InterCode_;
        code3->kind=InterCode_::W_IFGOTO;
        code3->u.Three.x=t1;
        code3->u.Three.y=t2;
        string tar(n->child->next_sib->name);
        code3->u.Three.relop=tar;
        code3->u.Three.label=create_label(label_true);
        interInsert(code3);
        //GOTO label_false
        InterCode code4=(InterCode)malloc(sizeof(struct InterCode_));//InterCode code4=new struct InterCode_;
        code4->kind=InterCode_::W_GOTO;
        code4->u.Single.op=create_label(label_false);
        interInsert(code4);
    }
    else if(strcmp(n->child->name,"NOT")==0){
        Trans_Cond(n->child->next_sib,label_false,label_true);
    }
    else if(strcmp(n->child->next_sib->name,"AND")==0){
        Operand label1=new_label();
        int x1=label1->u.intVal;
        //code1
        Trans_Cond(n->child,x1,label_false);
        //LABEL label1
        InterCode ans1=(InterCode)malloc(sizeof(struct InterCode_));
        ans1->kind=InterCode_::W_LABEL;
        ans1->u.Single.op=label1;
        interInsert(ans1);
        //code2
        Trans_Cond(n->child->next_sib->next_sib,label_true,label_false);
    }
    else if(strcmp(n->child->next_sib->name,"OR")==0){
        Operand label1=new_label();
        int x1=label1->u.intVal;
        //code1
        Trans_Cond(n->child,label_true,x1);
        //LABEL label1
        InterCode ans1=(InterCode)malloc(sizeof(struct InterCode_));
        ans1->kind=InterCode_::W_LABEL;
        ans1->u.Single.op=label1;
        interInsert(ans1);
        //code2
        Trans_Cond(n->child->next_sib->next_sib,label_true,label_false);
    }
    else{
        Operand t1=new_temp();
        //code1
        Trans_Exp(n->child,t1);
        //code2  IF t1!=#0 GOTO label_true
        InterCode code3=(InterCode)malloc(sizeof(struct InterCode_));
        code3->kind=InterCode_::W_IFGOTO;
        code3->u.Three.x=t1;

        Operand t2=(Operand)malloc(sizeof(struct Operand_));
        t2->kind=Operand_::CONSTANT;
        t2->u.intVal=0;
        code3->u.Three.y=t2;
 
        code3->u.Three.relop="!=";
        code3->u.Three.label=create_label(label_true);
        interInsert(code3);
        //GOTO label_false
        InterCode code4=(InterCode)malloc(sizeof(struct InterCode_));
        code4->kind=InterCode_::W_GOTO;
        code4->u.Single.op=create_label(label_false);
        interInsert(code4);
    }
}

void Trans_Exp(Node* n, Operand place){
    if(n->child->next_sib==NULL && strcmp(n->child->name,"ID")==0){   //#include <cstring>
        
        place->kind=Operand_::VARIABLE;
        string tar(n->child->str_constant);
        place->u.strVal=tar;
    }
    else if(strcmp(n->child->name,"INT")==0){
        //place->kind=Operand_::CONSTANT;
        //place->u.intVal=n->int_constant;
        Operand op=(Operand)malloc(sizeof(struct Operand_));
        op->kind=Operand_::CONSTANT;
        op->u.intVal=n->int_constant;

        InterCode code=(InterCode)malloc(sizeof(struct InterCode_));
        code->kind=InterCode_::W_ASSIGN;
        code->u.Assign.left=place;
        code->u.Assign.right=op;
        interInsert(code);
    }
    else if(strcmp(n->child->name,"NOT")==0 || strcmp(n->child->next_sib->name,"AND")==0 || strcmp(n->child->next_sib->name,"OR")==0 || strcmp(n->child->next_sib->name,"RELOP")==0 ){
        //逻辑运算
        //NOT Exp
        //Exp AND|OR|RELOP Exp
        //Trans_Exp_Logic(n,place);
    }
    else if(strcmp(n->child->next_sib->name,"PLUS")==0||strcmp(n->child->next_sib->name,"MINUS")==0||strcmp(n->child->next_sib->name,"STAR")==0||strcmp(n->child->next_sib->name,"DIV")==0){
        //算数运算Exp PLUS|MINUS|STAR|DIV Exp
        //Trans_Exp_MATH(n,place);
    }
    else if(strcmp(n->child->name,"LP")==0 || strcmp(n->child->name,"MINUS")==0){
        //LP Exp RP
        //Minus Exp
        Trans_Exp(n->child->next_sib,place);
    }else if(strcmp(n->child->next_sib->name,"ASSIGNOP")==0){
        Trans_Exp_ASSIGNOP(n,place);
    }
    else if(strcmp(n->child->next_sib->name,"DOT")==0){
        ;
    }else if(strcmp(n->child->next_sib->name,"LB")==0){
        //Exp LB Exp RB
        //Trans_Exp_Array(n,place);
    }
    else if(strcmp(n->child->next_sib->next_sib->name,"RP")==0){
        //ID LP RP
        Trans_Exp_Func(n,place);
    }
    else{
        //ID LP Args RP
        Trans_Exp_FuncParams(n,place);
    } 
}
void Trans_Exp_Func(Node* n,Operand place){
    //ID LP RP

    string tar(n->child->name);
    Type func=functionMap.at(tar);
    if(func->u.myfunc->name=="read"){
        InterCode code=(InterCode)malloc(sizeof(struct InterCode_));
        code->kind=InterCode_::W_READ;
        code->u.Single.op->u.intVal=place->u.intVal;
        interInsert(code);
    }else{
        Operand op=(Operand)malloc(sizeof(struct Operand_));
        //op->kind=Operand_::VARIABLE;
        op->u.strVal=func->u.myfunc->name;

        InterCode code=(InterCode)malloc(sizeof(struct InterCode_));
        code->kind=InterCode_::W_CALL;
        code->u.Assign.left=place;
        code->u.Assign.right=op;
        interInsert(code);
    }
}
void Trans_Args(Node* n,ArgList arg_list){
    if(n->child->next_sib==NULL){
        //Args->Exp
        //code1
        Operand t1=new_temp();
        Trans_Exp(n->child,t1);

        //arg_list=t1+arg_list
        ArgList arg_t1=(ArgList)malloc(sizeof(struct ArgList_));
        arg_t1->arg=t1;
        arg_t1->next=arg_list;
        arg_list=arg_t1;
    }else{
        //Exp COMMA Args
        //code1
        Operand t1=new_temp();
        Trans_Exp(n->child,t1);
        //arg_list=t1+arg_list
        ArgList arg_t1=(ArgList)malloc(sizeof(struct ArgList_));
        arg_t1->arg=t1;
        arg_t1->next=arg_list;
        arg_list=arg_t1;
        //code2
        Trans_Args(n->child->next_sib->next_sib,arg_list);
    }
}
void Trans_Exp_FuncParams(Node* n,Operand place){
    //code1
    ArgList arg_list=NULL;
    Trans_Args(n->child->next_sib->next_sib,arg_list);
    //write
    string tar(n->child->name);
    Type func=functionMap.at(tar);
    if(func->u.myfunc->name=="write"){
        InterCode code=(InterCode)malloc(sizeof(struct InterCode_));
        code->kind=InterCode_::W_WRITE;
        //code->u.Single.op->u.intVal=arg_list->arg->u.intVal;
        code->u.Single.op=arg_list->arg;
        interInsert(code);
        return;
    }
    //code2
    while(arg_list!=NULL){
        InterCode code=(InterCode)malloc(sizeof(struct InterCode_));
        code->kind=InterCode_::W_ARG;
        code->u.Single.op=arg_list->arg;
        interInsert(code);
        arg_list=arg_list->next;
    }
    //[place:=CALL 函数名]
    Operand funcName=(Operand)malloc(sizeof(struct Operand_));
    funcName->u.strVal=tar;

    InterCode code=(InterCode)malloc(sizeof(struct InterCode_));
    code->kind=InterCode_::W_CALL;
    code->u.Assign.left=place;
    code->u.Assign.right=funcName;
    interInsert(code);
}

void Trans_Exp_ASSIGNOP(Node* n,Operand place){
    //exp -> exp1 assignop exp
    //exp1 -> id | id LB exp RB
    string id(n->child->child->name);
    Type type=map.at(id);
    if(type->kind==Type_::BASIC){
        //等号左边是ID
        
        //code1
        Operand t1=new_temp();
        Trans_Exp(n->child->next_sib->next_sib,t1);
        //code2.1
        Operand op=(Operand)malloc(sizeof(struct Operand_));
        op->kind=Operand_::VARIABLE;
        op->u.strVal=id;

        InterCode code21=(InterCode)malloc(sizeof(struct InterCode_));
        code21->kind=InterCode_::W_ASSIGN;
        code21->u.Assign.right=t1;
        code21->u.Assign.left=op;
        interInsert(code21);
        //code2.2
        if(place->kind!=Operand_::NONE){
            Operand op2=(Operand)malloc(sizeof(struct Operand_));
            op2->kind=Operand_::VARIABLE;
            op2->u.strVal=id;

            InterCode code22=(InterCode)malloc(sizeof(struct InterCode_));
            code22->kind=InterCode_::W_ASSIGN;
            code22->u.Assign.left=place;
            code22->u.Assign.right=op2;     
            interInsert(code22);
        }
    }else{
        //等号左边是一维数组类型
    }
}
