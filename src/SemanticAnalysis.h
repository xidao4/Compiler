#include <unordered_map>
#include <string>
using namespace std;

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct FuncList_* FuncList;
#define IS_INT 0
#define IS_FLOAT 1




struct Type_{
    enum{BASIC,ARRAY,STRUCTURE,FUNCTION,ERROR} kind;
    union{
        int basic;
        struct {Type elem; int size;} array;
        FieldList structure;
        FuncList myfunc;
        int error_type;
    }u;
};
struct FieldList_{
    string name;//域名
    Type type;//域的类型
    FieldList tail;//下一个域的指针
};
struct FuncList_{
    string name;//函数名&参数名
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
string OptTag(Node* n);
void DefList_in_Struct(Node* n,string optTag);
void Def_in_Struct(Node* n,string optTag);
void DecList_in_Struct(Node* n,string optTag,Type type);
void Dec_in_Struct(Node* n,string optTag,Type type);
void VarDec_in_Struct(Node* n,string optTag,Type type);//VarDec 结构体变量的声明
/*high-level definitions*/
void ExtDecList(Node* n,Type type);
/*declarition*/
void VarDec(Node* n,Type type);//VarDec 既可以是全局的变量，又可以在函数中使用来声明变量
/*definition in function*/
Type FunDec(Node* n,Type return_type);
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