/*
 * - Define ternery tree to receive args from render func or RF hash args into an
 * array.
 * - Traverse tree and build output
 */

#include <stdio.h>
#include "temple.h"

void visit_content(struct ASTNode *root) {
    // printf("PRINTING CONTENT");
    printf("%s ", root->content);
}

void visit_expression(struct ASTNode *root) {
    enum Identifier id = root->identifier;
    switch(id) {
        case ID_VAR:
            // find arg in argument list and print
            // printf("PRINTING VAR");
            printf("%s", root->arguments[0]);
            break;
        case ID_EXTENDS:
            printf("EXTENDING\n");
            break;
        default:
            break;
    }
    return;
}
void visit_block_expression(struct ASTNode *root) {
    // build string for root and return it?
    if (root->identifier == ID_INSERT) {
        // save first arg with string as value in ternary tree
    }

    for (int i = 0; i < root->children_len; i++) {
        struct ASTNode node = root->children[i];
        enum ASTNodeType ast_type = node.type;
        switch(ast_type) {
            case AST_CONTENT:
                visit_content(&node);
                break;
            case AST_EXPRESSION:
                visit_expression(&node);
                break;
            case AST_BLOCK_EXPRESSION:
                // evaluate if statement
                // if true, iterate through children until else
                // otherwise, pass else to visit_block_expression
                // if INSERT node encountered, add root result to ternary tree
                visit_block_expression(&node);
                break;
            default:
                break;
        }
    }
    return;
}

int main() {
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
 // print_nary_tree(0, &root);
 visit_block_expression(&root);

 return 0;
};
