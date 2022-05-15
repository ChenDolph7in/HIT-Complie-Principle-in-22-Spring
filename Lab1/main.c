#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "syntax.tab.h"
#include "main.h"
extern int yyrestart();

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
            n->token = token;
            n->float_value = atof(yytext);
        }
        else if (!strcmp(token, "INT_HEX"))
        {
            n->token = "INT_HEX";
            n->integer_value = (int)strtol(yytext, NULL, 16);
        }
        else if (!strcmp(token, "INT_DEX"))
        {
            n->token = "INT_DEX";
            n->integer_value = atoi(yytext);
        }
        else if (!strcmp(token, "INT_OCT"))
        {
            n->token = "INT_OCT";
            n->integer_value = (int)strtol(yytext, NULL, 8);
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
        if (n->lineno != -1)
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
    return 0;
}

void yyerror(char *s)
{
    fprintf(stderr, "Error type B at line %d: syntax error near '%s'\n", yylineno, yytext);
}