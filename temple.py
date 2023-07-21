import os
import re
import enum
import glob
import pathlib
from collections import namedtuple


class BlockEnum(enum.Enum):
    BLOCK   = "BLOCK"
    DO      = "DO"
    EXTENDS = "EXTENDS"


class InvalidTemplateError(BaseException):
    pass


Token = namedtuple('Token', ('type', 'key', 'values'), defaults=[[]])


IN_QUOTES_PAT       = r'"(.*?)"'
TEMPLATE_TOKEN_PAT  = r'\{\%.*?\%\}'

def parse(filename):
    """Parses html template files—supports inheritance, blocks and for-loops."""
    lines = []
    path = pathlib.Path(filename)
    with open(path, 'r') as f:
        lines = f.readlines()

    html = ""
    block_str = ""
    template_blocks = []
    for line in lines:
        token_matches = re.findall(TEMPLATE_TOKEN_PAT, line)
        if token_matches:
            raw_token = token_matches[0]
            # pull in entire parent template
            if raw_token.startswith('{% extends'):
                template_blocks.append(Token(BlockEnum.EXTENDS, raw_token))
                parent_path = re.search(IN_QUOTES_PAT, raw_token).groups()[0]
                with open(parent_path, 'r') as f:
                    # TODO: not recursive-only parent depth of one supported
                    html += f.read()

            # start of child block-track lines within block to add into parent
            if raw_token.startswith('{% block'):
                template_blocks.append(Token(BlockEnum.BLOCK, raw_token))

            if raw_token.startswith('{% do '):
                raw_els = re.search(IN_QUOTES_PAT, raw_token).groups()[0]
                token = Token(BlockEnum.DO, '{{ VALUES }}', raw_els.split(','))
                template_blocks.append(token)

            # end of child block—add lines into parent html block
            if raw_token == '{% endblock %}':
                if not template_blocks:
                    msg = 'Missing template starting block or ending block.'
                    raise InvalidTemplateError(msg)

                token = template_blocks.pop()
                if token.type == BlockEnum.BLOCK:
                    html = re.sub(token.key, block_str, html)
                    block_str = ""
                if token.type == BlockEnum.DO:
                    fmt_vals = "\n".join(token.values)
                    block_str = re.sub(token.key, fmt_vals, block_str)

        # add lines when not within blocks
        if template_blocks and not token_matches:
            block_str += line
        elif not token_matches:
            html += line

    return html


def build(path, output_path='./output'):
    """Parses templates and compiles html files."""
    # we overwrite the build directory each time
    dir_path = pathlib.Path(output_path)
    dir_path.mkdir(exist_ok=True)

    files = glob.glob(path, recursive=True)
    print('Compiling html for templates: {}'.format(" ".join(files)))

    for file in files:
        # make sure to create any sub dirs before writing files
        output_path = os.path.join(dir_path, file)
        output_dir_path = os.path.dirname(output_path)
        if not os.path.exists(output_dir_path):
            os.mkdir(output_dir_path)

        with open(output_path, 'w') as f:
            f.write(parse(file))
