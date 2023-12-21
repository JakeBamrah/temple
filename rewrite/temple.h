#include <stdint.h>


/* --------------------------------- Lexer --------------------------------- */
enum Token {
    TOKEN_EXPR_OPEN,
    TOKEN_EXPR_CLOSE,
    TOKEN_BLOCK_OPEN,
    TOKEN_BLOCK_CLOSE,
    TOKEN_IDENTIFIER,
    TOKEN_CONTENT,
    TOKEN_DEFAULT
};

typedef struct {
    char *value;
    uint16_t value_len;
    enum Token type;
    uint16_t line_no;
} TokenNode;

void print_token(TokenNode *token);
const char *print_expr(enum Token expr);

int tokenize(const char *text, TokenNode *tokens);
void reset_str_buf(int *j, int *str_start_size, char *str_buf);

/* -------------------------------- Parser --------------------------------- */
enum ASTNodeType {
    AST_BLOCK_EXPRESSION,
    AST_EXPRESSION,
    AST_CONTENT,
    AST_TEMPLATE
};

enum Identifier {
    ID_IF,
    ID_ELSE,
    ID_INSERT,
    ID_EXTENDS,
    ID_ENDBLOCK,
    ID_CONTENT,
    ID_TEMPLATE,
    ID_VAR
};

struct ASTNode {
    enum ASTNodeType type;
    enum Identifier identifier;
    char *arguments[20];
    char *content;
    int children_len;
    int arguments_len;
    int has_inverse_node;
    struct ASTNode *children;
    struct ASTNode *inverse_node;
};

void print_ast_node(struct ASTNode *node);
const char *print_identifier(enum Identifier id);
const char *print_ast_type(enum ASTNodeType type);
void print_nary_tree(int level, struct ASTNode *root);

enum Identifier parse_identifier(TokenNode *t_node);
void parse_content(int *t_pos, TokenNode *t_node, struct ASTNode *node);
void parse_expression(int *t_pos, int t_count, TokenNode *t_nodes, struct ASTNode *node);
struct ASTNode * build_ast(int *t_pos, int t_count, TokenNode *t_nodes, struct ASTNode *root);

/* -------------------------------- Interpreter --------------------------------- */
void traverse(struct ASTNode *root, char *buff);
void visit_block_expression(struct ASTNode *root, char *buff);
