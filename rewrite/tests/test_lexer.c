#include <check.h>
#include <stdlib.h>

#include "../temple.h"


START_TEST (reset_str_buffer) {
    int j = 100;
    int str_start_size = 50;
    char *str_buf = (char *)malloc(sizeof(char) * str_start_size);
    str_buf[0] = 'h';
    reset_str_buf(&j, &str_start_size, str_buf);

    ck_assert_int_eq(j, 0);
    ck_assert_int_eq(str_start_size, 100);
    for (int i = 0; i < 50; i++)
        ck_assert_int_eq(str_buf[i], 0);
}
END_TEST

START_TEST (tokenize_all) {
    char *text = "This is a {% if test %} for template-engine\n {% else %} "
                 "this is a {{ variable }} test {% endblock %}";
    TokenNode tokens_cmp[18] = {
        { "This is a ", 11, TOKEN_CONTENT, 0},
        { 0, 0, TOKEN_BLOCK_OPEN, 0},
        { "if", 3, TOKEN_IDENTIFIER, 0},
        { "test", 5, TOKEN_IDENTIFIER, 0},
        { 0, 0, TOKEN_BLOCK_CLOSE, 0},
        { " for template-engine\n ", 32, TOKEN_CONTENT, 1},
        { 0, 0, TOKEN_BLOCK_OPEN, 1},
        { "else", 5, TOKEN_IDENTIFIER, 1},
        { 0, 0, TOKEN_BLOCK_CLOSE, 1},
        { " this is a ", 12, TOKEN_CONTENT, 1},
        { 0, 0, TOKEN_EXPR_OPEN, 1},
        { "variable", 9, TOKEN_IDENTIFIER, 1},
        { 0, 0, TOKEN_EXPR_CLOSE, 1},
        { " test ", 7, TOKEN_CONTENT, 1},
        { 0, 0, TOKEN_BLOCK_OPEN, 1},
        { "endblock", 9, TOKEN_IDENTIFIER, 1},
        { 0, 0, TOKEN_BLOCK_CLOSE, 1},
    };

    TokenNode tokens[1000];
    int token_count = tokenize(text, tokens);
    for (int i = 0; i < token_count; i++) {
        if (tokens[i].value) {
            ck_assert_str_eq(tokens[i].value, tokens_cmp[i].value);
        }
        ck_assert_int_eq(tokens[i].line_no, tokens_cmp[i].line_no);
        ck_assert_int_eq(tokens[i].type, tokens_cmp[i].type);
    }

    ck_assert_int_eq(token_count, 17);
}
END_TEST

START_TEST (basic_tokenize_check) {
    char *text = "This is a test";
    TokenNode tokens[1000];
    int token_count = tokenize(text, tokens);
    ck_assert_int_eq(token_count, 1);
}

START_TEST (tokenize_sanity_check) {
    char *text = "This is a {% if test %} for template-engine\n {% else %} "
                 "this is a {{ variable }} test {% endblock %}\nand if "
                 "this is a text then we will try {% extends 'base.html' %}";

    TokenNode tokens[1000];
    int token_count = tokenize(text, tokens);
    ck_assert_int_eq(token_count, 22);
}
END_TEST

int main(void)
{
    Suite *s1 = suite_create("Core");
    TCase *tc1_1 = tcase_create("Core");
    SRunner *sr = srunner_create(s1);
    int nf;

    suite_add_tcase(s1, tc1_1);
    tcase_add_test(tc1_1, reset_str_buffer);
    tcase_add_test(tc1_1, tokenize_all);
    tcase_add_test(tc1_1, basic_tokenize_check);
    tcase_add_test(tc1_1, tokenize_sanity_check);

    srunner_run_all(sr, CK_ENV);
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return nf == 0 ? 0 : 1;
}
