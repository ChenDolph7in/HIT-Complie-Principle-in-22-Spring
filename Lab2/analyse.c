#include "analyse.h"
int errnum; //产生错误数目

/* 打印哈希表，DEBUG使用 */
void printHashTable()
{
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        struct hashList *list = hashTable[i].list;
        while (list != NULL)
        {
            printf("hash:%s\t", list->name);
            if (list->myType->typeName == BASIC)
            {
                printf("type:%s\t", list->myType->UNION.TOKEN);
            }
            list = list->next;
        }
    }
    printf("\n");
}

/* 输入字符串，返回哈希值 */
unsigned int hash_pjw(char *name)
{
    // printf("hash_hwj\n");
    char *temp = name;
    unsigned int val = 0, i;
    for (; *name; ++name)
    {
        val = (val << 2) + *name;
        if (i = val & ~HASH_TABLE_SIZE)
            val = (val ^ (i >> 12)) & HASH_TABLE_SIZE;
    }
    return val;
}

/* 初始化哈希表 */
struct hashHead *initHashTable()
{
    errnum = 0;
    hashTable = (struct hashHead *)malloc(sizeof(struct hashHead) * (HASH_TABLE_SIZE + 1));
    struct hashHead *temp = hashTable;
    for (int i = 0; i <= HASH_TABLE_SIZE; i++)
    {
        temp[i].list = NULL;
    }
    return hashTable;
}

/* 在哈希表中查找某个字符，返回以该字符为name的哈希表节点 */
struct hashList *lookup(char *name, Sort type)
{
    // printf("lookup :%s\n", name);
    unsigned int val = hash_pjw(name);
    struct hashList *list = hashTable[val].list;
    while (list != NULL)
    {
        if (!strcmp(list->name, name))
        {
            if (type == ALL)
            {
                if (list->myType->typeName != ALL & list->myType->typeName != NOT_FUNCTION)
                    return list;
            }
            else if (type == NOT_FUNCTION)
            {
                if (list->myType->typeName != FUNCTION && list->myType->typeName != ALL & list->myType->typeName != NOT_FUNCTION)
                    return list;
            }
            else
            {
                if (list->myType->typeName == type)
                    return list;
            }
        }
        list = list->next;
    }
    // printHashTable();
    return NULL;
}

/*
    创建新的类型,参数如下
    基本类型:   BASIC,
    数组：      ARRAY, 元素类型,数目
    结构体：    STRUCTURE,结构体名,成员变量列表
    函数：      FUNCTION,函数变量数,函数变量列表,函数返回类型
*/
struct type *createType(Sort sort, ...)
{
    // printf("createType\n");
    struct type *newType = (struct type *)malloc(sizeof(struct type));
    va_list vaList;
    newType->typeName = sort;
    switch (sort)
    {
    case BASIC:
        va_start(vaList, sort);
        newType->UNION.TOKEN = va_arg(vaList, char *);
        break;
    case ARRAY:
        va_start(vaList, sort);
        newType->UNION.ARRAY.elemType = va_arg(vaList, struct type *);
        newType->UNION.ARRAY.size = va_arg(vaList, int);
        break;
    case STRUCTURE:
        va_start(vaList, sort);
        newType->UNION.STRUCT.structName = va_arg(vaList, char *);
        newType->UNION.STRUCT.argv = va_arg(vaList, struct memberVariable *);
        break;
    case FUNCTION:
        va_start(vaList, sort);
        newType->UNION.FUN.argc = va_arg(vaList, int);
        newType->UNION.FUN.argv = va_arg(vaList, struct memberVariable *);
        newType->UNION.FUN.returnType = va_arg(vaList, struct type *);
        break;
    default:
        free(newType);
        newType = NULL;
        break;
    }
    return newType;
}

/* 向哈希表合适位置中插入新节点,匿名结构体插入下标为HASH_TABLE_SIZE哈希链表尾 */
void insertIntoHashTable(struct hashList *node)
{
    // printf("insertIntoHashTable %s\n", node->name);
    struct hashList *list;
    unsigned int val = 0;

    if (node->myType->typeName == STRUCTURE && node->name == NULL)
    {
        val = HASH_TABLE_SIZE;
        list = hashTable[HASH_TABLE_SIZE].list;
    }
    else if (node->name == NULL)
    {
        printf("not-struct unnamed\n");
    }
    else
    {
        val = hash_pjw(node->name);
        list = hashTable[val].list;
    }
    if (list == NULL)
    {
        hashTable[val].list = node;
        node->next = NULL;
    }
    else
    {
        while (list->next)
        {
            list = list->next;
        }
        list->next = node;
        node->next = NULL;
    }
}

/* 检查两个类型是否可进行匹配,若可以返回1,否则返回0 */
int checkType(struct type *t1, struct type *t2)
{
    // printf("checkType\n");
    if (t1 == NULL || t2 == NULL)
        return 1;
    if (t1->typeName == FUNCTION || t2->typeName == FUNCTION)
        return 0;
    if (t1->typeName != t2->typeName)
        return 0;
    else
    {
        switch (t1->typeName)
        {
        case BASIC:
            return t1->UNION.TOKEN == t2->UNION.TOKEN;
        case ARRAY:
            return checkType(t1->UNION.ARRAY.elemType, t2->UNION.ARRAY.elemType);
        case STRUCTURE:
            return !strcmp(t1->UNION.STRUCT.structName,
                           t2->UNION.STRUCT.structName);
        }
    }
}

/* 打印错误信息 */
void printError(ErrorType type, int line, char *msg)
{
    errnum++;
    printf("Error type %d at Line %d: %s\n", type, line, msg);
}

// ExtDef -> Specifier ExtDecList SEMI | Specifier SEMI | Specifier FunDec CompSt
void ExtDef(struct node *node)
{
    // printf("ExtDef\n");
    struct type *specifierType = Specifier(node->left_sons);
    // Specifier -> TYPE
    if (!strcmp(node->left_sons->left_sons->token, "TYPE"))
    {
        // ExtDef -> Specifier ExtDecList SEMI
        if (!strcmp(node->left_sons->siblings->token, "ExtDecList"))
        {
            ExtDecList(node->left_sons->siblings, specifierType);
        }
        // ExtDef -> Specifier FunDec CompSt
        else if (!strcmp(node->left_sons->siblings->token, "FunDec"))
        {
            FunDec(node->left_sons->siblings, specifierType);
            CompSt(node->left_sons->siblings->siblings, specifierType);
        }
    }
    // Specifier -> StructSpecifier
    else if (!strcmp(node->left_sons->left_sons->token, "StructSpecifier"))
    {
        // do nothing
    }
    else
    {
        printf("ExtDef Error\n");
    }
}

/*
    ExtDecList -> VarDec | VarDec COMMA ExtDecList
    解析ExtDecList
*/
void ExtDecList(struct node *node, struct type *specifier)
{
    // printf("ExtDecList\n");
    struct node *temp = node;
    // ExtDecList -> VarDec
    struct hashList *hashNode = VarDec(temp->left_sons, specifier);
    struct hashList *searchNode = lookup(hashNode->name, NOT_FUNCTION);
    if (hashNode->myType->typeName == searchNode->myType->typeName)
    {
        char msg[100] = {0};
        sprintf(msg, "Redefined variable \"%s\".", hashNode->name);
        printError(REDEF_VAR, temp->lineno, msg);
    }
    else
    {
        insertIntoHashTable(hashNode);
    }
    // ExtDecList -> VarDec COMMA ExtDecList
    if (temp->left_sons->siblings)
    {
        ExtDecList(temp->left_sons->siblings->siblings, specifier);
    }
}

/*
    Specifier -> TYPE | StructSpecifier
    解析类型描述符,解析并返回类型描述符表示的BASIC或STRUCTURE类型
*/
struct type *Specifier(struct node *node)
{
    // printf("Specifier\n");
    struct node *temp = node->left_sons;
    // Specifier -> TYPE
    if (!strcmp(temp->token, "TYPE"))
    {
        if (!strcmp(temp->type, "float"))
        {
            return createType(BASIC, "FLOAT");
        }
        // INT_OCT,INT_DEX,INT_HEX
        else
        {
            return createType(BASIC, "INT");
        }
    }
    // Specifier -> StructSpecifier
    else
    {
        return StructSpecifier(temp);
    }
}

/*
    StructSpecifier -> STRUCT OptTag LC DefList RC | STRUCT Tag
    解析结构体类型描述符，返回STRUCTURE类型
*/
struct type *StructSpecifier(struct node *node)
{
    // printf("StructSpecifier\n");
    struct node *temp = node->left_sons->siblings; // OptTag / Tag
    struct type *returnType = NULL;                // 返回类型

    // OptTag -> ID | e
    if (strcmp(temp->token, "Tag"))
    {
        // 非Tag,表示新定义结构体,创建新节点
        struct hashList *hashNode = (struct hashList *)malloc(sizeof(struct hashList));
        struct type *myType = createType(STRUCTURE, NULL, NULL); //确定为结构体类型,结构体名、成员变量列表暂定为NULL
        hashNode->myType = myType;
        hashNode->name = NULL; //新定义结构体名暂定为NULL
        // StructSpecifier -> STRUCT OptTag LC DefList RC
        if (!strcmp(temp->token, "OptTag"))
        {
            //有OptTag,为非匿名结构体,设定结构体名
            hashNode->name = temp->left_sons->type;
            temp = temp->siblings;
        }
        // StructSpecifier -> STRUCT LC DefList RC
        else
        {
            //没有OptTag,为匿名结构体,结构体名为NULL(已初始化为NULL,可省略)
            hashNode->name = NULL;
        }
        if (!strcmp(temp->siblings->token, "DefList"))
        {
            DefList(temp->siblings, hashNode);
        }
        // 将新定义的结构体插入符号表
        if ((hashNode->name != NULL) && (lookup(hashNode->name, STRUCTURE)))
        {
            char msg[100] = {0};
            sprintf(msg, "Duplicated name \"%s\".", hashNode->name);
            printError(DUPLICATED_NAME, node->lineno, msg);
        }
        else
        {
            returnType = myType;
            insertIntoHashTable(hashNode);
        }
    }
    // Tag -> ID
    else
    {
        // 创建结构体类型，在符号表中找到该节点
        struct hashList *hashNode = lookup(temp->left_sons->type, STRUCTURE);
        if (hashNode == NULL)
        {
            char msg[100] = {0};
            sprintf(msg, "Undefined structure \"%s\".", temp->left_sons->type);
            printError(UNDEF_STRUCT, node->lineno, msg);
            returnType = createType(STRUCTURE, NULL, NULL);
        }
        else
        {
            // 返回找到的STRUCTURE节点的成员变量列表
            if (hashNode->myType->typeName != STRUCTURE)
            {
                char msg[100] = {0};
                sprintf(msg, "Undefined structure \"%s\".", temp->left_sons->type);
                printError(UNDEF_STRUCT, node->lineno, msg);
                hashNode->myType = createType(STRUCTURE, NULL, NULL);
                returnType = createType(STRUCTURE, NULL, NULL);
            }
            else
            {
                returnType = hashNode->myType;
            }
        }
    }
    return returnType;
}

/*
    VarDec -> ID | VarDec LB INT RB
    解析变量定义,第一层基本类型为参数specifier,最终类型可能为数组
    返回新符号在哈希表中的节点
*/
struct hashList *VarDec(struct node *node, struct type *specifier)
{
    // printf("VarDec\n");
    struct node *id = node;
    while (id->left_sons)
        id = id->left_sons;
    struct hashList *hashNode = (struct hashList *)malloc(sizeof(struct hashList));
    hashNode->name = id->type;
    hashNode->next = NULL;
    if (!strcmp(node->left_sons->token, "ID"))
    {
        hashNode->myType = specifier;
    }
    // VarDec -> VarDec LB INT RB
    else
    {
        struct node *varDec = node->left_sons;
        struct type *temp = specifier;
        // printf("VarDec -> VarDec LB INT RB.\n");
        while (varDec->siblings)
        {
            hashNode->myType = createType(ARRAY, temp, varDec->siblings->siblings->integer_value);
            temp = hashNode->myType;
            varDec = varDec->left_sons;
        }
    }
    return hashNode;
}

/*
    FunDec -> ID LP VarList RP | ID LP RP
    解析函数定义,将新的函数符号插入哈希表,返回值类型为参数returnType
*/
void FunDec(struct node *node, struct type *returnType)
{
    // printf("FunDec\n");
    // 判断声明的函数是否存在
    struct hashList *temp = lookup(node->left_sons->type, FUNCTION);
    if ((temp == NULL) || (temp != NULL && temp->myType->typeName != FUNCTION))
    {
        struct hashList *hashNode = (struct hashList *)malloc(sizeof(struct hashList));
        hashNode->myType = createType(FUNCTION, 0, NULL, NULL);
        hashNode->name = node->left_sons->type;
        hashNode->next = NULL;
        // FunDec -> ID LP VarList RP
        if (!strcmp(node->left_sons->siblings->siblings->token, "VarList"))
        {
            VarList(node->left_sons->siblings->siblings, hashNode);
        }
        insertIntoHashTable(hashNode);
    }
    else
    {
        char msg[100] = {0};
        sprintf(msg, "Redefined function \"%s\".", node->left_sons->type);
        printError(REDEF_FUNC, node->lineno, msg);
    }
}

/*
    VarList -> ParamDec COMMA VarList | ParamDec
    解析函数形参列表,将新解析出的变量不断插入func函数的哈希表节点中的形参列表中
*/
void VarList(struct node *node, struct hashList *func)
{
    // printf("VarList\n");
    int argc = 0;
    struct node *temp = node->left_sons;

    // VarList -> ParamDec
    struct memberVariable *memList = ParamDec(temp);
    struct memberVariable *tempMemList = NULL;
    func->myType->UNION.FUN.argv = memList;
    tempMemList = memList;
    argc++;

    // VarList -> ParamDec COMMA VarList
    while (temp->siblings)
    {
        temp = temp->siblings->siblings->left_sons;
        memList = ParamDec(temp);
        if (memList)
        {
            tempMemList->next = memList;
            tempMemList = tempMemList->next;
            argc++;
        }
    }

    func->myType->UNION.FUN.argc = argc;
}

/*
    ParamDec -> Specifier VarDec
    解析单个形参定义,返回新解析出的参数列表节点
*/
struct memberVariable *ParamDec(struct node *node)
{
    // printf("ParamDec\n");
    struct type *specifierType = Specifier(node->left_sons);
    if (specifierType != NULL)
    {
        struct hashList *hashNode = VarDec(node->left_sons->siblings, specifierType);

        struct hashList *temp = lookup(hashNode->name, NOT_FUNCTION);
        if ((temp == NULL) || (temp != NULL && temp->myType->typeName != FUNCTION))
        {
            insertIntoHashTable(hashNode);
            struct memberVariable *memListNode = (struct memberVariable *)malloc(sizeof(struct memberVariable));
            memListNode->memType = hashNode->myType;
            memListNode->name = hashNode->name;
            memListNode->next = NULL;
            return memListNode;
        }
        else
        {
            char msg[100] = {0};
            sprintf(msg, "Redefined variable \"%s\".", hashNode->name);
            printError(REDEF_VAR, node->lineno, msg);
            return NULL;
        }
    }
    return NULL;
}

/*
    CompSt -> LC DefList StmtList RC
    解析{}语句块
*/
void CompSt(struct node *node, struct type *returnType)
{
    // printf("CompSt\n");
    struct node *temp = node->left_sons->siblings;
    if (!strcmp(temp->token, "DefList"))
    {
        DefList(temp, NULL);
        temp = temp->siblings;
    }
    if (!strcmp(temp->token, "StmtList"))
    {
        StmtList(temp, returnType);
    }
}

/*
    StmtList -> Stmt StmtList | e
    解析Stmt列表
*/
void StmtList(struct node *node, struct type *returnType)
{
    // printf("StmtList\n");
    while (node->left_sons)
    {
        Stmt(node->left_sons, returnType);
        node = node->left_sons->siblings;
    }
}

/*
    Stmt -> Exp SEMI
        | CompSt
        | RETURN Exp SEMI
        | IF LP Exp RP Stmt
        | IF LP Exp RP Stmt ELSE Stmt
        | WHILE LP Exp RP Stmt
    解析StmtList中的一条语句
*/
void Stmt(struct node *node, struct type *returnType)
{
    // printf("Stmt\n");
    struct type *expType = NULL;
    // Stmt -> Exp SEMI
    if (!strcmp(node->left_sons->token, "Exp"))
    {
        expType = Exp(node->left_sons);
    }

    // Stmt -> CompSt
    else if (!strcmp(node->left_sons->token, "CompSt"))
        CompSt(node->left_sons, returnType);

    // Stmt -> RETURN Exp SEMI
    else if (!strcmp(node->left_sons->token, "RETURN"))
    {
        expType = Exp(node->left_sons->siblings);

        if (!checkType(returnType, expType))
            printError(TYPE_MISMATCH_RETURN, node->lineno, "Type mismatched for return.");
    }

    // Stmt -> IF LP Exp RP Stmt
    else if (!strcmp(node->left_sons->token, "IF"))
    {
        struct node *stmt = node->left_sons->siblings->siblings->siblings->siblings;
        expType = Exp(node->left_sons->siblings->siblings);
        Stmt(stmt, returnType);
        // Stmt -> IF LP Exp RP Stmt ELSE Stmt
        if (stmt->siblings != NULL)
            Stmt(stmt->siblings->siblings, returnType);
    }

    // Stmt -> WHILE LP Exp RP Stmt
    else if (!strcmp(node->left_sons->token, "WHILE"))
    {
        expType = Exp(node->left_sons->siblings->siblings);
        Stmt(node->left_sons->siblings->siblings->siblings->siblings, returnType);
    }
}

/*
    DefList -> Def DefList | e
    解析变量定义列表
*/
void DefList(struct node *node, struct hashList *structInfo)
{
    // printf("DefList\n");
    while (node->left_sons)
    {
        Def(node->left_sons, structInfo);
        node = node->left_sons->siblings;
    }
}

/*
    Def -> Specifier DecList SEMI
    解析变量定义语句
*/
void Def(struct node *node, struct hashList *structInfo)
{
    // printf("Def\n");
    struct type *dectype = Specifier(node->left_sons);
    DecList(node->left_sons->siblings, dectype, structInfo);
}

/*
    DecList -> Dec | Dec COMMA DecList
    解析变量声明列表
*/
void DecList(struct node *node, struct type *specifier, struct hashList *structInfo)
{
    // printf("DecList\n");
    struct node *temp = node;
    while (temp)
    {
        Dec(temp->left_sons, specifier, structInfo);
        if (temp->left_sons->siblings)
            temp = temp->left_sons->siblings->siblings;
        else
            break;
    }
}

/*
    Dec -> VarDec | VarDec ASSIGNOP Exp
    解析单个变量声明
*/
void Dec(struct node *node, struct type *specifier, struct hashList *structInfo)
{
    // printf("Dec\n");
    //  Dec -> VarDec
    if (node->left_sons->siblings == NULL)
    {
        if (structInfo != NULL)
        {
            struct hashList *decitem = VarDec(node->left_sons, specifier);
            struct memberVariable *payload = (struct memberVariable *)malloc(sizeof(struct memberVariable));
            payload->memType = decitem->myType;
            payload->next = NULL;
            payload->name = decitem->name;
            struct memberVariable *structMemVar = structInfo->myType->UNION.STRUCT.argv;
            struct memberVariable *last = NULL;
            while (structMemVar != NULL)
            {
                if (!strcmp(payload->name, structMemVar->name))
                {
                    //出现重定义，报错
                    char msg[100] = {0};
                    sprintf(msg, "Redefined field \"%s\".", decitem->name);
                    printError(REDEF_FEILD, node->lineno, msg);
                    return;
                }
                else
                {
                    last = structMemVar;
                    structMemVar = structMemVar->next;
                }
            }
            //新建一个fieldlist,删除之前的item
            struct memberVariable *firstMemVar = (struct memberVariable *)malloc(sizeof(struct memberVariable));
            firstMemVar->memType = decitem->myType;
            firstMemVar->name = decitem->name;
            firstMemVar->next = NULL;
            if (last == NULL)
            {
                structInfo->myType->UNION.STRUCT.argv = firstMemVar;
            }
            else
            {
                last->next = firstMemVar;
            }
        }
        else
        {
            // 非结构体内，判断返回的item有无冲突，无冲突放入表中，有冲突报错delete
            struct hashList *decitem = VarDec(node->left_sons, specifier);
            struct hashList *temp = lookup(decitem->name, NOT_FUNCTION);
            if ((temp == NULL) || (temp != NULL && temp->myType->typeName == FUNCTION))
            {
                // printf("ready to insert\n");
                insertIntoHashTable(decitem);
            }
            else
            {
                //出现冲突，报错
                char msg[100] = {0};
                sprintf(msg, "Redefined variable \"%s\".", decitem->name);
                printError(REDEF_VAR, node->lineno, msg);
            }
        }
    }
    // Dec -> VarDec ASSIGNOP Exp
    else
    {
        if (structInfo != NULL)
        {
            // 结构体内不能赋值，报错
            printError(REDEF_FEILD, node->lineno, "Illegal initialize variable in struct.");
        }
        else
        {
            // 判断赋值类型是否相符
            // 如果相符，注册该符号
            struct hashList *decitem = VarDec(node->left_sons, specifier);
            struct type *exptype = Exp(node->left_sons->siblings->siblings);
            struct hashList *temp = lookup(decitem->name, ALL);
            if (!((temp == NULL) || (temp != NULL && temp->myType->typeName == FUNCTION)))
            {
                //出现冲突，报错
                char msg[100] = {0};
                sprintf(msg, "Redefined variable \"%s\".",
                        decitem->name);
                printError(REDEF_VAR, node->lineno, msg);
            }
            if (!checkType(decitem->myType, exptype))
            {
                //类型不相符
                //报错
                printError(TYPE_MISMATCH_ASSIGN, node->lineno,
                           "Type mismatchedfor assignment.");
            }
            if (decitem->myType && decitem->myType->typeName == ARRAY)
            {
                //报错，对非basic类型赋值
                printError(TYPE_MISMATCH_ASSIGN, node->lineno,
                           "Illegal initialize variable.");
            }
            else
            {
                insertIntoHashTable(decitem);
            }
        }
    }
}

/*
    Exp -> Exp ASSIGNOP Exp
        | Exp AND Exp
        | Exp OR Exp
        | Exp RELOP Exp
        | Exp PLUS Exp
        | Exp MINUS Exp
        | Exp STAR Exp
        | Exp DIV Exp
        | Exp LB Exp RB
        | Exp DOT ID
        | LP Exp RP
        | MINUS Exp
        | NOT Exp
        | ID LP Args RP
        | ID LP RP
        | ID
        | INT
        | FLOAT
    解析表达式
*/
struct type *Exp(struct node *node)
{
    // printf("Exp\n");
    struct node *t = node->left_sons;
    //二值运算
    if (!strcmp(t->token, "Exp"))
    {
        // 基本数学运算符
        if (strcmp(t->siblings->token, "LB") && strcmp(t->siblings->token, "DOT"))
        {
            struct type *p1 = Exp(t);
            struct type *p2 = Exp(t->siblings->siblings);
            struct type *returnType = NULL;

            // Exp -> Exp ASSIGNOP Exp
            if (!strcmp(t->siblings->token, "ASSIGNOP"))
            {
                //检查左值
                struct node *tchild = t->left_sons;

                if (!strcmp(tchild->token, "FLOAT") ||
                    !strcmp(tchild->token, "INT_OCT") ||
                    !strcmp(tchild->token, "INT_DEX") ||
                    !strcmp(tchild->token, "INT_HEX"))
                {
                    //报错，左值
                    printError(ASSIGN_LEFT_NOT_VAR, t->lineno, "The left-hand side of an assignment must be a variable.");
                }
                else if (!strcmp(tchild->token, "ID") ||
                         !strcmp(tchild->siblings->token, "LB") ||
                         !strcmp(tchild->siblings->token, "DOT"))
                {
                    if (!checkType(p1, p2))
                    {
                        //报错，类型不匹配
                        printError(TYPE_MISMATCH_ASSIGN, t->lineno, "Type mismatched for assignment.");
                    }
                    else
                    {
                        // returnType = copyType(p1);
                        returnType = p1;
                    }
                }
                else
                {
                    //报错，左值
                    printError(ASSIGN_LEFT_NOT_VAR, t->lineno, "The left-hand side of an assignment must be avariable.");
                }
            }
            // Exp -> Exp AND Exp
            //      | Exp OR Exp
            //      | Exp RELOP Exp
            //      | Exp PLUS Exp
            //      | Exp MINUS Exp
            //      | Exp STAR Exp
            //      | Exp DIV Exp
            else
            {
                if (p1 && p2 && (p1->typeName == ARRAY || p2->typeName == ARRAY))
                {
                    //报错，数组，结构体运算
                    printError(TYPE_MISMATCH_OP, t->lineno, "Type mismatched for operands.");
                }
                else if (!checkType(p1, p2))
                {
                    //报错，类型不匹配
                    printError(TYPE_MISMATCH_OP, t->lineno, "Type mismatched for operands.");
                }
                else
                {
                    if (p1 && p2)
                    {
                        returnType = p1;
                    }
                }
            }

            return returnType;
        }
        // 数组和结构体访问
        else
        {
            // Exp -> Exp LB Exp RB
            if (!strcmp(t->siblings->token, "LB"))
            {

                //数组
                struct type *p1 = Exp(t);
                struct type *p2 = Exp(t->siblings->siblings);
                struct type *returnType = NULL;

                if (!p1)
                {
                    // 第一个exp为null，上层报错，这里不用再管
                }
                else if (p1 && p1->typeName != ARRAY)
                {
                    //报错，非数组使用[]运算符
                    char msg[100] = {0};
                    sprintf(msg, "\"%s\" is not an array.", t->left_sons->type);
                    printError(NOT_A_ARRAY, t->lineno, msg);
                }
                else if (!p2 || p2->typeName != BASIC || strcmp(p2->UNION.TOKEN, "INT"))
                {
                    //报错，不用int索引[]
                    char msg[100] = {0};
                    sprintf(msg, "\"%s\" is not an integer.", t->siblings->siblings->left_sons->type);
                    printError(NOT_A_INT, t->lineno, msg);
                }
                else
                {
                    returnType = p1->UNION.ARRAY.elemType;
                }
                return returnType;
            }
            // Exp -> Exp DOT ID
            else
            {
                struct type *p1 = Exp(t);
                struct type *returnType = NULL;
                if (!p1 || p1->typeName != STRUCTURE)
                {
                    //报错，对非结构体使用.运算符
                    printError(ILLEGAL_USE_DOT, t->lineno, "Illegal use of \".\".");
                }
                else
                {
                    struct node *ref_id = t->siblings->siblings;
                    struct memberVariable *structVarList = p1->UNION.STRUCT.argv;
                    while (structVarList != NULL)
                    {
                        if (!strcmp(structVarList->name, ref_id->type))
                        {
                            break;
                        }
                        structVarList = structVarList->next;
                    }
                    if (structVarList == NULL)
                    {
                        //报错，没有可以匹配的域名
                        char msg[100] = {0};
                        sprintf(msg,"Non—existent field \"%s\"",ref_id->type);
                        printError(NONEXIST_FIELD, node->lineno, msg);
                    }
                    else
                    {
                        returnType = structVarList->memType;
                    }
                }
                // if (p1)
                //     deleteType(p1);
                return returnType;
            }
        }
    }
    //单目运算符
    // Exp -> MINUS Exp
    //      | NOT Exp
    else if (!strcmp(t->token, "MINUS") || !strcmp(t->token, "NOT"))
    {
        struct type *p1 = Exp(t->siblings);
        struct type *returnType = NULL;
        if (!p1 || p1->typeName != BASIC)
        {
            //报错，数组，结构体运算
            printError(TYPE_MISMATCH_OP, t->lineno, "TYPE_MISMATCH_OP");
        }
        else
        {
            returnType = p1;
        }
        return returnType;
    }
    else if (!strcmp(t->token, "LP"))
    {
        return Exp(t->siblings);
    }
    // Exp -> ID LP Args RP
    //		| ID LP RP
    else if (!strcmp(t->token, "ID") && t->siblings)
    {
        struct hashList *funcInfo = lookup(t->type, ALL);

        // function not find
        if (funcInfo == NULL)
        {
            char msg[100] = {0};
            sprintf(msg, "Undefined function \"%s\".", t->type);
            printError(UNDEF_FUNC, node->lineno, msg);
            return NULL;
        }
        else if (funcInfo->myType->typeName != FUNCTION)
        {
            char msg[100] = {0};
            sprintf(msg, "\"%s\" is not a function.", t->type);
            printError(NOT_A_FUNC, node->lineno, msg);
            return NULL;
        }
        // Exp -> ID LP Args RP
        else if (!strcmp(t->siblings->siblings->token, "Args"))
        {
            Args(t->siblings->siblings, funcInfo);
            return funcInfo->myType->UNION.FUN.returnType;
        }
        // Exp -> ID LP RP
        else
        {
            if (funcInfo->myType->UNION.FUN.argc != 0)
            {
                char msg[100] = {0};
                sprintf(msg, "too few arguments to function \"%s\", except %d args.", funcInfo->name, funcInfo->myType->UNION.FUN.argc);
                printError(FUNC_AGRC_MISMATCH, node->lineno, msg);
            }
            return funcInfo->myType->UNION.FUN.returnType;
        }
    }
    // Exp -> ID
    else if (!strcmp(t->token, "ID"))
    {
        struct hashList *tp = lookup(t->type, ALL);
        if (tp == NULL || tp->myType->typeName == FUNCTION)
        {
            char msg[100] = {0};
            sprintf(msg, "Undefined variable \"%s\".", t->type);
            printError(UNDEF_VAR, t->lineno, msg);
            return NULL;
        }
        else
        {
            return tp->myType;
        }
    }
    else
    {
        // Exp -> FLOAT
        if (!strcmp(t->token, "FLOAT"))
        {
            return createType(BASIC, "FLOAT");
        }
        // Exp -> INT
        else
        {
            return createType(BASIC, "INT");
        }
    }
}

/*
    Args -> Exp COMMA Args | Exp
    解析函数输入参数,类型是否与node中EXP返回类型和数目匹配
*/
void Args(struct node *node, struct hashList *funcInfo)
{
    // printf("Args\n");
    struct node *temp = node;
    struct memberVariable *arg = funcInfo->myType->UNION.FUN.argv;
    while (temp)
    {
        if (arg == NULL)
        {
            char msg[100] = {0};
            sprintf(msg, "too many arguments to function \"%s\", except %d args.", funcInfo->name, funcInfo->myType->UNION.FUN.argc);
            printError(FUNC_AGRC_MISMATCH, node->lineno, msg);
            break;
        }
        struct type *realType = Exp(temp->left_sons);
        if (!checkType(realType, arg->memType))
        {
            char msg[100] = {0};
            sprintf(msg, "Function \"%s\" is not applicable for arguments.", funcInfo->name);
            printError(FUNC_AGRC_MISMATCH, node->lineno, msg);
            return;
        }

        arg = arg->next;
        if (temp->left_sons->siblings)
        {
            temp = temp->left_sons->siblings->siblings;
        }
        else
        {
            break;
        }
    }
    if (arg != NULL)
    {
        char msg[100] = {0};
        sprintf(msg, "too few arguments to function \"%s\", except %d args.",
                funcInfo->name, funcInfo->myType->UNION.FUN.argc);
        printError(FUNC_AGRC_MISMATCH, node->lineno, msg);
    }
}

/*
    对语法分析树进行语义分析,输入树根节点,遍历找到ExtDef进行解析,返回出错次数
*/
int semanticAnalyse(struct node *root)
{
    if (root == NULL)
        return errnum;

    if (!strcmp(root->token, "ExtDef"))
        ExtDef(root);

    semanticAnalyse(root->left_sons);
    semanticAnalyse(root->siblings);
    return errnum;
}
