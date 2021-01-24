#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include "SyntaxNode.h"
#include "InterCode.h"
#include "SemanticAnalysis.h"
using namespace std;

//extern FILE* fp;
//extern ofstream outfile;
extern string outFileName;
extern unordered_map<string,Type> map;
extern unordered_map<string,Type> functionMap;
extern InterCode code_head;
extern InterCode code_tail;
extern int Temp_Num;
extern int Label_Num;

Operand new_operand(){
    Operand op=(Operand)malloc(sizeof(struct Operand_));
    return op;
}
Operand new_temp(){
    Temp_Num++;
    Operand ans=(Operand)malloc(sizeof(struct Operand_));
    ans->kind=Operand_::TMP_VAR;
    string tmp="t"+to_string(Temp_Num);
    ans->u.strVal=tmp;//bug
    //ans->u.intVal=Temp_Num;
    cout<<"Temp_Num:"<<Temp_Num<<endl;
    return ans;
}
Operand new_label(){
    Label_Num++;
    Operand ans=(Operand)malloc(sizeof(struct Operand_));
    ans->kind=Operand_::LABEL;
    ans->u.intVal=Label_Num;
    cout<<"Label_Num:"<<Label_Num<<endl;
    return ans;
}
Operand create_label(int x){
    //Label_Num++;
    Operand ans=(Operand)malloc(sizeof(struct Operand_));
    ans->kind=Operand_::LABEL;
    ans->u.intVal=x;
    return ans;
}
string printOperand(Operand op){
    if(op->kind==Operand_::CONSTANT){
        //return "#"+to_string(op->u.intVal);
        return op->u.strVal;
    }
    else if(op->kind==Operand_::TMP_VAR){
        return op->u.strVal;
    }
    else if(op->kind==Operand_::VARIABLE){
        return op->u.strVal;
    }
    else if(op->kind==Operand_::LABEL){
        return "label"+to_string(op->u.intVal);
    }
    else{
        return "Operand的类型是NONE";
    }
}
void printIR(InterCode head){
    if(head==NULL) cout<<"head=NULL!"<<endl;
    ofstream outfile;
	outfile.open(outFileName);
    while(head!=NULL){
        if(head->kind==InterCode_::W_LABEL){
            outfile<<"LABEL label"<<head->u.Single.op->u.intVal<<" :"<<endl;
        }
        else if(head->kind==InterCode_::W_FUNCTION){
            outfile<<"FUNCTION "<<head->u.Single.op->u.strVal<<" :"<<endl;
        }
        else if(head->kind==InterCode_::W_ASSIGN){
            outfile<<printOperand(head->u.Assign.left);
            outfile<<" := ";
            outfile<<printOperand(head->u.Assign.right);
            outfile<<endl;
        }
        else if(head->kind==InterCode_::W_ADD||head->kind==InterCode_::W_SUB||head->kind==InterCode_::W_MUL||head->kind==InterCode_::W_DIV){
            outfile<<printOperand(head->u.Double.result);
            outfile<<" := ";
            outfile<<printOperand(head->u.Double.op1);
            if(head->kind==InterCode_::W_ADD)
                outfile<<" + ";
            else if(head->kind==InterCode_::W_SUB)
                outfile<<" - ";
            else if(head->kind==InterCode_::W_MUL)
                outfile<<" * ";
            else if(head->kind==InterCode_::W_DIV)
                outfile<<" / ";
            outfile<<printOperand(head->u.Double.op2);
            outfile<<endl;
        }
        else if(head->kind==InterCode_::W_GET_ADDR){
            //[x := &y + z]
            outfile<<printOperand(head->u.Double.result)<<" := &"<<printOperand(head->u.Double.op1)<<" + "<<printOperand(head->u.Double.op2)<<endl;
        }
        else if(head->kind==InterCode_::W_GET_VAL){
            //[x := *y]
            outfile<<printOperand(head->u.Assign.left)<<" := *"<<printOperand(head->u.Assign.right)<<endl;
        }
        else if(head->kind==InterCode_::W_VAL_GOT){
            //[*x := y]
            outfile<<"*"<<printOperand(head->u.Assign.left)<<" := "<<printOperand(head->u.Assign.right)<<endl;
        }
        else if(head->kind==InterCode_::W_GOTO){
            outfile<<"GOTO label"<<head->u.Single.op->u.intVal<<endl;
            //fprintf(fp,"GOTO label%d\n",head->u.Single.op->u.intVal);
        }
        else if(head->kind==InterCode_::W_IFGOTO){
            //fprintf(fp,"IF ");
            outfile<<"IF ";
            outfile<<printOperand(head->u.Three.x);
            outfile<<" "<<head->u.Three.relop<<" ";
            //fprintf(fp," %s ",head->u.Three.relop);
            outfile<<printOperand(head->u.Three.y);
            outfile<<" GOTO ";
            //fprintf(fp," GOTO ");
            outfile<<printOperand(head->u.Three.label);
            //fprintf(fp,"\n");
            outfile<<endl;
        }
        else if(head->kind==InterCode_::W_RETURN){
            outfile<<"RETURN ";
            outfile<<printOperand(head->u.Single.op);
            outfile<<endl;
        }
        else if(head->kind==InterCode_::W_DEC){
            //fprintf(fp,"DEC ");
            outfile<<"DEC ";
            //printOperand(head->u.Dec.op);
            //fprintf(fp,"v_%s",head->u.Dec.op->u.strVal);
            outfile<<head->u.Dec.op->u.strVal;
            //fprintf(fp," %d\n",head->u.Dec.size);
            outfile<<" "<<head->u.Dec.size<<endl;
        }
        else if(head->kind==InterCode_::W_ARG){
            outfile<<"ARG ";
            //fprintf(fp,"ARG ");
            outfile<<printOperand(head->u.Single.op);
            outfile<<endl;
            //fprintf(fp,"\n");
        }
        else if(head->kind==InterCode_::W_CALL){
            outfile<<printOperand(head->u.Assign.left);
            outfile<<" := CALL ";
            //fprintf(fp," := CALL ");
            //printOperand(head->u.Assign.right);
            //fprintf(fp,"%s",head->u.Assign.right->u.strVal);
            outfile<<head->u.Assign.right->u.strVal<<endl;
            //fprintf(fp,"\n");
        }
        else if(head->kind==InterCode_::W_PARAM){
            //fprintf(fp,"PARAM ");
            outfile<<"PARAM ";
            //printOperand(head->u.Single.op);
            //fprintf(fp,"%s",head->u.Single.op->u.strVal);
            outfile<<head->u.Single.op->u.strVal<<endl;
            //fprintf(fp,"\n");
        }
        else if(head->kind==InterCode_::W_READ){
            outfile<<"READ ";
            //printOperand(head->u.Single.op);
            //fprintf(fp,"t%d",head->u.Single.op->u.intVal);
            outfile<<head->u.Single.op->u.strVal;
            outfile<<endl;
        }
        else if(head->kind==InterCode_::W_WRITE){
            outfile<<"WRITE ";
            //printOperand(head->u.Single.op);
            //fprintf(fp,"t%d",head->u.Single.op->u.intVal);
            outfile<<head->u.Single.op->u.strVal;
            outfile<<endl;
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
    string tar(n->child->str_constant);
    op->u.strVal=tar;
    code->u.Single.op=op;
    interInsert(code);

    //ID LP VarList RP
    if(n->child->next_sib->next_sib->next_sib!=NULL){
        Trans_VarList(n->child->next_sib->next_sib);
    }
}
void Trans_VarList(Node* n){
    if(n->child->next_sib==NULL){
        //ParamDec
        Trans_ParamDec(n->child);
    }else{
        //ParamDec COMMA VarList
        Trans_ParamDec(n->child);
        Trans_VarList(n->child->next_sib->next_sib);
    }
}
void Trans_ParamDec(Node* n){
    //Specifier VarDec
    Trans_VarDec_in_FuncParam(n->child->next_sib);
}
void Trans_VarDec_in_FuncParam(Node* n){
    if(strcmp(n->child->name,"ID")==0){
        //VarDec->ID
        //假设：函数参数不能是数组

        Operand op=(Operand)malloc(sizeof(struct Operand_));
        //op->kind=Operand_::VARIABLE;
        string tar(n->child->str_constant);
        op->u.strVal=tar;

        InterCode code=(InterCode)malloc(sizeof(struct InterCode_));
        code->kind=InterCode_::W_PARAM;
        code->u.Single.op=op;
        interInsert(code);
    }else{
        //VarDec->VarDec LB INT RB
        cout<<"VarDec_in_FuncParams：函数参数不可以是数组哦"<<endl;
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
    if(n->child->next_sib==NULL){
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
        //not debug
        Operand t1=new_temp();
        Trans_Exp(n->child->next_sib->next_sib,t1);

        Operand left=(Operand)malloc(sizeof(struct Operand_));
        left->kind=Operand_::VARIABLE;
        string tar(n->child->child->str_constant);//Dec->VarDec->ID
        left->u.strVal=tar;

        InterCode ans=(InterCode)malloc(sizeof(struct InterCode_));
        ans->kind=InterCode_::W_ASSIGN;
        ans->u.Assign.left=left;
        ans->u.Assign.right=t1;
        interInsert(ans);
    } 
}
void Trans_VarDec_in_Function(Node* n){
    cout<<"VarDec in function or global"<<endl;
    //VarDec->ID
    //VarDec->VarDec LB INT RB
    if(strcmp(n->child->name,"ID")==0) return;


    string tar(n->child->child->str_constant);//node->name=ID  node->str_constant=n;
    Type type=map.at(tar);

    //类型为ARR才需要输出一些内容
    if(type->kind==Type_::ARRAY){
        //not debug
        cout<<"VarDec->VarDec LB INT RB    arr[4]"<<endl;

        if(type->u.array.elem->kind!=Type_::BASIC){
            cout<<"数组元素不是基本类型"<<endl;

        }else{
            //int size=calculateSize(type);
            int size=4*n->child->next_sib->next_sib->int_constant;
            //size=4*INT即可

            Operand op=(Operand)malloc(sizeof(struct Operand_));
            //op->kind=VARIABLE;
            op->u.strVal=tar;

            InterCode code=(InterCode)malloc(sizeof(struct InterCode_));
            code->kind=InterCode_::W_DEC;
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


    // Stmt -> CompSt
    if(n->child->next_sib==NULL){
        // Stmt -> CompSt        
        Trans_CompSt(n->child);
    }
    else if(strcmp(n->child->next_sib->name,"SEMI")==0){
        // Stmt -> Exp SEMI
        cout<<"Stmt"<<endl;
        Operand place=new_operand();
        place->kind=Operand_::NONE;
        Trans_Exp(n->child,place);
        Trans_Exp(n->child,place);
    }
    else if(strcmp(n->child->name,"RETURN")==0){
        // Stmt -> RETURN Exp SEMI

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
    if(n->child->next_sib!=NULL && strcmp(n->child->next_sib->name,"RELOP")==0){
        // Exp1 RELOP Exp2
        Operand t1=new_temp();
        Operand t2=new_temp();
        //code1
        Trans_Exp(n->child,t1);
        cout<<"done cond code1"<<endl;
        //code2
        Trans_Exp(n->child->next_sib->next_sib,t2);
        cout<<"done cond code2"<<endl;

        //[IF t1 op t2 GOTO label_true]
        InterCode code3=(InterCode)malloc(sizeof(struct InterCode_));//InterCode code3=new struct InterCode_;
        code3->kind=InterCode_::W_IFGOTO;
        code3->u.Three.x=t1;
        code3->u.Three.y=t2;
        //printf("%s\n",n->child->next_sib->str_constant);
        string tar(n->child->next_sib->str_constant);
        code3->u.Three.relop=tar;
        code3->u.Three.label=create_label(label_true);
        interInsert(code3);

        //[GOTO label_false]
        InterCode code4=(InterCode)malloc(sizeof(struct InterCode_));//InterCode code4=new struct InterCode_;
        code4->kind=InterCode_::W_GOTO;
        code4->u.Single.op=create_label(label_false);
        interInsert(code4);
    }
    else if(strcmp(n->child->name,"NOT")==0){
        Trans_Cond(n->child->next_sib,label_false,label_true);
    }
    else if(n->child->next_sib!=NULL && strcmp(n->child->next_sib->name,"AND")==0){
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
    else if(n->child->next_sib!=NULL && strcmp(n->child->next_sib->name,"OR")==0){
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
        cout<<"条件表达式的other cases!!!!"<<endl;

        Operand t1=new_temp();
        //code1
        Trans_Exp(n,t1);
        //code2  IF t1!=#0 GOTO label_true
        InterCode code3=(InterCode)malloc(sizeof(struct InterCode_));
        code3->kind=InterCode_::W_IFGOTO;
        code3->u.Three.x=t1;

        Operand t2=(Operand)malloc(sizeof(struct Operand_));
        t2->kind=Operand_::CONSTANT;
        t2->u.strVal="#0";
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
    // Cond: other cases
}

void Trans_Exp(Node* n, Operand place){
    if(n->child->next_sib==NULL && strcmp(n->child->name,"ID")==0){   //#include <cstring>
        cout<<"Exp->ID"<<endl;
        
        place->kind=Operand_::VARIABLE;
        string tar(n->child->str_constant);
        place->u.strVal=tar;
    }
    else if(strcmp(n->child->name,"INT")==0){
        cout<<"Exp->INT"<<endl;
        /*
        Operand op=(Operand)malloc(sizeof(struct Operand_));
        op->kind=Operand_::CONSTANT;
        op->u.intVal=n->child->int_constant;

        InterCode code=(InterCode)malloc(sizeof(struct InterCode_));
        code->kind=InterCode_::W_ASSIGN;
        code->u.Assign.left=place;
        code->u.Assign.right=op;
        interInsert(code);
        */
        place->kind=Operand_::CONSTANT;
        string tmp="#"+to_string(n->child->int_constant);
        place->u.strVal=tmp;
    }
    else if(strcmp(n->child->name,"NOT")==0|| strcmp(n->child->next_sib->name,"RELOP")==0|| strcmp(n->child->next_sib->name,"AND")==0|| strcmp(n->child->next_sib->name,"OR")==0){
        //逻辑运算
        //NOT Exp
        //Exp AND|OR|RELOP Exp
        Trans_Exp_Logic(n,place);
    }
    else if(strcmp(n->child->next_sib->name,"PLUS")==0||strcmp(n->child->next_sib->name,"MINUS")==0||strcmp(n->child->next_sib->name,"STAR")==0||strcmp(n->child->next_sib->name,"DIV")==0){
        //算数运算Exp PLUS|MINUS|STAR|DIV Exp
        Trans_Exp_MATH(n,place);
    }
    else if(strcmp(n->child->name,"LP")==0 ){
        //LP Exp RP
        cout<<"Exp->LP Exp RP"<<endl;
        Trans_Exp(n->child->next_sib,place);
    }
    else if(strcmp(n->child->name,"MINUS")==0){
        //Minus Exp
        Trans_Exp_MINUS(n,place);
    }
    else if(strcmp(n->child->next_sib->name,"ASSIGNOP")==0){
        Trans_Exp_ASSIGNOP(n,place);
    }
    else if(strcmp(n->child->next_sib->name,"DOT")==0){
        ;
    }
    else if(strcmp(n->child->next_sib->name,"LB")==0){
        //Exp LB Exp RB
        Trans_Exp_Array(n,place);
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

void Trans_Exp_Array(Node* n,Operand place){
    // Exp -> Exp1 LB Exp RB      
    //Exp1 -> ID
    //1
    Operand t1=new_temp();
    Trans_Exp(n->child->next_sib->next_sib,t1);
    //2
    Operand cons4=(Operand)malloc(sizeof(struct Operand_));
    cons4->kind=Operand_::CONSTANT;
    cons4->u.strVal="#4";

    Operand t2=new_temp();
    InterCode code2=(InterCode)malloc(sizeof(struct InterCode_));
    code2->kind=InterCode_::W_MUL;
    code2->u.Double.result=t2;
    code2->u.Double.op1=t1;
    code2->u.Double.op2=cons4;
    interInsert(code2);
    //3
    Operand arrID=(Operand)malloc(sizeof(struct Operand_));
    arrID->kind=Operand_::VARIABLE;
    string tar(n->child->child->str_constant);
    arrID->u.strVal=tar;

    Operand t3=new_temp();
    InterCode code3=(InterCode)malloc(sizeof(struct InterCode_));
    code3->kind=InterCode_::W_GET_ADDR;
    code3->u.Double.result=t3;
    code3->u.Double.op1=arrID;
    code3->u.Double.op2=t2;
    interInsert(code3);
    //4
    InterCode code4=(InterCode)malloc(sizeof(struct InterCode_));
    code4->kind=InterCode_::W_GET_VAL;
    code4->u.Assign.left=place;
    code4->u.Assign.right=t3;
    interInsert(code4);
}
void Trans_Exp_Logic(Node* n,Operand place){
    //NOT Exp
    //Exp AND|OR|RELOP Exp
    cout<<"Exp 逻辑！！！特别"<<endl;

    Operand label1=new_label();
    int x1=label1->u.intVal;
    Operand label2=new_label();
    int x2=label2->u.intVal;

    //[place := #0]
    Operand op=(Operand)malloc(sizeof(struct Operand_));
    op->kind=Operand_::CONSTANT;
    op->u.strVal="#0";

    InterCode code=(InterCode)malloc(sizeof(struct InterCode_));
    code->kind=InterCode_::W_ASSIGN;
    code->u.Assign.left=place;
    code->u.Assign.right=op;
    interInsert(code);

    //code1
    Trans_Cond(n,x1,x2);

    //[LABEL label1]
    InterCode codeLabel=(InterCode)malloc(sizeof(struct InterCode_));
    codeLabel->kind=InterCode_::W_LABEL;
    //codeLabel->u.Single.op->u.intVal=x1;  //bug
    codeLabel->u.Single.op=create_label(x1);
    interInsert(codeLabel);

    //[place:=#1]
    Operand op1=(Operand)malloc(sizeof(struct Operand_));
    op1->kind=Operand_::CONSTANT;
    op1->u.strVal="#1";

    InterCode code2=(InterCode)malloc(sizeof(struct InterCode_));
    code2->kind=InterCode_::W_ASSIGN;
    code2->u.Assign.left=place;
    code2->u.Assign.right=op1;
    interInsert(code2);

    //[LABEL label2]
    InterCode codeLabel2=(InterCode)malloc(sizeof(struct InterCode_));
    codeLabel2->kind=InterCode_::W_LABEL;
    codeLabel2->u.Single.op=create_label(x2);
    interInsert(codeLabel2);
}
void Trans_Exp_MINUS(Node* n,Operand place){
    //Minus Exp
    Operand t1=new_temp();
    //code1
    Trans_Exp(n->child->next_sib,t1);
    //[place := #0 - t1]
    if(place->kind==Operand_::NONE) return;

    Operand op1=(Operand)malloc(sizeof(struct Operand_));
    op1->kind=Operand_::CONSTANT;
    op1->u.strVal="#0";

    InterCode code=(InterCode)malloc(sizeof(struct InterCode_));
    code->kind=InterCode_::W_SUB;
    if(place->kind==Operand_::NONE) code->u.Double.result=new_temp();
    else code->u.Double.result=place;
    code->u.Double.op1=op1;
    code->u.Double.op2=t1;
    interInsert(code);
}
void Trans_Exp_MATH(Node* n,Operand place){
    // Exp PLUS Exp
    Operand t1=new_temp();
    Operand t2=new_temp();
    //code1
    Trans_Exp(n->child,t1);
    //code2
    Trans_Exp(n->child->next_sib->next_sib,t2);
    //[place := t1 op t2]
    if(place==NULL) return;
    InterCode code=(InterCode)malloc(sizeof(struct InterCode_));
    if(strcmp(n->child->next_sib->name,"PLUS")==0)
        code->kind=InterCode_::W_ADD;
    else if(strcmp(n->child->next_sib->name,"MINUS")==0)
        code->kind=InterCode_::W_SUB;
    else if(strcmp(n->child->next_sib->name,"STAR")==0)
        code->kind=InterCode_::W_MUL;
    else if(strcmp(n->child->next_sib->name,"DIV")==0)
        code->kind=InterCode_::W_DIV;
    else
        cout<<"Exp_Math的操作符错了"<<endl;
    code->u.Double.result=place;
    code->u.Double.op1=t1;
    code->u.Double.op2=t2;
    interInsert(code);
}
void Trans_Exp_ASSIGNOP(Node* n,Operand place){
    //exp -> exp1 assignop exp
    //exp1 -> id | Exp LB exp RB
    cout<<"Exp_ASSIGNOP"<<endl;

    //Type type=map.at(id);
    //if(type->kind==Type_::BASIC){
    if(strcmp(n->child->child->name,"ID")==0){
        //等号左边是ID
        //exp -> exp1 assignop exp
        //exp1 -> id
        string id(n->child->child->str_constant);
        //code1
        Operand t1=new_temp();
        Trans_Exp(n->child->next_sib->next_sib,t1);
        //[variable.name:=t1]
        Operand op=(Operand)malloc(sizeof(struct Operand_));
        op->kind=Operand_::VARIABLE;
        op->u.strVal=id;

        InterCode code21=(InterCode)malloc(sizeof(struct InterCode_));
        code21->kind=InterCode_::W_ASSIGN;
        code21->u.Assign.left=op;
        code21->u.Assign.right=t1;
        interInsert(code21);
        //[place:=variable.name]
        if(place->kind!=Operand_::NONE){
            // Operand op2=(Operand)malloc(sizeof(struct Operand_));
            // op2->kind=Operand_::VARIABLE;
            // op2->u.strVal=id;

            InterCode code22=(InterCode)malloc(sizeof(struct InterCode_));
            code22->kind=InterCode_::W_ASSIGN;
            code22->u.Assign.left=place;
            code22->u.Assign.right=op;     
            interInsert(code22);
        }
    }
    else{
        //等号左边是一维数组类型
        //exp -> exp1 assignop exp
        //exp1 -> Exp LB Exp RB 
        //1
        Operand t1=new_temp();
        Trans_Exp(n->child->next_sib->next_sib,t1);
        //2
        Operand t2=new_temp();
        Trans_Exp(n->child->child->next_sib->next_sib,t2);
        //3
        Operand cons4=(Operand)malloc(sizeof(struct Operand_));
        cons4->kind=Operand_::CONSTANT;
        cons4->u.strVal="#4";

        Operand t3=new_temp();
        InterCode code3=(InterCode)malloc(sizeof(struct InterCode_));
        code3->kind=InterCode_::W_MUL;
        code3->u.Double.result=t3;
        code3->u.Double.op1=t2;
        code3->u.Double.op2=cons4;
        interInsert(code3);
        //4
        Operand arrID=(Operand)malloc(sizeof(struct Operand_));
        arrID->kind=Operand_::VARIABLE;
        string tar(n->child->child->child->str_constant);
        arrID->u.strVal=tar;

        Operand t4=new_temp();
        InterCode code4=(InterCode)malloc(sizeof(struct InterCode_));
        code4->kind=InterCode_::W_GET_ADDR;
        code4->u.Double.result=t4;
        code4->u.Double.op1=arrID;
        code4->u.Double.op2=t3;
        interInsert(code4);
        //5
        InterCode code5=(InterCode)malloc(sizeof(struct InterCode_));
        code5->kind=InterCode_::W_VAL_GOT;
        code5->u.Assign.left=t4;
        code5->u.Assign.right=t1;
        interInsert(code5);
        //6
        if(place->kind!=Operand_::NONE){
            InterCode code6=(InterCode)malloc(sizeof(struct InterCode_));
            code6->kind=InterCode_::W_GET_VAL;
            code6->u.Assign.left=place;
            code6->u.Assign.right=t4;
            interInsert(code6);
        } 
    }
}

void Trans_Exp_Func(Node* n,Operand place){
    //ID LP RP
    cout<<"Exp_Func"<<endl;
    string tar(n->child->str_constant);
    Type func=functionMap.at(tar);
    if(func->u.myfunc->name=="read"){
        InterCode code=(InterCode)malloc(sizeof(struct InterCode_));
        code->kind=InterCode_::W_READ;
        code->u.Single.op=place;//seg
        interInsert(code);
    }else{
        Operand op=(Operand)malloc(sizeof(struct Operand_));
        //op->kind=Operand_::VARIABLE;
        op->u.strVal=func->u.myfunc->name;

        InterCode code=(InterCode)malloc(sizeof(struct InterCode_));
        code->kind=InterCode_::W_CALL;
        if(place->kind==Operand_::NONE)code->u.Assign.left=new_temp();
        else code->u.Assign.left=place;
        code->u.Assign.right=op;
        interInsert(code);
    }
}
ArgList Trans_Args(Node* n,ArgList arg_list){
    if(n->child->next_sib==NULL){
        //Args->Exp
        //code1
        Operand t1=new_temp();
        Trans_Exp(n->child,t1);

        //arg_list=t1+arg_list
        ArgList arg_t1=(ArgList)malloc(sizeof(struct ArgList_));
        arg_t1->arg=t1;
        arg_t1->next=arg_list;
        return arg_t1;
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
        return Trans_Args(n->child->next_sib->next_sib,arg_list);
    }
}
void Trans_Exp_FuncParams(Node* n,Operand place){
    //Exp->ID LP Args RP

    //code1
    ArgList arg_list=NULL;
    arg_list=Trans_Args(n->child->next_sib->next_sib,arg_list);
    //write
    string tar(n->child->str_constant);
    Type func=functionMap.at(tar);
    if(func->u.myfunc->name=="write"){
        InterCode code=(InterCode)malloc(sizeof(struct InterCode_));
        code->kind=InterCode_::W_WRITE;
        //code->u.Single.op->u.intVal=arg_list->arg->u.intVal;
        code->u.Single.op=arg_list->arg;//bug
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
    if(place->kind==Operand_::NONE)code->u.Assign.left=new_temp();
    else code->u.Assign.left=place;
    code->u.Assign.right=funcName;
    interInsert(code);
}
