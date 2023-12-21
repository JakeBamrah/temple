/*
 * - Define ternery tree to receive args from render func or RF hash args into an
 * array.
 * - Traverse tree and build output
 */

#include <stdio.h>

#include "libh.h"
#include "temple.h"


void visit_content(struct ASTNode *root, char *key) {
    printf("[KEY: %s] %s ", key, root->content);
}

void visit_expression(struct ASTNode *root) {
    enum Identifier id = root->identifier;
    switch(id) {
        case ID_VAR:
            // find arg in argument hashmap and add to key-value string
            printf("%s", root->arguments[0]);
            break;
        case ID_EXTENDS:
            // add extend key to hashmap
            // after child traversal (in main) we will traverse parent and
            // insert child nodes where needed
            printf("[EXTEND PARENT: %s]\n", root->arguments[0]);
            break;
        default:
            break;
    }
    return;
}

void visit_block_expression(struct ASTNode *root, char *key) {
    for (int i = 0; i < root->children_len; i++) {
        struct ASTNode node = root->children[i];
        enum Identifier id = node.identifier;
        switch(id) {
            case ID_IF:
                // TODO: validate if statement properly using args list
                if (node.arguments[0]) {
                    for (int j = 0; j < node.children_len; j++) {
                        if (node.children[j].identifier == ID_ELSE)
                            continue;
                        traverse(&node.children[j], key);
                    }
                }
                else {
                    if (node.has_inverse_node) {
                        traverse(node.inverse_node, key);
                    }
                }
                break;
            case ID_INSERT:
                // if child store string value using insert key (to be used by
                // parent "extends" later)
                traverse(&node, node.arguments[0]); // add hashmap arg

                // else if not child (parent), check hashmap
                //      if hashmap lookup - insert string into root key
                //      elsee traverse with root key and use parent default
                break;
            default:
                traverse(&node, key);
                break;
        }
    }
}

void traverse(struct ASTNode *root, char *key) {
    // pass key and hashmap
    // when content is entered, append string to value in current key
    // then we have a hashmap
    switch(root->type) {
        case AST_CONTENT:
            visit_content(root, key);
            break;
        case AST_EXPRESSION:
            visit_expression(root);
            break;
        case AST_BLOCK_EXPRESSION:
            visit_block_expression(root, key);
            break;
        case AST_TEMPLATE:
            visit_block_expression(root, key);
            break;
        default:
            break;
    }
    return;
}

int main() {
  char *text = "{{ extends base.html }}"
        "{%insert title %} This is a {% if test == 'hello' %} for template-engine\n"
        "{% else %} within {%if best %} nested if {% endblock %} this is a"
        "{{ else_variable }} test {% endblock %}\n"
        "and now add title info {% endblock %}"
        "{%insert body %} We are in body {% endblock %}";

 TokenNode tokens[1000];
 int token_count = tokenize(text, tokens);

 int token_pos = 0;
 struct ASTNode root;
 root.type = AST_TEMPLATE;
 root.identifier = ID_TEMPLATE;
 build_ast(&token_pos, token_count, tokens, &root);
 /* print_nary_tree(0, &root); */
 char *key = "root";
 traverse(&root, key);

 return 0;
};
