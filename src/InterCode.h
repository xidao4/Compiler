#include <string>
#include <fstream>
using namespace std;

typedef struct Operand_* Operand;
typedef struct InterCode_* InterCode;
typedef struct ArgList_* ArgList;

struct ArgList_{
    Operand arg;
    ArgList next;
};
struct Operand_{
    //enum{TMP_VAR, VARIABLE, CONSTANT, LABEL, MYFUNCTION, VAR_ADDR, TMP_ADDR, MYSTAR, NONE}kind;
    enum{TMP_VAR, VARIABLE, CONSTANT, LABEL, NONE}kind;
    union{
        Operand addr;
        int intVal;//标签序号 
        string strVal;//函数名称 常数的字符串 变量名 临时变量名t4
    }u;//操作数的属性不同
};
struct InterCode_{//单条中间代码。将中间代码划分为19种。
    enum{W_LABEL, W_FUNCTION, W_ASSIGN, W_ADD, W_SUB, W_MUL, W_DIV, 
    W_GET_ADDR, W_GET_VAL, W_VAL_GOT, 
    W_GOTO, W_IFGOTO, W_RETURN, W_DEC, W_ARG, W_CALL, W_PARAM, W_READ, W_WRITE}kind;
    union{
        struct{Operand op;}Single;
        struct{Operand op1, op2, result;}Double;
        struct{Operand x,y,label; string relop;}Three;
        struct{Operand left,right;}Assign;
        struct{Operand op;int size;}Dec;
    }u;//不同类型的中间代码有不同个数和种类的操作数
    InterCode prev;
    InterCode next;
};




void interInsert(InterCode a);
Operand new_temp();
Operand new_label();
Operand create_label(int x);

void printIR(InterCode head);
string printOperand(Operand op);

void Trans_Program(Node* n);
void Trans_ExtDefList(Node* n);
void Trans_ExtDef(Node* n);
void Trans_FunDec(Node* n);
void Trans_VarList(Node* n);
void Trans_ParamDec(Node* n);
void Trans_VarDec_in_FuncParam(Node* n);

void Trans_CompSt(Node* n);
void Trans_StmtList(Node* n);
void Trans_Stmt(Node* n);

void Trans_DefList_in_Function(Node* n);
void Trans_Def_in_Function(Node* n);
void Trans_DecList_in_Function(Node* n);
void Trans_Dec_in_Function(Node* n);
void Trans_VarDec_in_Function(Node* n);

void Trans_Cond(Node* n,int label_true,int label_false);


void Trans_Exp(Node* n, Operand place);
void Trans_Exp_Array(Node* n,Operand place);
void Trans_Exp_ASSIGNOP(Node* n,Operand place);
void Trans_Exp_Func(Node* n,Operand place);
void Trans_Exp_FuncParams(Node* n,Operand place);
ArgList Trans_Args(Node* n,ArgList arg_list);
void Trans_Exp_MATH(Node* n,Operand place);
void Trans_Exp_Logic(Node* n,Operand place);  // and or not relop
void Trans_Exp_MINUS(Node* n,Operand place);
