#include "interCode.h"

/* 初始化线型中间代码双向循环链表以及初始化其他全局变量 */
void initInterCodeList()
{
    currentLabel = 1;
    currentTemp = 1;
    list = (InterCodeList)malloc(sizeof(struct InterCodeList_));
    list->head = NULL;
    list->tail = NULL;
}

/* 向线型IR中插入新的中间代码行节点 */
void insertInterCode(InterCodes node)
{
    if (list->head == NULL)
    {
        list->head = node;
        list->tail = node;
    }
    else
    {
        node->prev = list->tail;
        node->next = NULL;
        list->tail->next = node;
        list->tail = node;
    }
}

/* 创建新的参数列表 */
ArgList createArgList()
{
    ArgList argv = (ArgList)malloc(sizeof(struct ArgList_));
    argv->head = NULL;
    argv->tail = NULL;
    return argv;
}

/* 创建新的参数列表结点 */
Arg createArg(Operand op)
{
    Arg arg = (Arg)malloc(sizeof(struct Arg_));
    arg->next = NULL;
    arg->op = op;
    return arg;
}

/* 根据实验一的分析树和实验二的全局的符号表生成中间代码，并保存在全局list中 */
void createInterCodeList(struct node *root)
{
    if (root == NULL)
        return;
    if (!strcmp(root->token, "ExtDefList"))
    {
        translate_ExtDefList(root);
    }
    else
    {
        createInterCodeList(root->left_sons);
        createInterCodeList(root->siblings);
    }
}

/* 将list中的中间代码逐行打印到指定文件中，若filename为空，则打印到stdout*/
void printInterCodeList(char *filename)
{
    if (filename)
    {
        FILE *fp = fopen(filename, "w");
        InterCodes cur = list->head;
        while (cur)
        {
            switch (cur->code->kind)
            {
            case IR_LABEL:
                fprintf(fp, "LABEL ");
                printOp(fp, cur->code->u.uniop.op);
                fprintf(fp, " :");
                break;
            case IR_FUNCTION:
                fprintf(fp, "FUNCTION ");
                printOp(fp, cur->code->u.uniop.op);
                fprintf(fp, " :");
                break;
            case IR_ASSIGN:
                printOp(fp, cur->code->u.assign.left);
                fprintf(fp, " := ");
                printOp(fp, cur->code->u.assign.right);
                break;
            case IR_ADD:
                printOp(fp, cur->code->u.binop.result);
                fprintf(fp, " := ");
                printOp(fp, cur->code->u.binop.op1);
                fprintf(fp, " + ");
                printOp(fp, cur->code->u.binop.op2);
                break;
            case IR_SUB:
                printOp(fp, cur->code->u.binop.result);
                fprintf(fp, " := ");
                printOp(fp, cur->code->u.binop.op1);
                fprintf(fp, " - ");
                printOp(fp, cur->code->u.binop.op2);
                break;
            case IR_MUL:
                printOp(fp, cur->code->u.binop.result);
                fprintf(fp, " := ");
                printOp(fp, cur->code->u.binop.op1);
                fprintf(fp, " * ");
                printOp(fp, cur->code->u.binop.op2);
                break;
            case IR_DIV:
                printOp(fp, cur->code->u.binop.result);
                fprintf(fp, " := ");
                printOp(fp, cur->code->u.binop.op1);
                fprintf(fp, " / ");
                printOp(fp, cur->code->u.binop.op2);
                break;
            case IR_GET_ADDR:
                printOp(fp, cur->code->u.assign.left);
                fprintf(fp, " := &");
                printOp(fp, cur->code->u.assign.right);
                break;
            case IR_READ_ADDR:
                printOp(fp, cur->code->u.assign.left);
                fprintf(fp, " := *");
                printOp(fp, cur->code->u.assign.right);
                break;
            case IR_WRITE_ADDR:
                fprintf(fp, "*");
                printOp(fp, cur->code->u.assign.left);
                fprintf(fp, " := ");
                printOp(fp, cur->code->u.assign.right);
                break;
            case IR_GOTO:
                fprintf(fp, "GOTO ");
                printOp(fp, cur->code->u.uniop.op);
                break;
            case IR_IF_GOTO:
                fprintf(fp, "IF ");
                printOp(fp, cur->code->u.leap.x);
                fprintf(fp, " ");
                printOp(fp, cur->code->u.leap.relop);
                fprintf(fp, " ");
                printOp(fp, cur->code->u.leap.y);
                fprintf(fp, " GOTO ");
                printOp(fp, cur->code->u.leap.z);
                break;
            case IR_RETURN:
                fprintf(fp, "RETURN ");
                printOp(fp, cur->code->u.uniop.op);
                break;
            case IR_DEC:
                fprintf(fp, "DEC ");
                printOp(fp, cur->code->u.dec.op);
                fprintf(fp, " ");
                fprintf(fp, "%d", cur->code->u.dec.size);
                break;
            case IR_ARG:
                fprintf(fp, "ARG ");
                printOp(fp, cur->code->u.uniop.op);
                break;
            case IR_CALL:
                printOp(fp, cur->code->u.assign.left);
                fprintf(fp, " := CALL ");
                printOp(fp, cur->code->u.assign.right);
                break;
            case IR_PARAM:
                fprintf(fp, "PARAM ");
                printOp(fp, cur->code->u.uniop.op);
                break;
            case IR_READ:
                fprintf(fp, "READ ");
                printOp(fp, cur->code->u.uniop.op);
                break;
            case IR_WRITE:
                fprintf(fp, "WRITE ");
                printOp(fp, cur->code->u.uniop.op);
                break;
            }
            fprintf(fp, "\n");
            cur = cur->next;
        }

        fclose(fp);
    }
    else
    {
        FILE *fp = NULL;
        InterCodes cur = list->head;
        while (cur)
        {
            switch (cur->code->kind)
            {
            case IR_LABEL:
                printf("LABEL ");
                printOp(fp, cur->code->u.uniop.op);
                printf(" :");
                break;
            case IR_FUNCTION:
                printf("FUNCTION ");
                printOp(fp, cur->code->u.uniop.op);
                printf(" :");
                break;
            case IR_ASSIGN:
                printOp(fp, cur->code->u.assign.left);
                printf(" := ");
                printOp(fp, cur->code->u.assign.right);
                break;
            case IR_ADD:
                printOp(fp, cur->code->u.binop.result);
                printf(" := ");
                printOp(fp, cur->code->u.binop.op1);
                printf(" + ");
                printOp(fp, cur->code->u.binop.op2);
                break;
            case IR_SUB:
                printOp(fp, cur->code->u.binop.result);
                printf(" := ");
                printOp(fp, cur->code->u.binop.op1);
                printf(" - ");
                printOp(fp, cur->code->u.binop.op2);
                break;
            case IR_MUL:
                printOp(fp, cur->code->u.binop.result);
                printf(" := ");
                printOp(fp, cur->code->u.binop.op1);
                printf(" * ");
                printOp(fp, cur->code->u.binop.op2);
                break;
            case IR_DIV:
                printOp(fp, cur->code->u.binop.result);
                printf(" := ");
                printOp(fp, cur->code->u.binop.op1);
                printf(" / ");
                printOp(fp, cur->code->u.binop.op2);
                break;
            case IR_GET_ADDR:
                printOp(fp, cur->code->u.assign.left);
                printf(" := &");
                printOp(fp, cur->code->u.assign.right);
                break;
            case IR_READ_ADDR:
                printOp(fp, cur->code->u.assign.left);
                printf(" := *");
                printOp(fp, cur->code->u.assign.right);
                break;
            case IR_WRITE_ADDR:
                printf("*");
                printOp(fp, cur->code->u.assign.left);
                printf(" := ");
                printOp(fp, cur->code->u.assign.right);
                break;
            case IR_GOTO:
                printf("GOTO ");
                printOp(fp, cur->code->u.uniop.op);
                break;
            case IR_IF_GOTO:
                printf("IF ");
                printOp(fp, cur->code->u.leap.x);
                printf(" ");
                printOp(fp, cur->code->u.leap.relop);
                printf(" ");
                printOp(fp, cur->code->u.leap.y);
                printf(" GOTO ");
                printOp(fp, cur->code->u.leap.z);
                break;
            case IR_RETURN:
                printf("RETURN ");
                printOp(fp, cur->code->u.uniop.op);
                break;
            case IR_DEC:
                printf("DEC ");
                printOp(fp, cur->code->u.dec.op);
                printf(" ");
                printf("%d", cur->code->u.dec.size);
                break;
            case IR_ARG:
                printf("ARG ");
                printOp(fp, cur->code->u.uniop.op);
                break;
            case IR_CALL:
                printOp(fp, cur->code->u.assign.left);
                printf(" := CALL ");
                printOp(fp, cur->code->u.assign.right);
                break;
            case IR_PARAM:
                printf("PARAM ");
                printOp(fp, cur->code->u.uniop.op);
                break;
            case IR_READ:
                printf("READ ");
                printOp(fp, cur->code->u.uniop.op);
                break;
            case IR_WRITE:
                printf("WRITE ");
                printOp(fp, cur->code->u.uniop.op);
                break;
            }
            printf("\n");
            cur = cur->next;
        }
    }
}

/* 将运算符打印到文件操作符fp中，若为fp空，则输出到stdout */
void printOp(FILE *fp, Operand op)
{
    if (fp)
    {
        switch (op->kind)
        {
        case CONSTANT:
            fprintf(fp, "#%d", op->u.value);
            break;
        default:
            fprintf(fp, "%s", op->u.name);
            break;
        }
    }
    else
    {
        switch (op->kind)
        {
        case CONSTANT:
            printf("#%d", op->u.value);
            break;
        default:
            printf("%s", op->u.name);
            break;
        }
    }
}

/* 新建运算对象，根据kind读取边长参数初始化该对象 */
Operand createOperand(int kind, ...)
{
    Operand p = (Operand)malloc(sizeof(struct Operand_));

    p->kind = kind;
    va_list vaList;
    va_start(vaList, kind);
    switch (kind)
    {
    case CONSTANT:
        p->u.value = va_arg(vaList, int);
        break;
    default:
        p->u.name = va_arg(vaList, char *);
        break;
    }

    return p;
}

/* 新建中间代码行，根据kind读取边长参数初始化该对象 */
InterCode createInterCode(int kind, ...)
{
    InterCode p = (InterCode)malloc(sizeof(struct InterCode_));
    p->kind = kind;
    va_list vaList;
    switch (kind)
    {
    case IR_LABEL:
    case IR_FUNCTION:
    case IR_GOTO:
    case IR_RETURN:
    case IR_ARG:
    case IR_PARAM:
    case IR_READ:
    case IR_WRITE:
        va_start(vaList, kind);
        p->u.uniop.op = va_arg(vaList, Operand);
        break;
    case IR_ASSIGN:
    case IR_GET_ADDR:
    case IR_READ_ADDR:
    case IR_WRITE_ADDR:
    case IR_CALL:
        va_start(vaList, kind);
        p->u.assign.left = va_arg(vaList, Operand);
        p->u.assign.right = va_arg(vaList, Operand);
        break;
    case IR_ADD:
    case IR_SUB:
    case IR_MUL:
    case IR_DIV:
        va_start(vaList, kind);
        p->u.binop.result = va_arg(vaList, Operand);
        p->u.binop.op1 = va_arg(vaList, Operand);
        p->u.binop.op2 = va_arg(vaList, Operand);
        break;
    case IR_DEC:
        va_start(vaList, kind);
        p->u.dec.op = va_arg(vaList, Operand);
        p->u.dec.size = va_arg(vaList, int);
        break;
    case IR_IF_GOTO:
        va_start(vaList, kind);
        p->u.leap.x = va_arg(vaList, Operand);
        p->u.leap.relop = va_arg(vaList, Operand);
        p->u.leap.y = va_arg(vaList, Operand);
        p->u.leap.z = va_arg(vaList, Operand);
    }
    return p;
}

/* 根据kind生成中间代码，并将在该过程中生产的中间代码行插入list */
void genInterCode(int kind, ...)
{
    va_list vaList;
    Operand temp = NULL;
    Operand result = NULL, op1 = NULL, op2 = NULL, relop = NULL;
    int size = 0;
    InterCodes newCode = NULL;
    assert(kind >= 0 && kind < 19);
    switch (kind)
    {
    case IR_LABEL:
    case IR_FUNCTION:
    case IR_GOTO:
    case IR_RETURN:
    case IR_ARG:
    case IR_PARAM:
    case IR_READ:
    case IR_WRITE:
        va_start(vaList, kind);
        op1 = va_arg(vaList, Operand);
        if (op1->kind == ADDRESS)
        {
            temp = createTemp();
            genInterCode(IR_READ_ADDR, temp, op1);
            op1 = temp;
        }

        newCode = createInterCodes(createInterCode(kind, op1));
        insertInterCode(newCode);
        // if (kind == IR_ARG)
        //     printf("add arg\n");
        break;
    case IR_ASSIGN:
    case IR_GET_ADDR:
    case IR_READ_ADDR:
    case IR_WRITE_ADDR:
    case IR_CALL:
        va_start(vaList, kind);
        op1 = va_arg(vaList, Operand);
        op2 = va_arg(vaList, Operand);
        if (kind == IR_ASSIGN && (op1->kind == ADDRESS || op2->kind == ADDRESS))
        {
            if (op1->kind == ADDRESS && op2->kind != ADDRESS)
            {
                genInterCode(IR_WRITE_ADDR, op1, op2);
            }
            else if (op2->kind == ADDRESS && op1->kind != ADDRESS)
            {
                genInterCode(IR_READ_ADDR, op1, op2);
            }
            else
            {
                temp = createTemp();
                genInterCode(IR_READ_ADDR, temp, op2);
                genInterCode(IR_WRITE_ADDR, op1, temp);
            }
        }
        else
        {
            newCode = createInterCodes(createInterCode(kind, op1, op2));
            insertInterCode(newCode);
        }
        break;
    case IR_ADD:
    case IR_SUB:
    case IR_MUL:
    case IR_DIV:
        va_start(vaList, kind);
        result = va_arg(vaList, Operand);
        op1 = va_arg(vaList, Operand);
        op2 = va_arg(vaList, Operand);

        if (op2->kind == ADDRESS && op2->kind == ADDRESS)
        {
            temp = createTemp();
            genInterCode(IR_READ_ADDR, temp, op1);
            op1 = temp;
            temp = createTemp();
            genInterCode(IR_READ_ADDR, temp, op2);
            op2 = temp;
        }

        newCode = createInterCodes(createInterCode(kind, result, op1, op2));
        insertInterCode(newCode);
        break;
    case IR_DEC:
        va_start(vaList, kind);
        op1 = va_arg(vaList, Operand);
        size = va_arg(vaList, int);
        newCode = createInterCodes(createInterCode(kind, op1, size));
        insertInterCode(newCode);
        break;
    case IR_IF_GOTO:
        va_start(vaList, kind);
        result = va_arg(vaList, Operand);
        relop = va_arg(vaList, Operand);
        op1 = va_arg(vaList, Operand);
        op2 = va_arg(vaList, Operand);
        newCode = createInterCodes(createInterCode(kind, result, relop, op1, op2));
        insertInterCode(newCode);
        break;
    }
}

/* 创建新的IR结点 */
InterCodes createInterCodes(InterCode code)
{
    InterCodes temp = (InterCodes)malloc(sizeof(struct InterCode_));
    temp->code = code;
    temp->next = NULL;
    temp->prev = NULL;
    return temp;
}

/* 根据下一个Temp标号新建Temp */
Operand createTemp()
{
    char *newTempName = (char *)malloc(33);
    bzero(newTempName, 33);
    sprintf(newTempName, "temp%d", currentTemp++);
    // printf("createNewTempName : %s\n",newTempName);
    Operand newTemp = createOperand(VARIABLE, newTempName);
    return newTemp;
}

/* 根据下一个Label标号新建Label */
Operand createLabel()
{
    char *newLabelName = (char *)malloc(33);
    bzero(newLabelName, 33);
    sprintf(newLabelName, "label%d", currentLabel++);
    Operand newLabel = createOperand(OP_LABEL, newLabelName);
    return newLabel;
}

// ExtDefList -> ExtDef ExtDefList
//             | e
void translate_ExtDefList(struct node *node)
{
    // printf("ExtDefList\n");
    while (node->left_sons)
    {
        translate_ExtDef(node->left_sons);
        node = node->left_sons->siblings;
    }
}

// ExtDef -> Specifier ExtDecList SEMI
//         | Specifier SEMI
//         | Specifier FunDec CompSt
void translate_ExtDef(struct node *node)
{
    // printf("ExtDef\n");
    if (!strcmp(node->left_sons->siblings->token, "FunDec"))
    {
        translate_FunDec(node->left_sons->siblings);
        translate_CompSt(node->left_sons->siblings->siblings);
    }
    else
    {
        // printf("%s\n", node->left_sons->siblings->token);
    }
}

// FunDec -> ID LP VarList RP
//         | ID LP RP
void translate_FunDec(struct node *node)
{
    // printf("FunDec\n");
    Operand op = createOperand(OP_FUNCTION, node->left_sons->type);
    genInterCode(IR_FUNCTION, op);

    struct hashList *funcNode = lookup(node->left_sons->type, FUNCTION);
    struct memberVariable *argv = funcNode->myType->UNION.FUN.argv;
    while (argv)
    {
        genInterCode(IR_PARAM, createOperand(VARIABLE, argv->name));
        argv = argv->next;
    }
}

// CompSt -> LC DefList StmtList RC
void translate_CompSt(struct node *node)
{
    // printf("CompSt\n");
    if (!strcmp(node->left_sons->siblings->token, "DefList"))
    {
        translate_DefList(node->left_sons->siblings);
    }
    if (!strcmp(node->left_sons->siblings->siblings->token, "StmtList"))
    {
        translate_StmtList(node->left_sons->siblings->siblings);
    }
}

// DefList -> Def DefList
//          | e
void translate_DefList(struct node *node)
{
    // printf("DefList\n");
    while (node->left_sons)
    {
        translate_Def(node->left_sons);
        node = node->left_sons->siblings;
    }
}

// Def -> Specifier DecList SEMI
void translate_Def(struct node *node)
{
    // printf("Def\n");
    translate_DecList(node->left_sons->siblings);
}

// DecList -> Dec
//          | Dec COMMA DecList
void translate_DecList(struct node *node)
{
    // printf("DecList\n");
    struct node *temp = node;
    while (temp)
    {
        translate_Dec(temp->left_sons);
        if (temp->left_sons->siblings)
            temp = temp->left_sons->siblings->siblings;
        else
            break;
    }
}

// Dec -> VarDec
//      | VarDec ASSIGNOP Exp
void translate_Dec(struct node *node)
{
    // printf("Dec\n");
    //  Dec -> VarDec
    if (node->left_sons->siblings == NULL)
    {
        translate_VarDec(node->left_sons, NULL);
    }
    // Dec -> VarDec ASSIGNOP Exp
    else
    {
        Operand t1 = createTemp();
        translate_VarDec(node->left_sons, t1);
        Operand t2 = createTemp();
        translate_Exp(node->left_sons->siblings->siblings, t2);
        genInterCode(IR_ASSIGN, t1, t2);
    }
}

// Exp -> Exp ASSIGNOP Exp
//      | Exp AND Exp
//      | Exp OR Exp
//      | Exp RELOP Exp
//      | Exp PLUS Exp
//      | Exp MINUS Exp
//      | Exp STAR Exp
//      | Exp DIV Exp
//      | MINUS Exp
//      | NOT Exp
//      | ID LP Args RP
//      | ID LP RP
//      | Exp LB Exp RB
//      | Exp DOT ID
//      | ID
//      | INT
//      | FLOAT
void translate_Exp(struct node *node, Operand place)
{
    // printf("Exp\n");
    //  Exp -> LP Exp RP
    if (!strcmp(node->left_sons->token, "LP"))
    {
        // printf("Exp -> LP Exp RP\n");
        translate_Exp(node->left_sons->siblings, place);
    }

    else if (!strcmp(node->left_sons->token, "Exp") ||
             !strcmp(node->left_sons->token, "NOT"))
    {
        // 条件表达式 和 基本表达式
        if (strcmp(node->left_sons->siblings->token, "LB") &&
            strcmp(node->left_sons->siblings->token, "DOT"))
        {
            // Exp -> Exp AND Exp
            //      | Exp OR Exp
            //      | Exp RELOP Exp
            //      | NOT Exp
            if (!strcmp(node->left_sons->siblings->token, "AND") ||
                !strcmp(node->left_sons->siblings->token, "OR") ||
                !strcmp(node->left_sons->siblings->token, "RELOP") ||
                !strcmp(node->left_sons->token, "NOT"))
            {
                // printf("Exp -> Exp AND OR RELOP NOT Exp\n");
                Operand label1 = createLabel();
                Operand label2 = createLabel();
                Operand true_num = createOperand(CONSTANT, 1);
                Operand false_num = createOperand(CONSTANT, 0);
                genInterCode(IR_ASSIGN, place, false_num);
                translate_Cond(node, label1, label2);
                genInterCode(IR_LABEL, label1);
                genInterCode(IR_ASSIGN, place, true_num);
            }
            else
            {
                // Exp -> Exp ASSIGNOP Exp
                if (!strcmp(node->left_sons->siblings->token, "ASSIGNOP"))
                {
                    // printf("Exp -> Exp ASSIGNOP Exp\n");
                    Operand t2 = createTemp();
                    translate_Exp(node->left_sons->siblings->siblings, t2);
                    Operand t1 = createTemp();
                    translate_Exp(node->left_sons, t1);
                    genInterCode(IR_ASSIGN, t1, t2);
                }
                else
                {
                    Operand t1 = createTemp();
                    translate_Exp(node->left_sons, t1);
                    Operand t2 = createTemp();
                    translate_Exp(node->left_sons->siblings->siblings, t2);
                    // Exp -> Exp PLUS Exp
                    if (!strcmp(node->left_sons->siblings->token, "PLUS"))
                    {
                        // printf("Exp -> Exp PLUS Exp\n");
                        genInterCode(IR_ADD, place, t1, t2);
                    }
                    // Exp -> Exp MINUS Exp
                    else if (!strcmp(node->left_sons->siblings->token, "MINUS"))
                    {
                        // printf("Exp -> Exp MINUS Exp\n");
                        genInterCode(IR_SUB, place, t1, t2);
                    }
                    // Exp -> Exp STAR Exp
                    else if (!strcmp(node->left_sons->siblings->token, "STAR"))
                    {
                        // printf("Exp -> Exp STAR Exp\n");
                        genInterCode(IR_MUL, place, t1, t2);
                    }
                    // Exp -> Exp DIV Exp
                    else if (!strcmp(node->left_sons->siblings->token, "DIV"))
                    {
                        // printf("Exp -> Exp DIV Exp\n");
                        genInterCode(IR_DIV, place, t1, t2);
                    }
                }
            }
        }
        // 数组和结构体访问
        else
        {
            // Exp -> Exp LB Exp RB
            if (!strcmp(node->left_sons->siblings->token, "LB"))
            {
                // printf("Exp -> Exp LB Exp RB\n");
                if (node->left_sons->left_sons->siblings && !strcmp(node->left_sons->left_sons->siblings->token, "LB"))
                {
                    printf("Cannot translate: Code contains variables of multi-dimensional array type or parameters of array type.\n");
                    return;
                }
                else
                {
                    Operand idx = createTemp();
                    translate_Exp(node->left_sons->siblings->siblings, idx);
                    Operand base = createTemp();
                    translate_Exp(node->left_sons, base);
                    Operand width;
                    Operand offset = createTemp();
                    Operand target;
                    struct hashList *item = lookup(base->u.name, ARRAY);
                    width = createOperand(CONSTANT, getSize(item->myType->UNION.ARRAY.elemType));
                    genInterCode(IR_MUL, offset, idx, width);

                    if (base->kind == VARIABLE)
                    {
                        // printf("非结构体数组访问\n");
                        target = createTemp();
                        genInterCode(IR_GET_ADDR, target, base);
                    }
                    else
                    {
                        // printf("结构体数组访问\n");
                        target = base;
                    }
                    genInterCode(IR_ADD, place, target, offset);
                    place->kind = ADDRESS;
                    lastArrayName = base->u.name;
                }
            }
            // Exp -> Exp DOT ID
            else
            {
                // printf("Exp -> Exp DOT ID\n");

                Operand temp = createTemp();
                translate_Exp(node->left_sons, temp);
                Operand target;
                if (temp->kind == ADDRESS)
                {
                    target = createOperand(temp->kind, temp->u.name);
                }
                else
                {
                    target = createTemp();
                    genInterCode(IR_GET_ADDR, target, temp);
                }
                Operand id = createTemp();
                int offset = 0;
                struct hashList *item = lookup(temp->u.name, STRUCTURE);
                //结构体数组，temp是临时变量，查不到表，需要用处理数组时候记录下的数组名老查表
                if (item == NULL)
                {
                    item = lookup(lastArrayName, ARRAY);
                }
                struct memberVariable *tmp;
                // 结构体数组
                if (item->myType->typeName == ARRAY)
                {
                    tmp = item->myType->UNION.ARRAY.elemType->UNION.STRUCT.argv;
                }
                // 一般结构体
                else
                {
                    tmp = item->myType->UNION.STRUCT.argv;
                }
                while (tmp)
                {
                    if (!strcmp(tmp->name, node->left_sons->siblings->siblings->type))
                        break;
                    offset += getSize(tmp->memType);
                    tmp = tmp->next;
                }
                Operand tOffset = createOperand(CONSTANT, offset);
                if (place)
                {
                    genInterCode(IR_ADD, place, target, tOffset);
                    place->kind = ADDRESS;
                    place->u.name = id->u.name;
                }
            }
        }
    }
    //单目运算符
    // Exp -> MINUS Exp
    else if (!strcmp(node->left_sons->token, "MINUS"))
    {
        // printf("Exp -> MINUS Exp\n");
        Operand t1 = createTemp();
        translate_Exp(node->left_sons->siblings, t1);
        Operand zero = createOperand(CONSTANT, 0);
        genInterCode(IR_SUB, place, zero, t1);
    }
    // Exp -> ID LP Args RP
    //		| ID LP RP
    else if (!strcmp(node->left_sons->token, "ID") && node->left_sons->siblings)
    {
        Operand funcTemp =
            createOperand(OP_FUNCTION, node->left_sons->type);
        // Exp -> ID LP Args RP
        if (!strcmp(node->left_sons->siblings->siblings->token, "Args"))
        {
            // printf("Exp -> ID LP Args RP\n");
            ArgList argList = createArgList();
            translate_Args(node->left_sons->siblings->siblings, argList);
            if (!strcmp(node->left_sons->type, "write"))
            {
                genInterCode(IR_WRITE, argList->head->op);
            }
            else
            {
                Arg argTemp = argList->head;
                while (argTemp)
                {
                    // printf("if_arg3 : %s\n", argTemp->op->u.name);
                    if (argTemp->op->kind == VARIABLE)
                    {
                        // printf("if_arg2 : %s\n", argTemp->op->u.name);
                        struct hashList *item = lookup(argTemp->op->u.name, NOT_FUNCTION);

                        // 结构体作为参数需要传址
                        if (item && item->myType->typeName == STRUCTURE)
                        {
                            Operand varTemp = createTemp();
                            genInterCode(IR_GET_ADDR, varTemp, argTemp->op);
                            Operand varTempCopy = createOperand(ADDRESS, varTemp->u.name);
                            genInterCode(IR_ARG, varTempCopy);
                        }
                        // 一般参数直接传值
                        else
                        {
                            // printf("if_arg4 : %s\n", argTemp->op->u.name);
                            genInterCode(IR_ARG, argTemp->op);
                        }
                    }
                    else
                    {
                        // printf("if_arg5 : %s\n", argTemp->op->u.name);
                        genInterCode(IR_ARG, argTemp->op);
                    }

                    argTemp = argTemp->next;
                }
                if (place)
                {
                    genInterCode(IR_CALL, place, funcTemp);
                }
                else
                {
                    Operand temp = createTemp();
                    genInterCode(IR_CALL, temp, funcTemp);
                }
            }
        }
        // Exp -> ID LP RP
        else
        {
            // printf("Exp -> ID LP RP\n");
            if (!strcmp(node->left_sons->type, "read"))
            {
                genInterCode(IR_READ, place);
            }
            else
            {
                if (place)
                {
                    genInterCode(IR_CALL, place, funcTemp);
                }
                else
                {
                    Operand temp = createTemp();
                    genInterCode(IR_CALL, temp, funcTemp);
                }
            }
        }
    }
    // Exp -> ID
    else if (!strcmp(node->left_sons->token, "ID"))
    {
        // printf("Exp -> ID\n");
        struct hashList *item = lookup(node->left_sons->type, ALL);
        // 结构体参数传地址
        currentTemp--;
        if (item->isArg && item->myType->typeName == STRUCTURE)
        {
            place->kind = ADDRESS;
            place->u.name = node->left_sons->type;
        }
        // 非结构体参数情况都当做变量处理
        else
        {
            place->kind = VARIABLE;
            place->u.name = node->left_sons->type;
        }
    }
    else
    {
        // Exp -> INT
        currentTemp--;
        place->kind = CONSTANT;
        place->u.value = node->left_sons->integer_value;
    }
}

// VarDec -> ID
//         | VarDec LB INT RB
void translate_VarDec(struct node *node, Operand place)
{
    // printf("VarDec\n");
    if (!strcmp(node->left_sons->token, "ID"))
    {
        struct hashList *temp = lookup(node->left_sons->type, NOT_FUNCTION);
        struct type *type = temp->myType;
        if (type->typeName == BASIC)
        {
            if (place)
            {
                currentTemp--;
                place->kind = VARIABLE;
                place->u.name = temp->name;
            }
        }
        else if (type->typeName == ARRAY)
        {
            if (type->UNION.ARRAY.elemType->typeName == ARRAY)
            {
                printf("Cannot translate: Code contains variables of multi-dimensional array type or parameters of array type.\n");
                return;
            }
            else
            {
                genInterCode(IR_DEC, createOperand(VARIABLE, temp->name), getSize(type));
            }
        }
        else if (type->typeName == STRUCTURE)
        {
            genInterCode(IR_DEC, createOperand(VARIABLE, temp->name), getSize(type));
        }
    }
    else
    {
        translate_VarDec(node->left_sons, place);
    }
}

// Args -> Exp COMMA Args
//       | Exp
void translate_Args(struct node *node, ArgList argList)
{
    // printf("Arg\n");
    // Args -> Exp
    Arg temp = createArg(createTemp());
    translate_Exp(node->left_sons, temp->op);
    if (temp->op->kind == VARIABLE)
    {
        struct hashList *item = lookup(temp->op->u.name, NOT_FUNCTION);
        if (item && item->myType->typeName == ARRAY)
        {
            printf("Cannot translate: Code contains variables of multi-dimensional array type or parameters of array type.\n");
            return;
        }
    }
    if (argList->head == NULL)
    {
        argList->head = temp;
        argList->tail = temp;
    }
    else
    {
        argList->tail->next = temp;
        argList->tail = temp;
    }
    // Args -> Exp COMMA Args
    if (node->left_sons->siblings != NULL)
    {
        translate_Args(node->left_sons->siblings->siblings, argList);
    }
}

// Exp -> Exp AND Exp
//      | Exp OR Exp
//      | Exp RELOP Exp
//      | NOT Exp
void translate_Cond(struct node *node, Operand labelTrue, Operand labelFalse)
{
    // printf("Cond\n");
    //  Exp -> NOT Exp
    if (!strcmp(node->left_sons->token, "NOT"))
    {
        translate_Cond(node->left_sons->siblings, labelFalse, labelTrue);
    }
    // Exp -> Exp RELOP Exp
    else if (!strcmp(node->left_sons->siblings->token, "RELOP"))
    {
        Operand t1 = createTemp();
        Operand t2 = createTemp();
        translate_Exp(node->left_sons, t1);
        translate_Exp(node->left_sons->siblings->siblings, t2);

        Operand relop = createOperand(OP_RELOP, node->left_sons->siblings->type);

        if (t1->kind == ADDRESS)
        {
            Operand temp = createTemp();
            genInterCode(IR_READ_ADDR, temp, t1);
            t1 = temp;
        }
        if (t2->kind == ADDRESS)
        {
            Operand temp = createTemp();
            genInterCode(IR_READ_ADDR, temp, t2);
            t2 = temp;
        }

        genInterCode(IR_IF_GOTO, t1, relop, t2, labelTrue);
        genInterCode(IR_GOTO, labelFalse);
    }
    // Exp -> Exp AND Exp
    else if (!strcmp(node->left_sons->siblings->token, "AND"))
    {
        Operand label1 = createLabel();
        translate_Cond(node->left_sons, label1, labelFalse);
        genInterCode(IR_LABEL, label1);
        translate_Cond(node->left_sons->siblings->siblings, labelTrue, labelFalse);
    }
    // Exp -> Exp OR Exp
    else if (!strcmp(node->left_sons->siblings->token, "OR"))
    {
        Operand label1 = createLabel();
        translate_Cond(node->left_sons, labelTrue, label1);
        genInterCode(IR_LABEL, label1);
        translate_Cond(node->left_sons->siblings->siblings, labelTrue, labelFalse);
    }
    // other cases
    else
    {
        Operand t1 = createTemp();
        translate_Exp(node, t1);
        Operand t2 = createOperand(CONSTANT, 0);
        Operand relop = createOperand(OP_RELOP, "!=");

        if (t1->kind == ADDRESS)
        {
            Operand temp = createTemp();
            genInterCode(IR_READ_ADDR, temp, t1);
            t1 = temp;
        }
        genInterCode(IR_IF_GOTO, t1, relop, t2, labelTrue);
        genInterCode(IR_GOTO, labelFalse);
    }
}

// Stmt -> Exp SEMI
//       | CompSt
//       | RETURN Exp SEMI
//       | IF LP Exp RP Stmt
//       | IF LP Exp RP Stmt ELSE Stmt
//       | WHILE LP Exp RP Stmt
void translate_Stmt(struct node *node)
{
    // printf("Stmt\n");
    //  Stmt -> Exp SEMI
    if (!strcmp(node->left_sons->token, "Exp"))
    {
        translate_Exp(node->left_sons, NULL);
    }
    // Stmt -> CompSt
    else if (!strcmp(node->left_sons->token, "CompSt"))
    {
        translate_CompSt(node->left_sons);
    }
    // Stmt -> RETURN Exp SEMI
    else if (!strcmp(node->left_sons->token, "RETURN"))
    {
        Operand t1 = createTemp();
        translate_Exp(node->left_sons->siblings, t1);
        genInterCode(IR_RETURN, t1);
    }
    // Stmt -> IF LP Exp RP Stmt
    else if (!strcmp(node->left_sons->token, "IF"))
    {
        struct node *exp = node->left_sons->siblings->siblings;
        struct node *stmt = exp->siblings->siblings;
        Operand label1 = createLabel();
        Operand label2 = createLabel();

        translate_Cond(exp, label1, label2);
        genInterCode(IR_LABEL, label1);
        translate_Stmt(stmt);
        if (stmt->siblings == NULL)
        {
            genInterCode(IR_LABEL, label2);
        }
        // Stmt -> IF LP Exp RP Stmt ELSE Stmt
        else
        {
            Operand label3 = createLabel();
            genInterCode(IR_GOTO, label3);
            genInterCode(IR_LABEL, label2);
            translate_Stmt(stmt->siblings->siblings);
            genInterCode(IR_LABEL, label3);
        }
    }
    // Stmt -> WHILE LP Exp RP Stmt
    else if (!strcmp(node->left_sons->token, "WHILE"))
    {
        Operand label1 = createLabel();
        Operand label2 = createLabel();
        Operand label3 = createLabel();

        genInterCode(IR_LABEL, label1);
        translate_Cond(node->left_sons->siblings->siblings, label2, label3);
        genInterCode(IR_LABEL, label2);
        translate_Stmt(node->left_sons->siblings->siblings->siblings->siblings);
        genInterCode(IR_GOTO, label1);
        genInterCode(IR_LABEL, label3);
    }
}

// StmtList -> Stmt StmtList
//           | e
void translate_StmtList(struct node *node)
{
    // printf("StmtList\n");
    while (node->left_sons)
    {
        translate_Stmt(node->left_sons);
        node = node->left_sons->siblings;
    }
}

int getSize(struct type *type)
{
    // printf("getSize\n");
    if (type == NULL)
        return 0;
    else if (type->typeName == BASIC)
        return 4;
    else if (type->typeName == ARRAY)
        return type->UNION.ARRAY.size * getSize(type->UNION.ARRAY.elemType);
    else if (type->typeName == STRUCTURE)
    {
        int size = 0;
        struct memberVariable *argv = type->UNION.STRUCT.argv;
        while (argv)
        {
            size += getSize(argv->memType);
            argv = argv->next;
        }
        return size;
    }
    return 0;
}