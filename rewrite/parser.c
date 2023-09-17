/*
 * Extended Backus-Naur Form (EBNF) layout
 * root = statements;
 * block_expression = statements;
 * statements = { statement };
 * statement = CONTENT | expression | block_expression;
 * expression = OPEN_EXPRESSION, IDENTIFIER, arguments, CLOSE_EXPRESSION;
 * arguments = { IDENTIFIER };
 *
 * Builds a very simple AST tree from a list of tokens. Roots are denoted by the
 * initial node or by open-block expressions (TOKEN_BLOCK_OPEN). An 'endblock'
 * denotes the end of a root node. INSERT and IF statements are parent
 * block-expressions and must be terminated using ENDBLOCK.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "temple.h"


void parse_content(int *t_position, TokenNode *t_node, struct ASTNode *node) {
    node->type = AST_CONTENT;
    node->content = (char *)malloc(sizeof(char) * t_node->value_len);
    node->identifier = ID_CONTENT;
    node->children_len = 0;
    node->arguments_len = 0;
    memcpy(node->content, t_node->value, sizeof(char) * t_node->value_len);

    *t_position += 1;
    return;
}

enum Identifier parse_identifier(TokenNode *t_node) {
    // TODO: tidy this up with a hashmap when you implement it in libber
    char *id = t_node->value;
    if (strcmp(id, "if") == 0)
        return ID_IF;
    else if (strcmp(id, "else") == 0)
        return ID_ELSE;
    else if (strcmp(id, "insert") == 0)
        return ID_INSERT;
    else if (strcmp(id, "extends") == 0)
        return ID_EXTENDS;
    else if (strcmp(id, "endblock") == 0)
        return ID_ENDBLOCK;
    else if (strcmp(id, "template") == 0)
        return ID_TEMPLATE;

    return ID_VAR;
}

void parse_expression(int *t_position, int t_count, TokenNode *t_nodes, struct ASTNode *node) {
    int i = *t_position + 1; // move one index forward to skip open token

    // we always assume the first token is the identifier
    node->type = AST_EXPRESSION;
    node->identifier = parse_identifier(&t_nodes[i]);
    if (node->identifier == ID_VAR) { // node value is an arg for var blocks
        node->arguments[node->arguments_len++] = t_nodes[i].value;
    }
    i++;

    // parse remaining (optional) arguments
    while (i < t_count) {
        if (t_nodes[i].type == TOKEN_EXPR_CLOSE || t_nodes[i].type == TOKEN_BLOCK_CLOSE)
            break;

        node->arguments[node->arguments_len++] = t_nodes[i++].value;
    }

    // move past close token if we still have tokens to traverse
    if (i < t_count &&
            (t_nodes[i].type == TOKEN_EXPR_CLOSE || t_nodes[i].type == TOKEN_BLOCK_CLOSE)) {
        i++;
    }

    *t_position = i;
    return;
}

struct ASTNode * build_ast(int *t_position, int t_count, TokenNode *t_nodes, struct ASTNode *root) {
    if (root->identifier == ID_ENDBLOCK) {
        return root;
    }

    root->children = malloc(sizeof(struct ASTNode) * 20);
    while (*t_position < t_count) {
        TokenNode token = t_nodes[*t_position];
        switch(token.type) {
        case TOKEN_CONTENT:
            parse_content(t_position, &token, &root->children[root->children_len++]);
            break;
        case TOKEN_EXPR_OPEN:
            parse_expression(t_position, t_count, t_nodes, &root->children[root->children_len++]);
            break;
        case TOKEN_BLOCK_OPEN:
            parse_expression(t_position, t_count, t_nodes, &root->children[root->children_len++]);
            struct ASTNode *child = build_ast(
                    t_position, t_count, t_nodes,
                    &root->children[root->children_len - 1]);
            // END and ELSE blocks are the only nested structures that need
            // to be returned to un-nest future child nodes for parent
            if (child->identifier == ID_ENDBLOCK || child->identifier == ID_ELSE)
                return root;
            break;
        default:
            break;
        }
    };

    return root;
}

const char *print_identifier(enum Identifier id) {
    switch(id) {
    case ID_IF:
        return "IF";
    case ID_ELSE:
        return "ELSE";
    case ID_INSERT:
        return "INSERT";
    case ID_ENDBLOCK:
        return "ENDBLOCK";
    case ID_EXTENDS:
        return "EXTENDS";
    case ID_CONTENT:
        return "CONTENT";
    case ID_TEMPLATE:
        return "TEMPLATE";
    default:
        return "VAR";
    }
}

const char *print_ast_type(enum ASTNodeType type) {
    switch(type) {
    case AST_TEMPLATE:
        return "AST TEMPLATE";
    case AST_BLOCK_EXPRESSION:
        return "AST BLOCK";
    case AST_EXPRESSION:
        return "AST EXPRESSION";
    default:
        return "AST CONTENT";
    }
}

void print_ast_node(struct ASTNode *node) {
    printf("[%s] id:%s children:%-2d",
            print_ast_type(node->type),
            print_identifier(node->identifier),
            node->children_len);

    if (node->type == AST_CONTENT) {
        printf("content:'%s'", node->content);
    }

    if (node->type == AST_BLOCK_EXPRESSION || node->type == AST_EXPRESSION) {
        printf("args:[");
        for (int i = 0; i < node->arguments_len; i++) {
            printf("%s, ", node->arguments[i]);
        }
        printf("]");
    }
    printf("\n");
}

void print_nary_tree(int level, struct ASTNode *root) {
    // indent node symbol based on level
    for (int i = 0; i < level; i++)
        printf("|\t");
    print_ast_node(root);

    for (int i = 0; i < root->children_len; i++) {
        print_nary_tree(level + 1, &root->children[i]);
    }
}
