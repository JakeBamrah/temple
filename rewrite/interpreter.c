/*
 * - Define ternery tree to receive args from render func or RF hash args into an
 * array.
 * - Traverse tree and build output
 */

#include <stdio.h>
#include "temple.h"

void visit_content(struct ASTNode *root) {
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
            // leave extends until last, evaluate rest of template first
            printf("EXTENDING\n");
            break;
        default:
            break;
    }
    return;
}

void visit_block_expression(struct ASTNode *root) {
    for (int i = 0; i < root->children_len; i++) {
        struct ASTNode node = root->children[i];
        enum Identifier id = node.identifier;
        switch(id) {
            case ID_IF:
                // TODO: validate if statement properly using args list
                if (!node.arguments[0]) {
                    for (int j = 0; j < node.children_len; j++) {
                        if (node.children[j].identifier == ID_ELSE)
                            continue;
                        traverse(&node.children[j]);
                    }
                }
                else {
                    if (node.has_inverse_node) {
                        traverse(node.inverse_node);
                    }
                }
                break;
            case ID_INSERT:
                // save first arg with string as value in ternary tree
                traverse(&node);
                break;
            default:
                traverse(&node);
                break;
        }
    }
}

void traverse(struct ASTNode *root) {
    // build string for root and return it?
    switch(root->type) {
        case AST_CONTENT:
            visit_content(root);
            break;
        case AST_EXPRESSION:
            visit_expression(root);
            break;
        case AST_BLOCK_EXPRESSION:
            visit_block_expression(root);
            break;
        case AST_TEMPLATE:
            visit_block_expression(root);
            break;
        default:
            break;
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
 /* print_nary_tree(0, &root); */
 traverse(&root);

 return 0;
};
