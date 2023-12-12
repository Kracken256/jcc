package main

import (
	"encoding/json"
	"fmt"
	"os"
	"strconv"
)

/*
	My ambitious attempt to build a custom language and compiler.
*/

const (
	AttributeTypeNumber  = 10
	AttributeTypeFloat   = 20
	AttributeTypeString  = 30
	AttributeTypeBoolean = 40
)

var AttributeTypes = map[int]string{
	AttributeTypeNumber:  "Number",
	AttributeTypeFloat:   "Float",
	AttributeTypeString:  "String",
	AttributeTypeBoolean: "Boolean",
}

const (
	TokenTypeKeyword       = 10
	TokenTypeLiteralNumber = 20
	TokenTypeLiteralString = 30
	TokenTypeIdentifier    = 40
	TokenTypeOperator      = 50
	TokenTypeComment       = 70
	TokenTypeSeparator     = 80
)

var TokenTypes = map[int]string{
	TokenTypeKeyword:       "Keyword",
	TokenTypeLiteralNumber: "LiteralNumber",
	TokenTypeLiteralString: "LiteralString",
	TokenTypeIdentifier:    "Identifier",
	TokenTypeOperator:      "Operator",
	TokenTypeComment:       "Comment",
	TokenTypeSeparator:     "Separator",
}

var LexSeparators = []rune{
	'(', ')', '{', '}', '[', ']', ';', ',', ':',
}

var LexOperators = []string{
	"&&", "||", "<=", ">=", "??", "==", "!=", "::", "=", "+", "-", "*", "/", "%", "|", "&", "^", "!", "<", ">", ".", "?", "#", "@//", "@",
}

var LexKeywords = []string{
	"namespace", "include", "export", "module", "on", "for", "switch", "case", "break", "while", "default", "typedef", "template", "contraints", "and", "or", "not", "nor", "nand", "nand", "xor", "xnor", "auto", "true", "false", "on", "off", "high", "low"}

var LexTypesPrimitive = []string{
	"intn", "string", "bool", "bit", "line", "uintn",
}

var LexIndentifierChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"

type Attribute struct {
	Name  string
	Type  int
	Value interface{}
}

type Token struct {
	Type  int
	Value string
}

type CompilerUnit struct {
	FileName   string
	Tokens     []Token
	Attributes []Attribute
}

func main() {
	var inputFiles []string
	var outputFile string = "out.vhdl"
	var i = 0

	if len(os.Args) < 2 {
		fmt.Println("No input files specified")
		PrintHelp()
		os.Exit(1)
	}

	// Parse command line arguments
	for i = 1; i < len(os.Args); i++ {
		var arg = os.Args[i]

		if len(arg) < 1 {
			fmt.Println("Invalid argument: ", arg)
			os.Exit(1)
		}

		if arg[0] == '-' {
			if len(arg) < 2 {
				fmt.Println("Invalid argument: ", arg)
				os.Exit(1)
			}
			switch arg[1] {
			case 'o':
				if len(os.Args) < i+2 {
					fmt.Println("No output file specified")
					os.Exit(1)
				}
				outputFile = os.Args[i+1]
				i++
			default:
				fmt.Println("Invalid argument: ", arg)
				os.Exit(1)
			}
		} else {
			inputFiles = append(inputFiles, arg)
		}
	}

	if len(inputFiles) < 1 {
		fmt.Println("No input files specified")
		PrintHelp()
		os.Exit(1)
	}

	// check each input file exists
	for i = 0; i < len(inputFiles); i++ {
		var file = inputFiles[i]
		if _, err := os.Stat(file); os.IsNotExist(err) {
			fmt.Println("Input file does not exist: ", file)
			os.Exit(1)
		}
	}

	Compile(inputFiles, outputFile)
}

func PrintHelp() {
	fmt.Println("Usage: cdlc [options] <input files>")
	fmt.Println()
	fmt.Println("Options:")
	fmt.Println("  -o <file>  Specify output file")
}

func Compile(inputFiles []string, outputFile string) {
	var units []CompilerUnit

	for _, file := range inputFiles {
		// get source code
		source := GetSource(file)

		// Lex source code
		tokens := LexSource(source)

		// ...

		// Parse attributes
		attributes := ParseAttributes(tokens)

		units = append(units, CompilerUnit{file, tokens, attributes})
	}

	// print tokens
	// for _, fileToken := range units {
	// 	for _, token := range fileToken.Tokens {
	// 		fmt.Printf("%s: %s \"%s\"\n", fileToken.FileName, TokenTypes[token.Type], token.Value)
	// 	}

	// 	for _, attribute := range fileToken.Attributes {
	// 		fmt.Printf("%s: Attribute %s = %s\n", fileToken.FileName, attribute.Name, attribute.Value)
	// 	}
	// }

	// lets convert our attributes to a map and json serialize it

	var attributes map[string]interface{} = make(map[string]interface{})
	for _, unit := range units {
		for _, attribute := range unit.Attributes {
			if attribute.Type == AttributeTypeString {
				attributes[attribute.Name] = attribute.Value.(string)
			} else if attribute.Type == AttributeTypeNumber {
				i, err := strconv.Atoi(attribute.Value.(string))
				if err != nil {
					fmt.Println("Error converting attribute value to number: ", err)
					os.Exit(1)
				}
				attributes[attribute.Name] = i
			}
		}
	}

	var serialed []byte

	serialed, err := json.Marshal(attributes)
	if err != nil {
		fmt.Println("Error serializing attributes: ", err)
		os.Exit(1)
	}

	// write to output file
	err = os.WriteFile(outputFile, serialed, 0644)
	if err != nil {
		fmt.Println("Error writing output file: ", err)
		os.Exit(1)
	}
}

func GetSource(file string) string {
	content, err := os.ReadFile(file)
	if err != nil {
		fmt.Println("Error reading file: ", file)
		os.Exit(1)
	}

	return string(content)
}

const (
	LexStateDefault         = 10
	LexStateInStringLiteral = 20
	LexStateInNumberLiteral = 30
	LexStateInComment       = 40
	LexStateInIdentifier    = 50
)

const (
	LexStateModifierNone           = 10
	LexStateModifierLiteralEscaped = 20
)

func LexSource(source string) []Token {
	var tokens []Token
	var tempChars string
	var i int = 0
	var state int = LexStateDefault
	var modifier int = LexStateModifierNone

	// we will use a state machine to parse the source code

	for i < len(source) {
		switch state {
		case LexStateDefault:
			// check for single line comment
			if len(source)-i >= 2 && source[i:i+2] == "//" {
				i += 2

				for i < len(source) && source[i] != '\n' {
					i++
				}
				continue
			}

			// check for multi line comment
			if len(source)-i >= 2 && source[i:i+2] == "/*" {
				state = LexStateInComment
				i += 2
				continue
			}

			// check for separator
			var found bool = false
			for _, sep := range LexSeparators {
				if rune(source[i]) == sep {
					tokens = append(tokens, Token{TokenTypeSeparator, string(source[i])})
					i++
					found = true
					break
				}
			}

			if found {
				continue
			}

			// check for operator
			found = false
			for _, op := range LexOperators {
				if len(source)-i >= len(op) && source[i:i+len(op)] == op {
					tokens = append(tokens, Token{TokenTypeOperator, op})
					i += len(op)
					found = true
					break
				}
			}
			if found {
				continue
			}

			// check for number literal
			if source[i] >= '0' && source[i] <= '9' {
				state = LexStateInNumberLiteral
				tempChars += string(source[i])
				i++
				continue
			}

			// check for string literal
			if source[i] == '"' {
				state = LexStateInStringLiteral
				i++
				continue
			}

			// check for keyword
			found = false
			for _, keyword := range LexKeywords {
				if len(source)-i >= len(keyword) && source[i:i+len(keyword)] == keyword {
					found = false

					// keyword must be followed by a separator or whitespace
					if len(source)-i > len(keyword) && source[i+len(keyword)] != ' ' && source[i+len(keyword)] != '\n' && source[i+len(keyword)] != '\t' {
						found = true
					}

					if !found {
						for _, sep := range LexSeparators {
							if len(source)-i >= len(keyword)+1 && rune(source[i+len(keyword)]) == sep {
								found = true
								break
							}
						}
					}

					if found {
						tokens = append(tokens, Token{TokenTypeKeyword, keyword})
						i += len(keyword)
						break
					}

				}
			}
			if found {
				continue
			}

			// check for identifier
			found = false
			for _, char := range LexIndentifierChars {
				if rune(source[i]) == char {
					state = LexStateInIdentifier
					tempChars += string(source[i])
					i++
					found = true
					break
				}
			}
			if found {
				continue
			}

		case LexStateInStringLiteral:
			if modifier == LexStateModifierLiteralEscaped {
				tempChars += string(source[i])
				modifier = LexStateModifierNone
				i++
				continue
			} else if source[i] == '\\' {
				modifier = LexStateModifierLiteralEscaped
				i++
				continue
			} else if source[i] == '"' {
				tokens = append(tokens, Token{TokenTypeLiteralString, tempChars})
				tempChars = ""
				state = LexStateDefault
				i++
				continue
			} else {
				tempChars += string(source[i])
				i++
				continue
			}
		case LexStateInComment:
			if len(source)-i >= 2 && source[i:i+2] == "*/" {
				state = LexStateDefault
				i += 2
				continue
			} else {
				i++
				continue
			}
		case LexStateInNumberLiteral:
			if source[i] >= '0' && source[i] <= '9' {
				tempChars += string(source[i])
				i++
				continue
			} else {
				tokens = append(tokens, Token{TokenTypeLiteralNumber, tempChars})
				tempChars = ""
				state = LexStateDefault
				continue
			}
		case LexStateInIdentifier:
			found := false
			for _, char := range LexIndentifierChars {
				if rune(source[i]) == char {
					tempChars += string(source[i])
					i++
					found = true
					break
				}
			}

			if found {
				continue
			}

			tokens = append(tokens, Token{TokenTypeIdentifier, tempChars})
			tempChars = ""
			state = LexStateDefault
			continue
		}
		i++
	}

	return tokens
}

func ParseAttributes(tokens []Token) []Attribute {
	var attributes []Attribute
	var i int = 0

	// attributes be like this:
	// #[NAME VALUE]
	//

	for i < len(tokens) {
		if tokens[i].Type != TokenTypeOperator || tokens[i].Value != "#" {
			i++
			continue
		}

		// [
		if i+1 >= len(tokens) || tokens[i+1].Type != TokenTypeSeparator || tokens[i+1].Value != "[" {
			i++
			continue
		}

		// NAME
		if i+2 >= len(tokens) || tokens[i+2].Type != TokenTypeIdentifier {
			i++
			continue
		}

		// VALUE
		if i+3 >= len(tokens) || (tokens[i+3].Type != TokenTypeLiteralString && tokens[i+3].Type != TokenTypeLiteralNumber) {
			i++
			continue
		}

		// ]
		if i+4 >= len(tokens) || tokens[i+4].Type != TokenTypeSeparator || tokens[i+4].Value != "]" {
			i++
			continue
		}

		if tokens[i+3].Type == TokenTypeLiteralString {
			attributes = append(attributes, Attribute{tokens[i+2].Value, AttributeTypeString, tokens[i+3].Value})
		} else {
			attributes = append(attributes, Attribute{tokens[i+2].Value, AttributeTypeNumber, tokens[i+3].Value})
		}

		i += 5
	}

	return attributes
}
