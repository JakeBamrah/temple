import os
import re
import glob
import pathlib

from lexer import Lexer
from parser import Parser
from interpreter import Interpreter


def compile_template(filename, key='root', template_vars=None, insert_vars=None):
    text = ""
    path = pathlib.Path(filename)
    dir_prefix = path.resolve().parent
    with open(path, 'r') as f:
        text = f.read()

    # tokenize templates
    l = Lexer(text=text)

    # build AST for templates
    p = Parser(tokens=l.tokens)

    # convert AST into completed corpus
    i = Interpreter(template_vars=template_vars, insert_vars=insert_vars)
    i.traverse(p.root, key)
    body = i.body

    # build text and html for current file
    # if extension doc provided we recursively traverse parents from bottom-up
    # whilie using our child "body" object to fill INSERT variables in parents
    if i.extends:
        path = dir_prefix.joinpath(i.extends) # convert rel. path to abs. for parent
        body = compile_template(path, key, template_vars=template_vars,
                                insert_vars=i.body)
    return body # final body obj will always contain a root key with content


SUBDIR_PAT = r'\.\.\/'
def build(path, output_path='./output'):
    """Parses templates and compiles html files."""
    # we overwrite the build directory each time
    dir_path = pathlib.Path(output_path)
    dir_path.mkdir(exist_ok=True)

    files = glob.glob(path, recursive=True)
    print('Compiling html for templates: {}'.format(" ".join(files)))

    for file in files:
        # make sure to create any sub dirs before writing files
        # remove relative paths and retain folder and file name for output dir
        output_path = os.path.join(dir_path, re.sub(SUBDIR_PAT, '', file))
        output_dir_path = os.path.dirname(output_path)
        if not os.path.exists(output_dir_path):
            os.mkdir(output_dir_path)

        with open(output_path, 'w') as f:
            body = compile_template(file)['root']
            f.write(body)
