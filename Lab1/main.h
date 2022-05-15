extern int yylineno;
extern char *yytext;
void yyerror(char *s);

struct node
{
    char *token;
    int lineno;
    struct node *siblings;
    struct node *left_sons;
    union
    {
        char *type;
        int integer_value;
        float float_value;
    };
};

struct node *insertNode(char *token, unsigned int num, ...);

void preorderTraversal(struct node *n, int i);