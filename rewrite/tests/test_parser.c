#include <check.h>
#include <stdlib.h>
#include <stdio.h>

#include "../temple.h"


START_TEST (parser_expression_token_check) {
    char *text = "{{ extends base.html }}";
    TokenNode tokens[1000];
    int token_count = tokenize(text, tokens);

    int token_pos = 0;
    struct ASTNode root;
    root.type = AST_TEMPLATE;
    root.identifier = ID_TEMPLATE;
    build_ast(&token_pos, token_count, tokens, &root);

    struct ASTNode child = root.children[0];
    ck_assert_int_eq(child.children_len, 0);
    ck_assert_int_eq(child.type, AST_EXPRESSION);
    ck_assert_int_eq(child.identifier, ID_EXTENDS);
    ck_assert_str_eq(child.arguments[0], "base.html");
}
END_TEST

START_TEST (parser_content_token_check) {
    char *text = "This is a content token.";
    TokenNode tokens[1000];
    int token_count = tokenize(text, tokens);

    int token_pos = 0;
    struct ASTNode root;
    root.type = AST_TEMPLATE;
    root.identifier = ID_TEMPLATE;
    build_ast(&token_pos, token_count, tokens, &root);

    struct ASTNode child = root.children[0];
    ck_assert_int_eq(child.children_len, 0);
    ck_assert_int_eq(child.type, AST_CONTENT);
    ck_assert_int_eq(child.identifier, ID_CONTENT);
    ck_assert_str_eq(child.content, "This is a content token.");
}
END_TEST

#define TEST_NO 6
START_TEST (parser_identifier_parse_check) {
    TokenNode tokens_cmp[TEST_NO] = {
        { "if", 3, TOKEN_IDENTIFIER, 0},
        { "else", 5, TOKEN_IDENTIFIER, 1},
        { "insert", 5, TOKEN_IDENTIFIER, 1},
        { "extends", 5, TOKEN_IDENTIFIER, 1},
        { "endblock", 9, TOKEN_IDENTIFIER, 1},
        { "variable", 9, TOKEN_IDENTIFIER, 1},
    };
    enum Identifier ids[TEST_NO] = { ID_IF, ID_ELSE, ID_INSERT, ID_EXTENDS,
        ID_ENDBLOCK, ID_VAR };
    for (int i = 0; i < TEST_NO; i++) {
        ck_assert_int_eq(parse_identifier(&tokens_cmp[i]), ids[i]);
    }
}


START_TEST (parser_sanity_check) {
    char *text = "{{ extends base.html }}"
                 "This is a {% if test == 'hello' %} for template-engine\n"
                 "{% else %} within {%if best %} nested if {% endblock %} this is a"
                 "{{ else_variable }} test {% endblock %}\n"
                 "and now add {% insert title %} title info {% endblock %}";

    TokenNode tokens[1000];
    int token_count = tokenize(text, tokens);

    int token_pos = 0;
    struct ASTNode root;
    root.type = AST_TEMPLATE;
    root.identifier = ID_TEMPLATE;
    build_ast(&token_pos, token_count, tokens, &root);

    // this could be better
    // checking if each top-level node has the right amount children and args
    int arg_count[5] = {1, 0, 3, 0, 1};
    int child_count[5] = {0, 0, 2, 0, 2};
    for (int i = 0; i < root.children_len; i++) {
        struct ASTNode child = root.children[i];
        ck_assert_int_eq(child.children_len, child_count[i]);
        ck_assert_int_eq(child.arguments_len, arg_count[i]);
    }
}
END_TEST

int main(void)
{
    Suite *s1 = suite_create("Core");
    TCase *tc1_1 = tcase_create("Core");
    SRunner *sr = srunner_create(s1);
    int nf;

    suite_add_tcase(s1, tc1_1);
    tcase_add_test(tc1_1, parser_expression_token_check);
    tcase_add_test(tc1_1, parser_content_token_check);
    tcase_add_test(tc1_1, parser_identifier_parse_check);
    tcase_add_test(tc1_1, parser_sanity_check);

    srunner_run_all(sr, CK_ENV);
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return nf == 0 ? 0 : 1;
}
