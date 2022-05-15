#ifndef SEMANTIC_H
#define SEMENTIC_H

#include "main.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#define HASH_TABLE_SIZE 0x3fff

typedef enum sort
{
    BASIC,
    ARRAY,
    STRUCTURE,
    FUNCTION,
    NOT_FUNCTION,
    ALL
} Sort;

typedef enum errorType
{
    UNDEF_VAR = 1,        // Undefined Variable
    UNDEF_FUNC,           // Undefined Function
    REDEF_VAR,            // Redefined Variable
    REDEF_FUNC,           // Redefined Function
    TYPE_MISMATCH_ASSIGN, // Type mismatchedfor assignment.
    ASSIGN_LEFT_NOT_VAR,      // The left-hand side of an assignment must be a variable.
    TYPE_MISMATCH_OP,     // Type mismatched for operands.
    TYPE_MISMATCH_RETURN, // Type mismatched for return.
    FUNC_AGRC_MISMATCH,   // Function is not applicable for arguments
    NOT_A_ARRAY,          // Variable is not a Array
    NOT_A_FUNC,           // Variable is not a Function
    NOT_A_INT,            // Variable is not a Integer
    ILLEGAL_USE_DOT,      // Illegal use of "."
    NONEXIST_FIELD,        // Non-existentfield
    REDEF_FEILD,          // Redefined field
    DUPLICATED_NAME,      // Duplicated name
    UNDEF_STRUCT          // Undefined structure
} ErrorType;

struct hashHead *hashTable;

/* 类型 */
struct type
{
    Sort typeName;
    union
    {
        // 基本类型int/float
        char *TOKEN;

        // 数组
        struct
        {
            struct type *elemType; //元素类型
            int size;              //数组大小
        } ARRAY;

        // 结构体
        struct
        {
            char *structName;
            struct memberVariable *argv; //成员变量列表
        } STRUCT;

        // 函数
        struct
        {
            int argc;                    //参数数量
            struct memberVariable *argv; //参数列表
            struct type *returnType;     //返回值类型
        } FUN;
    } UNION;
};

/* 变量列表，包括结构体成员变量列表或函数参数列表*/
struct memberVariable
{
    char *name;
    struct type *memType;
    struct memberVariable *next;
};

/* 哈希桶 */
struct hashHead
{
    struct hashList *list; //第一个哈希表项
};

/* 哈希表项 */
struct hashList
{
    char *name;
    struct type *myType;
    int isArg;
    struct hashList *next; //相同桶内冲突表项
};

unsigned int hash_pjw(char *name);
struct hashHead *initHashTable();
struct hashList *lookup(char *name,Sort type);
struct type *createType(Sort sort, ...);
void insertIntoHashTable(struct hashList *node);
int checkType(struct type *t1, struct type *t2);
void printError(ErrorType type, int line, char *msg);

void ExtDef(struct node *node);
void ExtDecList(struct node *node, struct type *specifier);
struct type *Specifier(struct node *node);
struct type *StructSpecifier(struct node *node);
struct hashList *VarDec(struct node *node, struct type *specifier);
void FunDec(struct node *node, struct type *returnType);
void VarList(struct node *node, struct hashList *func);
struct memberVariable *ParamDec(struct node *node);
void CompSt(struct node *node, struct type *returnType);
void StmtList(struct node *node, struct type *returnType);
void Stmt(struct node *node, struct type *returnType);
void DefList(struct node *node, struct hashList *structInfo);
void Def(struct node *node, struct hashList *structInfo);
void DecList(struct node *node, struct type *specifier, struct hashList *structInfo);
void Dec(struct node *node, struct type *specifier, struct hashList *structInfo);
struct type *Exp(struct node *node);
void Args(struct node *node, struct hashList *funcInfo);

int semanticAnalyse(struct node *root);

#endif