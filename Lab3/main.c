#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "syntax.tab.h"
#include "interCode.h"
extern int yyrestart();

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

struct node *insertNode(char *token, unsigned int num, ...)
{
    struct node *n = (struct node *)malloc(sizeof(struct node));
    // printf("\ntoken:%s\n", token);
    va_list args;
    va_start(args, num);
    if (num > 0)
    {
        n->token = token;
        struct node *son_node, *last_node = n;
        for (int k = 0; k < num; k++)
        {
            son_node = va_arg(args, struct node *);
            if (k == 0)
            {
                n->lineno = son_node->lineno;
                n->left_sons = son_node;
                last_node = son_node;
            }
            else
            {
                last_node->siblings = son_node;
                last_node = son_node;
            }
            if (k == num - 1)
            {
                last_node->siblings = NULL;
            }
        }
    }
    else
    {
        int line = va_arg(args, int);
        n->lineno = line;
        n->left_sons = NULL;
        if ((!strcmp(token, "ID")) || (!strcmp(token, "TYPE")))
        {
            n->token = token;
            char *temp = (char *)malloc(sizeof(char) * 33);
            strcpy(temp, yytext);
            n->type = temp;
        }
        else if (!strcmp(token, "FLOAT"))
        {
            char *temp = (char *)malloc(sizeof(char) * 33);
            strcpy(temp, yytext);
            n->type = temp;
            n->token = token;
            n->float_value = atof(yytext);
        }
        else if (!strcmp(token, "INT_HEX"))
        {
            char *temp = (char *)malloc(sizeof(char) * 33);
            strcpy(temp, yytext);
            n->type = temp;
            n->token = "INT_HEX";
            n->integer_value = (int)strtol(yytext, NULL, 16);
        }
        else if (!strcmp(token, "INT_DEX"))
        {
            char *temp = (char *)malloc(sizeof(char) * 33);
            strcpy(temp, yytext);
            n->type = temp;
            n->token = "INT_DEX";
            n->integer_value = atoi(yytext);
        }
        else if (!strcmp(token, "INT_OCT"))
        {
            char *temp = (char *)malloc(sizeof(char) * 33);
            strcpy(temp, yytext);
            n->type = temp;
            n->token = "INT_OCT";
            n->integer_value = (int)strtol(yytext, NULL, 8);
        }
        else if (!strcmp(token, "RELOP"))
        {
            char *temp = (char *)malloc(sizeof(char) * 33);
            strcpy(temp, yytext);
            n->type = temp;
            n->token = token;
        }
        else
        {
            n->token = token;
        }
    }
    return n;
}

void preorderTraversal(struct node *n, int i)
{
    if (n != NULL)
    {
        if (n->lineno != -2)
        {
            for (int k = 0; k < i; k++)
            {
                printf("  ");
            }
            if (!strcmp(n->token, "ID") || !strcmp(n->token, "TYPE"))
            {
                printf("%s: %s\n", n->token, n->type);
            }
            else if (!strcmp(n->token, "FLOAT"))
            {
                printf("%s: %f\n", n->token, n->float_value);
            }
            else if (!strcmp(n->token, "INT_OCT"))
            {
                printf("INT: %d\n", n->integer_value);
            }
            else if (!strcmp(n->token, "INT_DEX"))
            {
                printf("INT: %d\n", n->integer_value);
            }
            else if (!strcmp(n->token, "INT_HEX"))
            {
                printf("INT: %d\n", n->integer_value);
            }
            else if (n->left_sons != NULL)
            {
                printf("%s (%d)\n", n->token, n->lineno);
            }
            else
            {
                printf("%s\n", n->token);
            }
        }
        preorderTraversal(n->left_sons, i + 1);
        preorderTraversal(n->siblings, i);
    }
}

int main(int argc, char **argv)
{
    if (argc <= 1)
        return 1;
    FILE *f = fopen(argv[1], "r");
    if (!f)
    {
        perror(argv[1]);
        return 1;
    }
    yyrestart(f);
    yyparse();
    if (error == 0)
    {
        hashTable = initHashTable();
        //向符号表中插入write()和read()
        {
            struct hashList *writeNode = (struct hashList *)malloc(sizeof(struct hashList));
            struct hashList *readNode = (struct hashList *)malloc(sizeof(struct hashList));
            struct memberVariable *argv = (struct memberVariable *)malloc(sizeof(struct memberVariable));
            struct hashList *arg = (struct hashList *)malloc(sizeof(struct hashList));
            argv->name = "outputNum";
            argv->memType = createType(BASIC, "INT");
            argv->next = NULL;

            arg->name = argv->name;
            arg->isArg = 1;
            arg->myType = argv->memType;
            arg->next = NULL;
            insertIntoHashTable(arg);

            writeNode->name = "write";
            writeNode->myType = createType(FUNCTION, 1, argv, createType(BASIC, "INT"));
            writeNode->next = NULL;
            writeNode->isArg = 0;

            readNode->name = "read";
            readNode->myType = createType(FUNCTION, 0, NULL, createType(BASIC, "INT"));
            readNode->next = NULL;
            readNode->isArg = 0;
            insertIntoHashTable(readNode);
            insertIntoHashTable(writeNode);
            // printHashTable();
        }

        if (!semanticAnalyse(root))
        {
            // preorderTraversal(root, 0);
            initInterCodeList();
            createInterCodeList(root);

            if (argc <= 2)
            {
                printInterCodeList(NULL);
            }
            else
            {
                printInterCodeList(argv[2]);
            }
        }
    }
    return 0;
}

void yyerror(char *s)
{
    fprintf(stderr, "Error type B at line %d: syntax error near '%s'\n", yylineno, yytext);
}