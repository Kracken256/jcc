#!/usr/bin/env python3

import unittest
from typing import List, Tuple

class Node:
    def __init__(self, value: str, children: List['Node']):
        self.value = value
        self.children = children

    def __repr__(self):
        return f"Node({self.value}, {self.children})"
    
    def __str__(self):
        def print_tree(node, prefix, is_tail, res) -> str:
            res += prefix + ("└── " if is_tail else "├── ") + node.value + "\n"
            for i, child in enumerate(node.children):
                res = print_tree(child, prefix + ("    " if is_tail else "│   "), i == len(node.children) - 1, res)
            return res
        
        return print_tree(self, "", True, '')
    
precedence = {
    '*': 3,
    '/': 3,
    '+': 2,
    '-': 2,
    '&': 1,
}

operators = ['+', '-', '*', '/', '^', '&']

def is_literal(token: str) -> bool:
    # Check if is valid function name
    if len(token) == 0:
        return False
    if token[0].isdigit():
        return False
    for char in token:
        if not char.isalnum():
            return False
    return True

def is_function_call(token: str) -> bool:
    # Check if is valid function name
    if len(token) < 3:
        return False
    if token[0].isdigit():
        return False
    
    for char in token[:-2]:
        if not char.isalnum():
            return False
        
    if token[-2:] != '()':
        return False
    
    return True

def infix_to_postfix(infix_tokens):
    output = []
    operator_stack = []
    for token in infix_tokens:
        if token.isdigit():
            output.append(Node(token, []))
        elif is_function_call(token):
            output.append(Node(token, []))
        elif token == '(':
            operator_stack.append(token)
        elif token == ')':
            while operator_stack and operator_stack[-1] != '(':
                output.append(Node(operator_stack.pop(), []))
            operator_stack.pop()  # Discard the '('
        elif token in operators:
            while (operator_stack and operator_stack[-1] != '(' and  precedence[token] <= precedence[operator_stack[-1]]):
                output.append(Node(operator_stack.pop(), []))
            operator_stack.append(token)
        else:
            break

    while operator_stack:
        output.append(Node(operator_stack.pop(), []))
    return output

def parse_expression(tokens: List[str]) -> Node:
    # Helper function to convert infix expression to postfix

    print('infix', tokens)
    postfix_tokens = infix_to_postfix(tokens)
    print('postfix', postfix_tokens)
    operand_stack = []

    for token in postfix_tokens:
        if token.value.isdigit():
            operand_stack.append(token)
        elif is_function_call(token.value):
            operand_stack.append(token)
        else:
            # Pop two operands from the stack to construct the expression tree
            try:
                right_operand = operand_stack.pop()
                left_operand = operand_stack.pop()
            except IndexError:
                return None
            operand_stack.append(Node(token.value, [left_operand, right_operand]))

    print(operand_stack)

    return operand_stack[0] if operand_stack else None

def ast_evaluate(ast: Node) -> int:
    if ast.value.isdigit():
        return int(ast.value)
    elif is_function_call(ast.value):
        raise ValueError("Function call not supported")
    else:
        left = ast_evaluate(ast.children[0])
        right = ast_evaluate(ast.children[1])
        if ast.value == '+':
            return left + right
        elif ast.value == '-':
            return left - right
        elif ast.value == '*':
            return left * right
        elif ast.value == '/':
            return left // right
        elif ast.value == '^':
            return left ** right
        elif ast.value == '&':
            return left & right
        else:
            raise ValueError("Invalid operator")

# Test your implementation

class TestParseExpression(unittest.TestCase):
    def test_simple_addition(self):
        tokens = ["2", "+", "3"]
        ast = parse_expression(tokens)
        self.assertEqual(str(ast), "Node(+, [Node(2, []), Node(3, [])])")

    def test_operator_precedence(self):
        tokens = ["2", "+", "3", "*", "4"]
        ast = parse_expression(tokens)
        self.assertEqual(str(ast), "Node(+, [Node(2, []), Node(*, [Node(3, []), Node(4, [])])])")

    def test_parentheses(self):
        tokens = ["(", "2", "+", "3", ")", "*", "4"]
        ast = parse_expression(tokens)
        self.assertEqual(str(ast), "Node(*, [Node(+, [Node(2, []), Node(3, [])]), Node(4, [])])")

    def test_function_call(self):
        tokens = ["func()", "*", "5"]
        ast = parse_expression(tokens)
        self.assertEqual(str(ast), "Node(*, [Node(func(), []), Node(5, [])])")

    def test_invalid_function_call(self):
        tokens = ["fun()()", "*", "5"]
        ast = parse_expression(tokens)
        self.assertIsNone(ast)

    def test_invalid_expression(self):
        tokens = ["2", "+", "*", "3"]
        ast = parse_expression(tokens)
        self.assertIsNone(ast)

    def test_empty_expression(self):
        tokens = []
        ast = parse_expression(tokens)
        self.assertIsNone(ast)

    def test_invalid_token(self):
        tokens = ["2", "+", "3", "a"]
        ast = parse_expression(tokens)
        self.assertEqual(str(ast), "Node(+, [Node(2, []), Node(3, [])])")


# if __name__ == '__main__':
#     unittest.main()
        

print(parse_expression(["2", "+", "(", "hello()", "+", "3",')']))
