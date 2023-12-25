from evaluate import safe_eval
from parser import ASTNodeType, ExpressionType


class Interpreter:
    """Interpreter is responsible for converting an abstract-syntax-tree (AST)
    into a complete text body (prepared by Parser). The AST is traversed and
    added into the Interpreter.body object. The body object is responsible for
    building the final output strings from a given template. It stores these
    strings under a 'root' key which can be accessed once the AST has been
    traversed.

    Insert blocks are handled by gathering their block strings into specfic
    insert keys within the Interpreter.body object. The Int.body is passed to
    the parent template that the child "extends" from where the parent can
    impute these child strings into it's template as needed.

    Note: A child can only "extend" (inherit) from *one* parent.

    :param obj template_vars : variables to be replaced in templates
    :param obj insert_vars   : carries insert block info to parent template
    :param str extends       : stores a parent template to extend from
    :type obj                : dict[str: str]
    """

    def __init__(self, template_vars=None, insert_vars=None):
        self.body = {}
        self.extends = None
        self.template_vars = template_vars or {}
        self.insert_vars = insert_vars or {}


    def visit_content(self, root, key):
        self.body[key] += root.content


    def visit_expression(self, root, key):
        """Inserts variables into a block string and checks for template
        extension (inheritance).
        """
        match root.expression_id:
            case ExpressionType.VAR:
                self.body[key] += safe_eval(root.args[0], vars=self.template_vars)
            case ExpressionType.EXTENDS:
                self.extends = root.args[0]
            case _:
                pass


    def visit_block(self, root, key):
        """Inserts IF/ELSE block strings into final string body. INSERT blocks
        are added under a new key so that the parent template can use these
        keys to fill in their insert blocks later in the traversal. If
        insert_vars are provided, we assume that this is a top-level or parent
        file.

        :param ASTNode root : root node for AST
        :param str key      : key used to insert content into body object
        :return             : None
        """
        for node in root.children:
            match node.expression_id:
                case ExpressionType.IF:
                    # check whether expression boolean has been passed
                    expr = "".join(node.args)
                    if (safe_eval(expr, vars=self.template_vars)):
                        for c in node.children:
                            self.traverse(c, key)
                    elif node.inverse_node: # otherwise defer to else statement
                        self.traverse(node.inverse_node, key)
                case ExpressionType.INSERT:
                    # check to see if we have values to insert from child
                    # if not, treat as an insertion block
                    if node.args[0] in self.insert_vars:
                        if node.args[0] not in self.body:
                            self.body[node.args[0]] = ""
                        self.body[key] += self.insert_vars[node.args[0]]
                    elif not self.insert_vars:
                        # only traverse inserts if we are a child doc
                        self.traverse(node, node.args[0])
                case _:
                    self.traverse(node, key)


    def traverse(self, root, key):
        if key not in self.body:
            self.body[key] = ''

        match root.type:
            case ASTNodeType.CONTENT:
                self.visit_content(root, key)
            case ASTNodeType.EXPRESSION:
                self.visit_expression(root, key)
            case ASTNodeType.BLOCK:
                self.visit_block(root, key)
            case ASTNodeType.TEMPLATE:
                self.visit_block(root, key)
            case _:
                pass
        return
