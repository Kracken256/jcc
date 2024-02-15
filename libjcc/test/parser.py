#!/usr/bin/env python3

import unittest
from typing import List, Tuple

class NodeBase:
    def __init__(self, value):
        self.value = value

    def __repr__(self):
        return f"{self.__class__.__name__}({self.value})"
    
class IntegerLiteral(NodeBase):
    pass

class FloatingPointLiteral(NodeBase):
    pass

class StringLiteral(NodeBase):
    pass

class Keyword(NodeBase):
    pass

class Identifier(NodeBase):
    pass

class Punctuator(NodeBase):
    pass

class Operator(NodeBase):
    pass
    
precedence = {
    '*': 3,
    '/': 3,
    '+': 2,
    '-': 2,
}

operators = ['+', '-', '*', '/']

def infix_to_postfix(infix_tokens):
   pass

def parse_expression(tokens: List[str]) -> NodeBase:
    pass


# Stream of lexed tokens
# Start at beginning of stream and parse until the expression is complete
# In this case, the expression is a single floating point literal
tokens = [FloatingPointLiteral(3.14159265358979323846), Keyword('const'), Identifier("E"), Punctuator(':'), Identifier("double"), Operator('='), FloatingPointLiteral(2.71828182845904523536), Punctuator('}'), Punctuator('}'), Punctuator('}')]

print(parse_expression(tokens))