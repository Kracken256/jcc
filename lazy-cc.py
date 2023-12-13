#!/usr/bin/env python3

class CppParser:
    def __init__(self):
        self.tokens = []
        self.index = 0

    def tokenize(self, code):
        # Tokenization logic (simplified for illustration)
        self.tokens = code.split()

    def parse(self, code):
        self.tokenize(code)
        self.index = 0
        return self.parse_namespace()

    def parse_namespace(self):
        namespace = {'name': self.tokens[self.index + 1], 'children': []}
        self.index += 3  # Skip 'namespace <name> {'
        while self.index < len(self.tokens):
            if self.tokens[self.index] == 'namespace':
                sub_namespace = self.parse_namespace()
                namespace['children'].append(sub_namespace)
            elif self.tokens[self.index] in ('void', 'int', 'const', 'long'):
                func_or_var = self.parse_function_or_variable()
                namespace['children'].append(func_or_var)
            elif self.tokens[self.index] == '}':
                self.index += 1
                break
            else:
                self.index += 1
        return namespace

    def parse_function_or_variable(self):
        func_or_var = {'type': self.tokens[self.index], 'name': self.tokens[self.index + 1]}
        self.index += 2  # Skip type and name
        while self.index < len(self.tokens):
            if self.tokens[self.index] == '{':
                # Handle function body (not implemented in this simplified example)
                self.skip_function_body()
            elif self.tokens[self.index] == ';':
                self.index += 1
                break
            else:
                self.index += 1
        return func_or_var

    def skip_function_body(self):
        # For simplicity, skipping the function body here
        brace_count = 1
        while brace_count > 0:
            self.index += 1
            if self.tokens[self.index] == '{':
                brace_count += 1
            elif self.tokens[self.index] == '}':
                brace_count -= 1

# Example usage remains the same...


# Example usage
cpp_parser = CppParser()
cpp_code = ""
with open("test-code.cpp", "r") as f:
    cpp_code = f.read()

parsed_ast = cpp_parser.parse(cpp_code)

# Pretty print the AST
def pretty_print_ast(ast, indent=0):
    print('  ' * indent + ast['name'])
    if 'children' in ast:
        for child in ast['children']:
            pretty_print_ast(child, indent + 1)

pretty_print_ast(parsed_ast)
