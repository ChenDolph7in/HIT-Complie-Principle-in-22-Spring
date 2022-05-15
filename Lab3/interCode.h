#include "analyse.h"

typedef struct Operand_ *Operand;
typedef struct InterCode_ *InterCode;
typedef struct InterCodes_ *InterCodes;
typedef struct InterCodeList_ *InterCodeList;
typedef struct Arg_ *Arg;
typedef struct ArgList_ *ArgList;

/* 全局变量 */
InterCodeList list;  //线型IR数据结构
int currentLabel;    //下一个Label标号
int currentTemp;     //下一个Temp标号
char *lastArrayName; //上一个数组名

/* 运算对象数据结构 */
struct Operand_
{
    enum{VARIABLE, CONSTANT,OP_LABEL,OP_RELOP,ADDRESS,OP_FUNCTION} kind;
    union{int value;char *name;} u;
};

/* 中间代码行数据结构 */
struct InterCode_
{
    enum{IR_ASSIGN,IR_ADD,IR_SUB,IR_MUL,IR_DIV,IR_FUNCTION,IR_DEC,IR_LABEL,IR_READ_ADDR,IR_WRITE_ADDR,IR_GET_ADDR,
        IR_GOTO,IR_IF_GOTO,IR_ARG,IR_PARAM,IR_CALL,IR_RETURN,IR_READ,IR_WRITE
    } kind;
    union
    {
        struct{Operand right, left;} assign;
        struct{Operand op;} uniop;
        struct{Operand result, op1, op2;} binop;
        struct{Operand x, relop, y, z;} leap;
        struct{Operand op;int size;} dec;
    } u;
};

/* 线型中间代码双向循环链表中间结点 */
struct InterCodes_
{
    InterCode code;
    InterCodes prev, next;
};

/* 线型中间代码数据结构，使用双向循环链表 */
struct InterCodeList_
{
    InterCodes head, tail;
};

/* 参数列表中间结点，仿照实验二memberVariable数据结构，变量替换为Operand类型 */
struct Arg_
{
    Operand op;
    Arg next;
};

/* 参数列表数据结构，单向链表 */
struct ArgList_
{
    Arg head, tail;
};

// 辅助数据结构操作函数
void initInterCodeList();
void insertInterCode(InterCodes node);
ArgList createArgList();
Arg createArg(Operand op);
void createInterCodeList(struct node *root);
void printInterCodeList(char *filename);
void printOp(FILE *fp, Operand op);
Operand createOperand(int kind, ...);
InterCode createInterCode(int kind, ...);
void genInterCode(int kind, ...);
InterCodes createInterCodes(InterCode code);
int getSize(struct type *type);
Operand createTemp();
Operand createLabel();

// 基本表达式翻译模式过程函数
void translate_ExtDefList(struct node *node);
void translate_ExtDef(struct node *node);
void translate_FunDec(struct node *node);
void translate_StmtList(struct node *node);
void translate_Stmt(struct node *node);
void translate_CompSt(struct node *node);
void translate_DefList(struct node *node);
void translate_Def(struct node *node);
void translate_DecList(struct node *node);
void translate_Dec(struct node *node);
void translate_VarDec(struct node *node, Operand place);
void translate_Cond(struct node *node, Operand labelTrue, Operand labelFalse);
void translate_Args(struct node *node, ArgList argList);
void translate_Exp(struct node *node, Operand place);