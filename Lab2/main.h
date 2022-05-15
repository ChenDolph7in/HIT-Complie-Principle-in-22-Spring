extern int yylineno;
extern char *yytext;
extern int error;
extern struct node *root;
void yyerror(char *s);

struct node
{
    char *token;
    int lineno;
    struct node *siblings;
    struct node *left_sons;
    char *type;
    union
    {

        int integer_value;
        float float_value;
    };
};

struct node *insertNode(char *token, unsigned int num, ...);

void preorderTraversal(struct node *n, int i);