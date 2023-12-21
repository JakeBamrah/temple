/*
 * Temple syntax identifiers are found while traversing an input string and are
 * tracked using a stack. Identifiers are always enclosed using {{ or {%
 * syntax. Content within these brackets are treated as IDENTIFIER tokens and
 * content found outside these brackets are treated as DATA tokens.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libh.h"
#include "temple.h"


#define INITIAL_BUF_SIZE 100

const char *print_expr(enum Token expr) {
    switch (expr) {
    case TOKEN_EXPR_OPEN:
        return "OPEN EXPRESSION TOKEN";
    case TOKEN_EXPR_CLOSE:
        return "CLOSE EXPRESSION TOKEN";
    case TOKEN_BLOCK_OPEN:
        return "OPEN BLOCK TOKEN";
    case TOKEN_BLOCK_CLOSE:
        return "CLOSE BLOCK TOKEN";
    case TOKEN_IDENTIFIER:
        return "IDENTIFIER TOKEN";
    case TOKEN_CONTENT:
        return "CONTENT TOKEN";
    default:
        return "DEFAULT";
    }
}

void print_token(TokenNode *token) {
    printf("[TOKEN] line_no:%-4d type:%-21s value:%s value length:%d\n",
            token->line_no, print_expr(token->type), token->value, token->value_len);
}

void reset_str_buf(int *j, int *str_start_size, char *str_buf) {
    *j = 0;
    memset(str_buf, 0, *str_start_size);
    *str_start_size = INITIAL_BUF_SIZE;
}

int tokenize(const char *text, TokenNode *tokens) {
    // line and token trackers
    int token_count = 0;
    uint16_t line_no = 0;
    int len = strlen(text);
    stack s = stack_init();

    // str buffer vars
    int j = 0;
    int str_start_size = INITIAL_BUF_SIZE;
    char *str_buf = (char *)malloc(sizeof(char) * str_start_size);
    for (int i = 0; i < len; i++) {
        enum Token state = stack_empty(&s) ? TOKEN_DEFAULT : stack_peek(&s);
        if (state == TOKEN_DEFAULT) {
            // build normal content string (outside tokens)
            while (i < len) {
                if (i < len - 1 && text[i] == '{'
                    && (text[i + 1] == '{' || text[i + 1] == '%')) {
                    break;
                }

                if (text[i] == '\n') {
                    line_no++;
                }

                // expand buffer if chance of overflow
                if (j > str_start_size) {
                    str_start_size = str_start_size * 2;
                    str_buf = (char *)realloc(str_buf, sizeof(char) * str_start_size);
                }
                str_buf[j++] = text[i++];
            }

            if (j > 0) { // end of content string (create content token)
                str_buf[j++] = '\0';
                TokenNode t = {"", j, TOKEN_CONTENT, line_no};
                t.value = (char *)malloc(sizeof(char) * str_start_size);
                memcpy(t.value, str_buf, sizeof(char) * str_start_size);
                tokens[token_count++] = t;

                reset_str_buf(&j, &str_start_size, str_buf);
            }

            if (i < len - 1) { // open token found (not EOF)
                enum Token expr = text[i + 1] == '{' ? TOKEN_EXPR_OPEN : TOKEN_BLOCK_OPEN;
                tokens[token_count++] = (TokenNode){0, 0, expr, line_no};
                stack_push(&s, expr);
            }
        } else if (state == TOKEN_BLOCK_OPEN || state == TOKEN_EXPR_OPEN) {
            // find identifier token contents (if, var, etc.)
            // identifiers *should* be on one line so line_no remains constant
            while (i < len) {
                if (i < len - 1 && (text[i] == '}' || text[i] == '%')
                    && text[i + 1] == '}') {
                    break;
                }

                if (j > str_start_size) {
                    str_start_size = str_start_size * 2;
                    str_buf = (char *)realloc(str_buf, sizeof(char) * str_start_size);
                }

                // accumulate identifier tokens (splitting on spaces)
                if (text[i] != ' ') {
                    str_buf[j++] = text[i];
                } else if (text[i] == ' ' && j > 0) { // identifier token ended
                    str_buf[j++] = '\0';
                    TokenNode t = {"", j, TOKEN_IDENTIFIER, line_no};
                    t.value = (char *)malloc(sizeof(char) * str_start_size);
                    memcpy(t.value, str_buf, sizeof(char) * str_start_size);
                    tokens[token_count++] = t;

                    reset_str_buf(&j, &str_start_size, str_buf);
                }
                i++;
            }

            enum Token e = text[i] == '}' ? TOKEN_EXPR_CLOSE : TOKEN_BLOCK_CLOSE;
            tokens[token_count++] = (TokenNode){0, 0, e, line_no};
            stack_pop(&s);
        }
        i++;
    }

    return token_count;
}
