import enum

from lexer import Token


class ASTNodeType(enum.Enum):
    BLOCK       = 'ast_block_expression'
    EXPRESSION  = 'ast_expression'
    CONTENT     = 'ast_content'
    TEMPLATE    = 'ast_template'


class ExpressionType(enum.Enum):
    IF          = 'if'
    ELSE        = 'else'
    INSERT      = 'insert'
    EXTENDS     = 'extends'
    ENDBLOCK    = 'endblock'
    CONTENT     = 'content'
    TEMPLATE    = 'template'
    VAR         = 'var'

exp_id_to_enum_map = {e.value: e for e in ExpressionType}
exclude_from_args = {ExpressionType.EXTENDS,
                     ExpressionType.INSERT,
                     ExpressionType.IF}


class ASTNode:
    def __init__(self, type, expression_id, args=None, content=None,
                 children=None, inverse_node=None):
        self.type = type
        self.expression_id = expression_id
        self.args = args or list()
        self.content = content
        self.children = children or list()
        self.inverse_node = inverse_node

    def __repr__(self):
        args = ", ".join(self.args)
        return (f"ASTNode(type={self.type} expression={self.expression_id} "
                f"args=[{args}] content='{self.content}' inverse_node={self.inverse_node})")

class Parser:
    """Parses tokenized input into an abstract-syntax-tree (AST). AST nodes are
    as follows:
        - IF        : AST Block     Conditional block expression
        - ELSE      : AST Block     Block expression used if IF block fails
        - ENDBLOCK  : AST Block     end of a block statement
        - INSERT    : AST Expr.     Body of text to be inserted into parent
        - EXTENDS   : AST Expr.     parent template to inherit from
        - VAR       : AST Expr.     vars to be inserted into an AST Expr. node
        - CONTENT   : AST Content   str values that exist outside of blocks
        - TEMPLATE  : AST Template  top-level parent of an AST

    :param LexerToken[] tokens  : Tokenized text
    :property int token_pos:    : tracks total processed tokens in token array
    :property ASTNode root      : root node for AST
    """

    def __init__(self, tokens=None):
        self.token_pos = 0
        self.root = ASTNode(type=ASTNodeType.TEMPLATE,
                            expression_id=ExpressionType.TEMPLATE)

        # build AST tree
        if tokens:
            self.build_ast(tokens, self.root)


    @staticmethod
    def print_nary_tree(depth, root):
        s = "|\t" * depth
        print(s, root)

        for u in root.children:
            Parser.print_nary_tree(depth + 1, u)
        if root.inverse_node: # print else statement last if one present
            Parser.print_nary_tree(depth, root.inverse_node)


    def parse_expr_from_token(self, t):
        id = exp_id_to_enum_map.get(t.value, ExpressionType.VAR)
        if not id:
            raise ValueError("Expression id cannot be parsed from token")
        return id


    def parse_content(self, tokens, root):
        t = tokens[self.token_pos]
        node = ASTNode(type=ASTNodeType.CONTENT,
                       expression_id=ExpressionType.CONTENT,
                       content=t.value)
        root.children.append(node)
        self.token_pos += 1


    def parse_expression(self, tokens, root, block_start=False):
        i = self.token_pos + 1 # move pos one forward to skip open token
        t = tokens[i]
        t_count = len(tokens)
        ast_type = ASTNodeType.BLOCK if block_start else ASTNodeType.EXPRESSION
        node = ASTNode(type=ast_type,
                       expression_id=self.parse_expr_from_token(t))
        if node.expression_id == ExpressionType.VAR:
            node.args.append(t.value)
            i += 1

        if node.expression_id in exclude_from_args:
            i += 1 # no need to include the expression value twice

        while i < t_count: # get all arguments for the EXPR / BLOCK node
            t = tokens[i]
            if t.type == Token.EXPR_CLOSE or t.type == Token.BLOCK_CLOSE:
                break
            node.args.append(t.value)
            i += 1

        root.children.append(node)
        if i < t_count and (tokens[i].type == Token.EXPR_CLOSE or
                            tokens[i].type == Token.BLOCK_CLOSE):
            i += 1
        self.token_pos = i


    def build_ast(self, tokens, root):
        t_count = len(tokens)
        if root.expression_id == ExpressionType.ENDBLOCK:
            return root

        while self.token_pos < t_count:
            t = tokens[self.token_pos]
            match t.type:
                case Token.CONTENT:
                    self.parse_content(tokens, root)
                case Token.EXPR_OPEN:
                    self.parse_expression(tokens, root)
                case Token.BLOCK_OPEN:
                    # parse initial if / else before traversing it's children
                    self.parse_expression(tokens, root, block_start=True)
                    child = self.build_ast(tokens, root.children[-1])
                    if child.expression_id == ExpressionType.ELSE:
                        # else is a special child it is added as an inverse node
                        root.children.pop()
                        root.inverse_node = child
                        return root
                    if child.expression_id == ExpressionType.ENDBLOCK:
                        # we've now reached the end of the block
                        return root
                case _:
                    print('invalid node type')
        return root
