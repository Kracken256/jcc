package main

import (
	"crypto/sha256"
	"encoding/json"
	"fmt"
	"os"
	"strings"

	"gopkg.in/yaml.v1"
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
	TokenTypeMeta          = 90
)

var TokenTypes = map[int]string{
	TokenTypeKeyword:       "Keyword",
	TokenTypeLiteralNumber: "LiteralNumber",
	TokenTypeLiteralString: "LiteralString",
	TokenTypeIdentifier:    "Identifier",
	TokenTypeOperator:      "Operator",
	TokenTypeComment:       "Comment",
	TokenTypeSeparator:     "Separator",
	TokenTypeMeta:          "Meta",
}

var LexSeparators = []rune{
	'(', ')', '{', '}', '[', ']', ';', ',', ':',
}

var LexOperators = []string{
	"::", // namespace

	"+=",  // plus equals
	"-=",  // minus equals
	"*=",  // times equals
	"/=",  // floating divide equals
	"%=",  // modulus equals
	"//=", // floor divide equals

	"^^=",  // xor equals
	"||=",  // or equals
	"&&=",  // and equals
	"<<=",  // left shift equals
	">>=",  // arithmetic right shift equals
	">>>=", // unsigned right shift equals
	"|=",   // bitwise or equals
	"&=",   // bitwise and equals
	"^=",   // bitwise xor equals

	"<<", // left shift
	">>", // right shift
	"==", // equals
	"!=", // not equals
	"&&", // and
	"||", // or
	"^^", // xor
	"<=", // less than or equal
	">=", // greater than or equal
	"<",  // less than
	">",  // greater than

	"=", // assign
	"??",
	"@",

	"//", // floor divide
	"++", // increment
	"--", // decrement
	"+",  // plus
	"-",  // minus
	"*",  // times
	"&",  // bitwise and
	"|",  // bitwise or
	"^",  // bitwise xor
	"~",  // bitwise not
	"!",  // not

	"?",      // ternary
	"#",      // preprocessor
	".",      // member access
	",",      // comma
	"new",    // dynamic allocation
	"delete", // dynamic deallocation

}

var LexKeywords = []string{
	"namemap",
	"namespace",
	"using",
	"export",
	"seal",
	"unseal",
	"class",
	"struct",
	"union",
	"typedef",
	"public",
	"private",
	"protected",
	"claim",
	"virtual",
	"abstract",
	"volatile",
	"const",
	"enum",
	"static_map",
	"explicit",
	"extern",
	"friend",
	"operator",
	"this",
	"constructor",
	"destructor",
	"metaclass",
	"metatype",
	"metamap",
	"sizeof",
	"if",
	"else",
	"for",
	"while",
	"do",
	"switch",
	"return",
	"fault",
	"case",
	"break",
	"default",
	"abort",
	"throw",
	"continue",
	"intn",
	"uintn",
	"float",
	"double",
	"int",
	"signed",
	"unsigned",
	"long",
	"bool",
	"bit",
	"char",
	"void",
	"auto"}

var LexIndentifierChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"

type Attribute struct {
	Name  string
	Type  int
	Value interface{}
}

type Token struct {
	Type  int    `json:"type"`
	Value string `json:"value"`
}

type DebugToken struct {
	Type  string `json:"type"`
	Value string `json:"value"`
}

type CompilerUnit struct {
	FileName   string
	Tokens     []Token
	Attributes []Attribute
	Tree       SyntaxTree
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

		// preprocess source code
		preprocessedSource := PreprocessSource(source)

		// debug
		fmt.Println("==================================================================")
		fmt.Println("Preprocessed source:")
		fmt.Println("==================================================================")
		fmt.Println(preprocessedSource)
		fmt.Println("==================================================================")

		// Lex source code
		tokens := LexSource(preprocessedSource)

		// Build abstract syntax tree
		tree, err := BuildSyntaxTree(tokens)
		if err != nil {
			fmt.Println("Error building syntax tree: ", err)
			os.Exit(1)
		}

		// Parse attributes
		attributes := ParseAttributes(tokens)

		units = append(units, CompilerUnit{file, tokens, attributes, tree})
	}

	// Okay, we have the AST, now evaluate metacode
	for _, unit := range units {
		newtree, err := MetaTreeEval(unit.Tree)
		if err != nil {
			fmt.Println("Error evaluating metacode: ", err)
			os.Exit(1)
		}

		// update the tree
		unit.Tree = newtree
	}

	// Optimize the AST
	for _, unit := range units {
		newtree, err := OptimizeSyntaxTree(unit.Tree, 0)
		if err != nil {
			fmt.Println("Error optimizing syntax tree: ", err)
			os.Exit(1)
		}

		// update the tree
		unit.Tree = newtree
	}

	var srcCodeBlob string

	// Generate code
	for _, unit := range units {
		srcCode, err := GenerateCode(unit.Tree)
		if err != nil {
			fmt.Println("Error generating code: ", err)
			os.Exit(1)
		}

		srcCodeBlob += srcCode

		srcCodeBlob += fmt.Sprintf("\n\n///==================================================================\n/// File: %s\n///==================================================================\n\n", unit.FileName)
	}

	fmt.Print(srcCodeBlob)

	///================================================================================================
	// Debug stuff

	// lets convert our attributes to a map and json serialize it
	var tokens []DebugToken

	for _, unit := range units {
		for _, token := range unit.Tokens {
			tokens = append(tokens, DebugToken{strings.ToLower(TokenTypes[token.Type]), token.Value})
		}
	}

	file, err := os.Create(outputFile)
	if err != nil {
		fmt.Println("Error creating output file: ", err)
		os.Exit(1)
	}

	enc := json.NewEncoder(file)
	enc.SetEscapeHTML(false)
	enc.SetIndent("", "  ")

	err = enc.Encode(tokens)
	if err != nil {
		fmt.Println("Error serializing attributes: ", err)
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
	LexStateDefault             = 10
	LexStateInStringLiteral     = 20
	LexStateInNumberLiteral     = 30
	LexStateInSingleLineComment = 40
	LexStateInMultiLineComment  = 50
	LexStateInIdentifier        = 60
	LexStateInMeta              = 70
)

const (
	LexStateModifierNone           = 10
	LexStateModifierLiteralEscaped = 20
)

func CalculateBuildHash(src []byte) string {
	hash := sha256.Sum256(src)
	return fmt.Sprintf("J-%x", hash[:][0:8])
}

func PreprocessSource(source string) string {
	var newSource string = source

	fmt.Printf("Initial JSRCHASH: %s\n", CalculateBuildHash([]byte(newSource)))

	// lets minify the code
	// We will lex and rejoin the tokens
	tokens := LexSource(newSource)

	newSource = ""
	// rejoing tokens
	for i, token := range tokens {
		switch token.Type {
		case TokenTypeComment:
			continue
		case TokenTypeSeparator:
			newSource += token.Value
		case TokenTypeOperator:
			newSource += token.Value
		case TokenTypeKeyword:
			// add space if next token is not a separator
			if len(tokens)-1 > i && tokens[i+1].Type != TokenTypeSeparator {
				newSource += token.Value + " "
			} else {
				newSource += token.Value
			}
		case TokenTypeIdentifier:
			// add space if next token is not a separator
			if len(tokens)-1 > i && tokens[i+1].Type != TokenTypeSeparator {
				newSource += token.Value + " "
			} else {
				newSource += token.Value
			}
		case TokenTypeLiteralNumber:
			newSource += token.Value
		case TokenTypeLiteralString:
			newSource += "\"" + token.Value + "\""
		case TokenTypeMeta:
			/// TODO: include files,tags,etc
			continue
		default:
			fmt.Println("Invalid token type: ", token.Type)
			os.Exit(1)
		}
	}

	// lets wrap the code in the JXX:: namespace if it is not already
	if !strings.HasPrefix(newSource, "namespace JXX{") {
		newSource = "namespace JXX{" + newSource + "}"
	}

	fmt.Printf("Final JSRCHASH: %s\n", CalculateBuildHash([]byte(newSource)))

	return newSource
}

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
			// check for meta. Meta begins with a #
			if source[i] == '#' {
				state = LexStateInMeta
				tempChars += string(source[i])
				i++
				continue
			}

			// check for single line comment
			if len(source)-i >= 2 && source[i:i+2] == "//" {
				i += 2
				state = LexStateInSingleLineComment
				continue
			}

			// check for multi line comment
			if len(source)-i >= 2 && source[i:i+2] == "/*" {
				state = LexStateInMultiLineComment
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
					tokens = append(tokens, Token{TokenTypeKeyword, keyword})
					i += len(keyword)
					found = true
					break
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

			// check for whitespace
			if source[i] == ' ' || source[i] == '\t' || source[i] == '\r' || source[i] == '\n' {
				i++
				continue
			}

			// if we get here, we have an invalid character
			fmt.Println("Lex error: Invalid token: ", string(source[i]))
			// Dump tokens
			for _, token := range tokens {
				fmt.Println(TokenTypes[token.Type], ":", token.Value)
			}
			os.Exit(1)

		case LexStateInMeta:
			if source[i] == '\n' {
				state = LexStateDefault
				tokens = append(tokens, Token{TokenTypeMeta, strings.TrimSpace(tempChars)})
				tempChars = ""
				i++
				continue
			} else {
				tempChars += string(source[i])
				i++
				continue
			}

		case LexStateInStringLiteral:
			if modifier == LexStateModifierLiteralEscaped {
				tempChars += string(source[i])
				modifier = LexStateModifierNone
				i++
				continue
			} else if source[i] == '\\' && len(source)-i >= 2 && source[i+1] == '"' {
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
		case LexStateInSingleLineComment:
			if source[i] != '\n' {
				tempChars += string(source[i])
				i++
				continue

			} else if source[i] == '\n' || i == len(source)-1 {
				state = LexStateDefault
				tokens = append(tokens, Token{TokenTypeComment, strings.TrimSpace(tempChars)})
				tempChars = ""
				i++
				continue
			}

		case LexStateInMultiLineComment:
			if len(source)-i >= 2 && source[i:i+2] == "*/" {
				state = LexStateDefault
				tokens = append(tokens, Token{TokenTypeComment, strings.TrimSpace(tempChars)})
				i += 2
				continue
			} else {
				tempChars += string(source[i])
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

		default:
			fmt.Println("Invalid state: ", state)
			os.Exit(1)
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

type SyntaxNode struct {
	Type       int
	Value      string
	Attributes []Attribute
	Children   []SyntaxNode
}

func (n *SyntaxNode) AddChild(type_ int, value string, attributes []Attribute) {
	if n.Children == nil {
		n.Children = make([]SyntaxNode, 0)
	}

	n.Children = append(n.Children, SyntaxNode{type_, value, attributes, nil})
}

func (n *SyntaxNode) AddChildNode(node SyntaxNode) {
	if n.Children == nil {
		n.Children = make([]SyntaxNode, 0)
	}

	n.Children = append(n.Children, node)
}

func (n *SyntaxNode) RemoveChildByIndex(index int) {
	if index < 0 || index >= len(n.Children) {
		return
	}

	n.Children = append(n.Children[:index], n.Children[index+1:]...)
}

func (n *SyntaxNode) CountChildrenRecursive() int {
	var count int = 0

	for _, child := range n.Children {
		count += child.CountChildrenRecursive()
	}

	return count + len(n.Children)
}

type SyntaxTree struct {
	Root SyntaxNode
}

const (
	SyntaxTypeDefault    = 0
	SyntaxTypeStatement  = 10
	SyntaxTypeExpression = 20
	SyntaxTypeMeta       = 30
	SyntaxTypeFunction   = 40
)

func ParseExpression(tokens []Token) (SyntaxNode, error) {
	var node SyntaxNode

	return node, nil
}

func ParseStatement(tokens []Token) (SyntaxNode, error) {
	var node SyntaxNode

	return node, nil
}

func ParseMeta(tokens []Token) (SyntaxNode, error) {
	var node SyntaxNode

	return node, nil
}

func ParseFunction(tokens []Token) (SyntaxNode, error) {
	var node SyntaxNode

	return node, nil
}

func ParseClass(tokens []Token) (SyntaxNode, error) {
	var node SyntaxNode

	return node, nil
}

func ParseStruct(tokens []Token) (SyntaxNode, error) {
	var node SyntaxNode

	return node, nil
}

func ParseUnion(tokens []Token) (SyntaxNode, error) {
	var node SyntaxNode

	return node, nil
}

func ParseEnum(tokens []Token) (SyntaxNode, error) {
	var node SyntaxNode

	return node, nil
}

func ParseNamespace(tokens []Token) (SyntaxNode, error) {
	var node SyntaxNode

	return node, nil
}

func ParseTypedef(tokens []Token) (SyntaxNode, error) {
	var node SyntaxNode

	return node, nil
}

func ParseBlock(tokens []Token) (SyntaxNode, error) {
	var node SyntaxNode

	return node, nil
}

func BuildSyntaxTree(tokens []Token) (SyntaxTree, error) {
	var tree SyntaxTree

	// Do some basic proprocessing

	// remove comments
	for i := 0; i < len(tokens); i++ {
		if tokens[i].Type == TokenTypeComment {
			tokens = append(tokens[:i], tokens[i+1:]...)
			i--
		}
	}

	// the code already is wrapped in the JXX:: namespace
	// lets start parsing

	node, err := ParseBlock(tokens)

	if err != nil {
		return tree, err
	}

	tree.Root = node

	return tree, nil
}

func MetaTreeEval(tree SyntaxTree) (SyntaxTree, error) {
	return tree, nil
}

func OptimizeSyntaxTree(tree SyntaxTree, level int) (SyntaxTree, error) {
	return tree, nil
}

func GenerateCode(tree SyntaxTree) (string, error) {
	// for now we will output the tree in yaml format

	yaml, err := yaml.Marshal(tree)
	if err != nil {
		return "", err
	}

	return string(yaml), nil
}
