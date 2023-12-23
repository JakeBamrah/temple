import enum
from collections import namedtuple


class Token(enum.Enum):
    EXPR_OPEN     = 'token_expr_open'
    EXPR_CLOSE    = 'token_expr_close'
    BLOCK_OPEN    = 'token_block_open'
    BLOCK_CLOSE   = 'token_block_close'
    IDENTIFIER    = 'token_identifier'
    CONTENT       = 'token_content'
    DEFAULT       = 'token_default'


LexerToken = namedtuple('LexerToken', 'type value line_no')


class Lexer:
    """Tokenizes input into the following tokens:
        - Expression open   - {{
        - Expression closed - }}
        - Block open        - {%
        - Block closed      - %}
        - Identifier        - {{ IDENTIFER_TOKEN }}
        - Content           - str value that does not live in a expr. or block

    :param str text                 : text to be tokenized
    :property int line_no           : number of lines in text
    :property LexerToken[] tokens   : Tokenized text
    """

    def __init__(self, text=None):
        self.line_no = 0
        self.tokens = []

        if text:
            self.tokens = self.tokenize(text)


    def tokenize(self, text):
        """Tokenizes string input.

        :param str text : text to be tokenized
        :return         : LexerToken[]
        """
        i = 1
        j = 0
        stack = []
        tokens = []
        str_len = len(text)
        while i < str_len:
            state = Token.DEFAULT if not stack else stack[-1]
            if (state == Token.DEFAULT):
                if text[i - 1] == '{' and (text[i] == '{' or text[i] == '%'):
                    t_type = Token.EXPR_OPEN if text[i] == '{' else Token.BLOCK_OPEN
                    stack.append(t_type)
                    if i > 0: # start of expr. - check if content has been found
                        tokens.append(LexerToken(Token.CONTENT, text[j:i - 1], self.line_no))
                    j = i + 1
            elif state == Token.EXPR_OPEN or state == Token.BLOCK_OPEN:
                if (text[i - 1] == '}' or text[i - 1] == '%') and text[i] == '}':
                    # build token nodes for entire expression
                    tokens.append(LexerToken(state, "", self.line_no))
                    for tok in text[j:i - 1].split(): # found identifier block
                        # found end of block / expr - add identifier tokens
                        tokens.append(LexerToken(Token.IDENTIFIER, tok, self.line_no))

                    end_t_type = Token.EXPR_CLOSE if state == Token.EXPR_OPEN else Token.BLOCK_CLOSE
                    tokens.append(LexerToken(end_t_type, "", self.line_no))
                    j = i + 1
                    stack.pop() # end of expression or block

            if text[i] == '\n':
                self.line_no += 1

            i += 1

        if j < i: # check for any remaining content when we reach the end
            tokens.append(LexerToken(Token.CONTENT, text[j:], self.line_no))

        return tokens
