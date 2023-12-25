import ast
import operator


bin_ops = {
    ast.Add     : operator.add,
    ast.Sub     : operator.sub,
    ast.Mult    : operator.mul,
    ast.Div     : operator.truediv,
    ast.Mod     : operator.mod,
    ast.Pow     : operator.pow,
    ast.Eq      : operator.eq,
    ast.NotEq   : operator.ne,
    ast.LtE     : operator.le,
    ast.Lt      : operator.lt,
    ast.GtE     : operator.ge,
    ast.Gt      : operator.gt,
    ast.BinOp   : ast.BinOp
}
un_ops = {
    ast.USub: operator.neg,
    ast.UAdd: operator.pos,
    ast.UnaryOp: ast.UnaryOp,
}
ops = tuple(bin_ops) + tuple(un_ops)
var_ops = (ast.Name, ast.Starred)

def _eval(node, vars=None):
    """Traverse a Python AST node until atomic nodes are found. Leaf nodes
    will comprise of operator and/or value nodes. Operator nodes are
    applied to value nodes where provided. Each node is evaluated using a
    limited subset of rules which includes the operations:
        - Mathmatical operators : add, sub, div, mul, mod, pow
        - Comparison operators  : ==, !=, <, >, <=, >=
        - variables evaluation  : variable and starred variables

    :param str expr : expression to be evaluated
    :param vars var : object containing variable name (key) values
    :type vars      : Dict[str, int | bool | str]
    :return str
    """
    match node:
        case ast.Expression():
            return _eval(node.body, vars=vars)
        case ast.Str():
            return node.s
        case ast.Num():
            return node.value
        case ast.Constant():
            return node.value
        case ast.Name():
            if not vars:
                return
            return vars.get(node.id)
        case ast.BinOp():
            left = _eval(node.left, vars=vars)
            right = _eval(node.right, vars=vars)
            return bin_ops[type(node.op)](left, right)
        case ast.UnaryOp():
            if isinstance(node.operand, ops):
                op = _eval(node.operand, vars=vars)
            else:
                op = node.operand.value
            return un_ops[type(node.op)](operand)
        case ast.Compare():
            results = []
            left = _eval(node.left, vars=vars)
            for op, comp in zip(node.ops, node.comparators):
                right = _eval(comp, vars=vars)
                results.append(bin_ops[type(op)](left, right))
                left = right
            return all(results)
        case _:
            raise SyntaxError(f"Incorrect syntax, {type(node)}")


def safe_eval(expr, vars=None):
    """Converts expression into a Python AST node and evaluates it using a
    limited subset of operations.

    :param str expr : expression to be evaluated
    :param vars var : object containing variable name (key) values
    :type vars      : Dict[str, int | bool | str]
    """
    vars = vars or {}
    tree = ast.parse(expr, mode='eval') # 'eval' expects single line-expressions
    return _eval(tree, vars=vars)
