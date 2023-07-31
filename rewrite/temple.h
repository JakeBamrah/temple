#include <stdint.h>


enum Expression {
    TOKEN_BLOCK_OPEN,
    TOKEN_BLOCK_CLOSE,
    TOKEN_IDENTIFIER,
    TOKEN_DATA,
    TOKEN_DEFAULT
};

typedef struct {
    char *value;
    enum Expression type;
    uint16_t line_no;
} Token;

void print_token(Token *token);
const char *print_expr(enum Expression expr);

int tokenize(const char *text, Token *tokens);
void reset_str_buf(int *j, int *str_start_size, char *str_buf);
