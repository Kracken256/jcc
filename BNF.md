# Extended Backus–Naur Form Specification

A work in progress.

```bnf
<letter> ::= "A" | "B" | "C" | "D" | "E" | "F" | "G"
	       | "H" | "I" | "J" | "K" | "L" | "M" | "N"
	       | "O" | "P" | "Q" | "R" | "S" | "T" | "U"
	       | "V" | "W" | "X" | "Y" | "Z" | "a" | "b"
	       | "c" | "d" | "e" | "f" | "g" | "h" | "i"
	       | "j" | "k" | "l" | "m" | "n" | "o" | "p"
	       | "q" | "r" | "s" | "t" | "u" | "v" | "w"
	       | "x" | "y" | "z"

<digit> ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" 

/* Must start with a character or underscore. */
<identifier_symbol> ::= "_" | <letter>

<whitespace_char> ::= " " | "\t" | "\n"
<whitespace> ::= <whitespace_char> | <whitespace>  <whitespace_char>

<identifier_trailing_char> ::= <identifier_symbol> | <digit>
<identifier_trailing> ::= <identifier_trailing_char> | <identifier_trailing> <identifier_trailing_char>

/* Standard identifier format */ 
<identifier> ::= <identifier_symbol> | <identifier_symbol> <identifier_trailing>

/* TODO */
<typename> ::= <identifier>

/* Struct declarations */
<struct_declaration> ::= "struct" <whitespace> <identifier> <whitespace>? ";"

/* Union declarations */
<union_declaration> ::= "union" <whitespace> <identifier> <whitespace>? ";"

/* Class declarations */
<class_declaration> ::= "class" <whitespace> <identifier> <whitespace>? ";"

/* TODO */
<string_literal_char> ::= <letter> | <digit> | "[" | "]" | "{" | "}" | "(" 
			| ")" | "<" | ">" | "'" | "=" | "|" | "." | "," | ";" | "-" 
			| "+" | "*" | "?"

<string_literal_chars> ::= <string_literal_char> | <string_literal_chars> <string_literal_char>

/* String literals are wrapped in matching " or ' characters */
<string_literal> ::= "\"" <string_literal_chars> "\"" | "'" <string_literal_chars> "'"

<integer_literal> ::= <digit> | <integer_literal> <digit>

/* TODO */
<floating_point_literal> ::= <integer_literal> "."? <integer_literal>

/* TODO */
<constant_expr> ::= <string_literal> | <integer_literal> | <floating_point_literal> | "null"

/* TODO */
<expression> ::= <identifier>

/* TODO */
<code_block> ::= "{}" 

/* name: typename [= default] */
<function_parameter> ::= <identifier> <whitespace>? ":" <whitespace>? <identifier> | <identifier> 
	<whitespace>? ":" <whitespace>? <identifier> <whitespace>? "=" <whitespace>? <constant_expr>

/* param1: type1 [= default 1], param2: type2 [= default 2], ... */
<function_parameter_list> ::= <function_parameter> | <function_parameter_list> <whitespace>? "," 
	<whitespace>? <function_parameter>

<function_parameters> ::= <function_parameter_list>?

/* : typename */
<function_return> ::= ":" <whitespace>? <typename>

/* func name([param1: type1 [= default 1], param2: type2 [= default 2], ...])[:return_ty[e]]; */
<function_declaration> ::= "func" <whitespace> <identifier> <whitespace>? "(" <whitespace>? 
	<function_parameters> <whitespace>? ")" <whitespace>? <function_return>? <whitespace>? ";"

/* let name: typename [= value]; */
<let_declaration> ::= "let" <whitespace> <identifier> <whitespace>? ":" <whitespace>? 
	<typename> <whitespace>? ";" | "let" <whitespace> <identifier> <whitespace>? ":" 
	<whitespace>? <typename> <whitespace>? "=" <whitespace>? <expression> <whitespace>? ";"

/* var name: typename [= value]; */
<var_declaration> ::= "var" <whitespace> <identifier> <whitespace>? ":" <whitespace>? 
	<typename> <whitespace>? ";" | "var" <whitespace> <identifier> <whitespace>? ":" 
	<whitespace>? <typename> <whitespace>? "=" <whitespace>? <expression> <whitespace>? ";"

/* const name: typename = value; */
<const_declaration> ::= "const" <whitespace> <identifier> <whitespace>? ":" <whitespace>? 
	<typename> <whitespace>? "=" <constant_expr> <whitespace>? ";"

/* name: type [= default value]; */
<struct_field> ::= <identifier> <whitespace>? ":" <whitespace>? <typename> | <identifier> 
	<whitespace>? ":" <whitespace>? <typename> <whitespace>? "=" <whitespace>? <constant_expr> ";"

/* name: ([param1: type1 [= default 1], param2: type2 [= default 2], ...])[:return_ty[e]]; */
<struct_method> ::= <identifier> <whitespace>? ":" <whitespace>? "(" <whitespace>? 
	<function_parameters> <whitespace>? ")" <whitespace>? <function_return>? <whitespace>? ";"

/* @name(constant expression); */
<struct_attribute> ::= "@" <identifier> <whitespace>? "(" <whitespace>? <constant_expr> 
	<whitespace>? ")" <whitespace>? ";"

/* [@name(constant expression);]... */
<struct_attributes> ::= <struct_attribute> | <struct_attribute> <whitespace>? <struct_attributes>

/* [@name(constant expression);]... name: type [= default value];*/
<struct_field_with_attributes> ::= <struct_attributes>? <whitespace>? <struct_field>

/* [@name(constant expression);]... name: ([param1: type1 [= default 1], param2: type2 [= default 2], ...])[:return_ty[e]]; */
<struct_method_with_attributes> ::= <struct_attributes>? <whitespace>? <struct_method>

<struct_body_item> ::= <struct_field_with_attributes> | <struct_method_with_attributes>

<struct_body_items> ::= <struct_body_item> | <struct_body_items> <struct_body_item>

/* struct name { ... }; */
<struct_definition> ::= "struct" <whitespace> <identifier> <whitespace>? "{" <whitespace>? <struct_body_items> <whitespace>? "}" <whitespace>? ";"

/* union name { ... }; */
<union_definition> ::= "union" <whitespace> <identifier> <whitespace>? "{" <whitespace>? <struct_body_items> <whitespace>? "}" <whitespace>? ";"
```