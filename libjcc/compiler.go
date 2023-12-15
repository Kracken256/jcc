package main

import (
	"crypto/sha256"
	"errors"
	"fmt"
	"io/ioutil"
	"math"
	"os"
	"regexp"
	"strconv"
	"strings"
	"time"

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
	"global",
	"infer",
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
	"metafunction",
	"meta",
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

var LexBuiltinTypes = []string{
	"intn",   // Minimum width integer
	"uintn",  // Minimum width unsigned integer
	"float",  // 32 bit floating point
	"double", // 64 bit floating point
	"byte",   // 8 bit integer unsigned
	"char",   // 8 bit integer signed
	"word",   // 16 bit integer unsigned
	"short",  // 16 bit integer signed
	"dword",  // 32 bit integer unsigned
	"int",    // 32 bit integer signed
	"qword",  // 64 bit integer unsigned
	"long",   // 64 bit integer signed
	"bool",   // boolean
}

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

type CompilerAST struct {
	File string `json:"file"`
	Tree SyntaxTree
}

func Compile(inputFiles []string, outputFile string) {
	var units []CompilerUnit

	for _, file := range inputFiles {
		// get source code
		source := GetSource(file)

		// preprocess source code
		preprocessedSource, err := PreprocessSource(source)
		if err != nil {
			fmt.Println("Error preprocessing source: ", err)
			os.Exit(1)
		}

		// debug
		fmt.Println("==================================================================")
		fmt.Println("Preprocessed source:")
		fmt.Println("==================================================================")
		fmt.Println(preprocessedSource)
		fmt.Println("==================================================================")

		// Lex source code
		tokens := LexSource(preprocessedSource)

		attributes, tokens, err := LexMetaSource(tokens)
		if err != nil {
			fmt.Println("Error lexing meta source: ", err)
			os.Exit(1)
		}

		// Build abstract syntax tree
		tree, err := BuildSyntaxTree(&tokens)
		if err != nil {
			fmt.Println("Error building syntax tree: ", err)
			os.Exit(1)
		}

		// Evaluate metacode
		tree, err = MetaTreeEval(tree)
		if err != nil {
			fmt.Println("Error evaluating metacode: ", err)
			os.Exit(1)
		}

		// Optimize the AST
		tree, err = OptimizeSyntaxTree(tree, 0)
		if err != nil {
			fmt.Println("Error optimizing syntax tree: ", err)
			os.Exit(1)
		}

		units = append(units, CompilerUnit{file, tokens.tokens, attributes, tree})
	}

	var srcCodeBlob string

	// get date
	date := time.Now().Format("2006-01-02 15:04:05")

	for _, unit := range units {
		srcCode, err := GenerateCode(unit)
		if err != nil {
			fmt.Println("Error generating code: ", err)
			os.Exit(1)
		}

		var fnamePadded string = unit.FileName
		if len(fnamePadded) > 58 {
			fnamePadded = fnamePadded[:55] + "... "
		} else {
			for len(fnamePadded) < 59 {
				fnamePadded += " "
			}
		}

		srcCodeBlob += fmt.Sprintf("//==================================================================//\n"+
			"// Type: JXX Source Code                                            //\n"+
			"// File: %s//\n"+
			"// Date: %s                                        //\n"+
			"//==================================================================//\n\n%s\n"+
			"//==================================================================//\n"+
			"// EOF: %s //\n"+
			"//==================================================================//\n", fnamePadded, date, srcCode, fnamePadded)
	}

	yamlCode, err := ConvertASTToYAML(units)
	if err != nil {
		fmt.Println("Error converting AST to YAML: ", err)
		os.Exit(1)
	}

	ioutil.WriteFile("out.yaml", yamlCode, 0644)

	///================================================================================================
	// Debug stuff
	err = ioutil.WriteFile(outputFile, []byte(srcCodeBlob), 0644)
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
	LexStateModifierEscaped        = 30
)

func CalculateBuildHash(src []byte) string {
	hash := sha256.Sum256(src)
	return fmt.Sprintf("J-%x", hash[:][0:8])
}

func PreprocessSource(source string) (string, error) {
	var newSource string = source + "\n"

	fmt.Printf("Initial JSRCHASH: %s\n", CalculateBuildHash([]byte(newSource)))

	// lets minify the code
	// We will lex and rejoin the tokens
	tokens := LexSource(newSource)

	newSource = ""
	// rejoing tokens
	for i := 0; i < tokens.Len(); i++ {
		var token Token = tokens.PeekAt(i)

		switch token.Type {
		case TokenTypeComment:
			continue
		case TokenTypeSeparator:
			newSource += token.Value
		case TokenTypeOperator:
			newSource += token.Value
		case TokenTypeKeyword:
			// add space if next token is not a separator
			if tokens.Len()-1 > i && tokens.PeekAt(i+1).Type != TokenTypeSeparator {
				newSource += token.Value + " "
			} else {
				newSource += token.Value
			}
		case TokenTypeIdentifier:
			// add space if next token is not a separator
			if tokens.Len()-1 > i && tokens.PeekAt(i+1).Type != TokenTypeSeparator && tokens.PeekAt(i+1).Type != TokenTypeOperator {
				newSource += token.Value + " "
			} else {
				newSource += token.Value
			}
		case TokenTypeLiteralNumber:
			newSource += token.Value
		case TokenTypeLiteralString:
			newSource += "\"" + token.Value + "\""
		case TokenTypeMeta:
			newSource += token.Value
			continue
		default:
			fmt.Println("Invalid token type: ", token.Type)
			os.Exit(1)
		}
	}

	// // lets wrap the code in the JXX:: namespace if it is not already
	// if !strings.HasPrefix(newSource, "namespace JXX{") {
	// 	newSource = "namespace JXX{" + newSource + "}"
	// }

	fmt.Printf("Final JSRCHASH: %s\n", CalculateBuildHash([]byte(newSource)))

	return newSource, nil
}

func LexMetaSource(tokens Tokens) ([]Attribute, Tokens, error) {
	var attributes []Attribute
	var i int = 0

	for i < tokens.Len() {
		var token = tokens.PeekAt(i)

		if token.Type != TokenTypeMeta {
			i++
			continue
		}

		// #[NAME VALUE]

		if len(token.Value) < 3 {
			return nil, tokens, fmt.Errorf("invalid meta token: %s. Expected #[NAME VALUE]", token.Value)
		}

		if token.Value[0] != '#' {
			return nil, tokens, fmt.Errorf("invalid meta token: %s. Expected #", token.Value)
		}

		if token.Value[1] != '[' {
			return nil, tokens, fmt.Errorf("invalid meta token: %s. Expected [", token.Value)
		}

		if token.Value[len(token.Value)-1] != ']' {
			return nil, tokens, fmt.Errorf("invalid meta token: %s. Expected ]", token.Value)
		}

		var name string
		var value string
		var flip bool = true

		for i := 2; i < len(token.Value); i++ {
			if flip {
				if token.Value[i] == ' ' {
					name = token.Value[2:i]
					flip = false
					continue
				}
			} else {
				if token.Value[i] == ' ' {
					return nil, tokens, fmt.Errorf("invalid meta token: %s. Expected ]", token.Value)
				}
				value = token.Value[i : len(token.Value)-1]
				break
			}
		}

		if len(value) > 2 && value[0] == '"' && value[len(value)-1] == '"' {
			attributes = append(attributes, Attribute{name, AttributeTypeString, value})
		} else {
			attributes = append(attributes, Attribute{name, AttributeTypeNumber, value})
		}

		// remove the meta token
		// tokens = append(tokens[:i], tokens[i+1:]...)
		tokens.PopAt(i)
	}

	return attributes, tokens, nil
}

const (
	NumberLiteralFormatInvalid         = 0
	NumberLiteralFormatHex             = 10
	NumberLiteralFormatBinary          = 20
	NumberLiteralFormatOctal           = 30
	NumberLiteralFormatDecimal         = 40
	NumberLiteralFormatDecimalExplicit = 50
	NumberLiteralFormatFloat           = 60
)

func CheckNumberFormat(value string) int {
	// number formats:
	// 0xAbD10, -0xAbD10
	// 0b1010100010, -0b1010100010
	// 0o1234567, -0o1234567
	// 1234567890, -1234567890
	// 0d1234567890, -0d1234567890
	// 1.234567890e10, -1.234567890e10

	// check for hex
	regexpHex := regexp.MustCompile(`^0x[0-9a-fA-F]+$`)
	regexpBinary := regexp.MustCompile(`^0b[01]+$`)
	regexpOctal := regexp.MustCompile(`^0o[0-7]+$`)
	regexpDecimal := regexp.MustCompile(`^[0-9]+$`)
	regexpDecimalExplicit := regexp.MustCompile(`^0d[0-9]+$`)
	regexpFloat := regexp.MustCompile(`^([0-9]+(\.[0-9]+)?)?(e[+-]?([0-9]+(\.?[0-9]+)?)+)*$`)

	if len(value) < 1 {
		return NumberLiteralFormatInvalid
	}

	if value[0] == '-' {
		value = value[1:]
	}

	if len(value) < 1 {
		return NumberLiteralFormatInvalid
	}

	if regexpHex.MatchString(value) {
		return NumberLiteralFormatHex
	} else if regexpBinary.MatchString(value) {
		return NumberLiteralFormatBinary
	} else if regexpOctal.MatchString(value) {
		return NumberLiteralFormatOctal
	} else if regexpDecimalExplicit.MatchString(value) {
		return NumberLiteralFormatDecimalExplicit
	} else if regexpDecimal.MatchString(value) {
		return NumberLiteralFormatDecimal
	} else if regexpFloat.MatchString(value) {
		return NumberLiteralFormatFloat
	}

	return NumberLiteralFormatInvalid
}

func CheckIfValidNumericLiteral(value string) bool {
	return CheckNumberFormat(value) != NumberLiteralFormatInvalid
}

func ConvertNumbericLiteral(value string) (string, error) {
	var cxxString string
	var numtype int = CheckNumberFormat(value)

	if len(value) < 1 {
		return "", errors.New("invalid number literal")
	}

	if value[0] == '-' {
		cxxString = "-"
		value = value[1:]
	}

	switch numtype {
	case NumberLiteralFormatHex:
		cxxString += value
	case NumberLiteralFormatBinary:
		// convert to hex
		var val uint64
		for i := 2; i < len(value); i++ {
			if value[i] == '1' {
				val |= 1 << (len(value) - i - 1)
			}
		}
		cxxString += fmt.Sprintf("0x%x", val)
	case NumberLiteralFormatOctal:
		// convert to hex
		var val uint64
		for i := len(value) - 1; i >= 2; i-- {
			val += uint64(value[i]-'0') << uint64(3*(len(value)-i-1))
		}
		cxxString += fmt.Sprintf("0x%x", val)
	case NumberLiteralFormatDecimal:
		cxxString += value
	case NumberLiteralFormatDecimalExplicit:
		cxxString += value[2:]
	case NumberLiteralFormatFloat:
		if strings.Contains(value, "e") {
			var mantissaFloat float64
			var expFloat float64
			var err error

			eInx := strings.Index(value, "e")
			if eInx > 0 {
				mantissaFloat, err = strconv.ParseFloat(value[:eInx], 64)
				if err != nil {
					return "", err
				}
			}

			expFloat, err = strconv.ParseFloat(value[eInx+1:], 64)
			if err != nil {
				return "", err
			}

			cxxString += fmt.Sprintf("%f", mantissaFloat*math.Pow(10, expFloat))
		} else {
			cxxString += value
		}

		// The numbers may be xx.00000000
		// if so, we can convert to an integer
		var regexpSimple = regexp.MustCompile(`^\d*.0+$`)
		if regexpSimple.MatchString(cxxString) {
			fmt.Printf("Simple float: %s\n", cxxString)
			dotInx := strings.Index(cxxString, ".")
			if dotInx > 0 {
				cxxString = cxxString[:dotInx]
			}
		}

	}

	return cxxString, nil
}

func ExtractStringLiteral(value string) (string, error) {
	var state int = LexStateDefault
	var i int = 0
	var extracted string

	for i < len(value) {
		if state == LexStateModifierLiteralEscaped {
			extracted += string(value[i])
			state = LexStateDefault
			i++
			continue
		} else if value[i] == '\\' && len(value)-i >= 2 && value[i+1] == '"' {
			state = LexStateModifierLiteralEscaped
			i++
			continue
		} else if value[i] == '"' {
			return extracted, nil
		} else {
			extracted += string(value[i])
			i++
			continue
		}
	}

	return "", errors.New("invalid string literal")
}

func LexSource(source string) Tokens {
	var tokens Tokens
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
					tokens.PushBack(Token{TokenTypeSeparator, string(source[i])})
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
					tokens.PushBack(Token{TokenTypeOperator, op})
					i += len(op)
					found = true
					break
				}
			}
			if found {
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
					tokens.PushBack(Token{TokenTypeKeyword, keyword})
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
			for j, char := range LexIndentifierChars {
				// first character must be a letter or underscore
				if j == 0 && (source[i] < 'a' || source[i] > 'z') && (source[i] < 'A' || source[i] > 'Z') && source[i] != '_' {
					break
				}

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

			// check for number literal
			if (source[i] >= '0' && source[i] <= '9') || source[i] == '-' || source[i] == '+' || source[i] == 'e' {
				state = LexStateInNumberLiteral
				tempChars += string(source[i])
				i++
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
			// for _, token := range tokens {
			// 	fmt.Println(TokenTypes[token.Type], ":", token.Value)
			// }
			for i < tokens.Len() {
				fmt.Println(TokenTypes[tokens.PeekAt(i).Type], ":", tokens.PeekAt(i).Value)
				i++
			}
			os.Exit(1)

		case LexStateInMeta:
			if source[i] == ']' && modifier != LexStateModifierEscaped {
				// detect format error

				// verify name
				if len(tempChars) < 3 || tempChars[0] != '#' || tempChars[1] != '[' {
					fmt.Println("Lex error: Invalid meta token: ", tempChars)
					os.Exit(1)
				}

				var name string

				for i := 2; i < len(tempChars); i++ {
					if tempChars[i] == ' ' {
						name = tempChars[2:i]
						break
					}
				}

				// name must be at least 1 character and must be a valid uppercase identifier
				if len(name) < 1 {
					fmt.Printf("Lex error: Invalid meta token: %s. Expected name\n", tempChars)
					os.Exit(1)
				}

				if strings.HasPrefix(name, "JXX_") {
					fmt.Printf("Lex error: Invalid name in meta token: %s. Name cannot start with JXX_\n", tempChars)
					os.Exit(1)
				}

				var uppercaseIds = strings.ToUpper(LexIndentifierChars)

				for _, char := range name {
					var valid bool = false
					for _, idChar := range uppercaseIds {
						if char == idChar {
							valid = true
							break
						}
					}
					if !valid {
						fmt.Printf("Lex error: Invalid name in meta token: %s. Expected uppercase identifier\n", tempChars)
						os.Exit(1)
					}
				}

				if strings.Contains(tempChars[len(name)+3:], "\"") {

					_, err := ExtractStringLiteral(tempChars[len(name)+3:])
					if err != nil {
						fmt.Printf("Lex error: Invalid value in meta token: %s. Expected string literal\n", tempChars)
						os.Exit(1)
					}
				} else {
					// check number format
					if !CheckIfValidNumericLiteral(tempChars[len(name)+3:]) {
						fmt.Printf("Lex error: Invalid value in meta token: %s. Expected number literal\n", tempChars)
						os.Exit(1)
					}

					// convert number literal
					converted, err := ConvertNumbericLiteral(tempChars[len(name)+3:])
					if err != nil {
						fmt.Printf("Lex error: Failed to convert number literal: %s\n", err)
						os.Exit(1)
					}

					tempChars = tempChars[:len(name)+3] + converted
				}

				tokens.PushBack(Token{TokenTypeMeta, strings.TrimSpace(tempChars) + "]"})
				tempChars = ""
				state = LexStateDefault
				modifier = LexStateModifierNone
				i++
				continue
			} else if source[i] == '"' && modifier != LexStateModifierEscaped {
				tempChars += "\""
				modifier = LexStateModifierEscaped
				i++
				continue
			} else if source[i] == '"' && modifier == LexStateModifierEscaped {
				tempChars += "\""
				modifier = LexStateModifierNone
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
				tokens.PushBack(Token{TokenTypeLiteralString, tempChars})
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
			} else {
				state = LexStateDefault
				tokens.PushBack(Token{TokenTypeComment, strings.TrimSpace(tempChars)})
				tempChars = ""
				i++
				continue
			}

		case LexStateInMultiLineComment:
			if len(source)-i >= 2 && source[i:i+2] == "*/" {
				state = LexStateDefault
				tokens.PushBack(Token{TokenTypeComment, strings.TrimSpace(tempChars)})
				i += 2
				continue
			} else {
				tempChars += string(source[i])
				i++
				continue
			}
		case LexStateInNumberLiteral:
			if len(tempChars) == 1 {
				tempChars += string(source[i])
				i++
				continue
			}

			switch tempChars[0] {
			case '0':
				if len(tempChars) >= 2 && tempChars[1] == 'x' {
					if source[i] >= '0' && source[i] <= '9' || source[i] >= 'a' && source[i] <= 'f' || source[i] >= 'A' && source[i] <= 'F' {
						tempChars += string(source[i])
						i++
						continue
					} else if len(tempChars) == 2 {
						fmt.Println("Lex error: Invalid hex number literal: ", tempChars)
						os.Exit(1)
						continue
					} else {
						tokens.PushBack(Token{TokenTypeLiteralNumber, tempChars})
						tempChars = ""
						state = LexStateDefault
						continue
					}
				} else if len(tempChars) >= 2 && tempChars[1] == 'b' {
					if source[i] == '0' || source[i] == '1' {
						tempChars += string(source[i])
						i++
						continue
					} else if len(tempChars) == 2 {
						fmt.Println("Lex error: Invalid binary number literal: ", tempChars)
						os.Exit(1)
						continue
					} else {
						tokens.PushBack(Token{TokenTypeLiteralNumber, tempChars})
						tempChars = ""
						state = LexStateDefault
						continue
					}
				} else if len(tempChars) >= 2 && tempChars[1] == 'o' {
					if source[i] >= '0' && source[i] <= '7' {
						tempChars += string(source[i])
						i++
						continue
					} else {
						fmt.Println("Lex error: Invalid octal number literal: ", tempChars)
						os.Exit(1)
						continue
					}
				} else if len(tempChars) >= 2 && tempChars[1] == 'd' {
					if source[i] >= '0' && source[i] <= '9' {
						tempChars += string(source[i])
						i++
						continue
					} else {
						fmt.Println("Lex error: Invalid decimal number literal: ", tempChars)
						os.Exit(1)
						continue
					}
				} else if source[i] >= '0' && source[i] <= '9' || source[i] == '.' {
					tempChars += string(source[i])
					i++
					continue
				} else {
					fmt.Println("Lex error: Invalid number literal: ", tempChars)
					os.Exit(1)
					continue
				}
			case '1', '2', '3', '4', '5', '6', '7', '8', '9':
				if source[i] >= '0' && source[i] <= '9' || source[i] == '.' || source[i] == 'e' {
					tempChars += string(source[i])
					i++
					continue
				} else {
					fmt.Println("Lex error: Invalid number literal: ", tempChars)
					os.Exit(1)
					continue
				}
			default:
				fmt.Println("Lex error: Invalid number literal: (default)", tempChars)
				os.Exit(1)
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

			tokens.PushBack(Token{TokenTypeIdentifier, tempChars})
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

type SyntaxNode struct {
	Type       int          `json:"type"`
	Value      string       `json:"value"`
	Attributes []Attribute  `json:"attributes"`
	Children   []SyntaxNode `json:"children"`
}

func (n *SyntaxNode) AddChild(type_ int, value string, attributes []Attribute) *SyntaxNode {
	if n.Children == nil {
		n.Children = make([]SyntaxNode, 0)
	}

	n.Children = append(n.Children, SyntaxNode{type_, value, attributes, nil})

	return &n.Children[len(n.Children)-1]
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
	Root SyntaxNode `json:"root"`
}

const (
	SyntaxTypeNamespaceBlock = 10
	SyntaxTypeFunction       = 20
	SyntaxTypeClass          = 30
	SyntaxTypeStruct         = 40
	SyntaxTypeUnion          = 50
	SyntaxTypeEnum           = 60
	SyntaxTypeTypedef        = 70
	SyntaxTypeNamemap        = 80
	SyntaxTypeExpression     = 90
	SyntaxTypeStatement      = 100
	SyntaxTypeLValue         = 110
	SyntaxTypeRValue         = 120
	SyntaxTypeType           = 130
	SyntaxTypeIdentifer      = 140
	SyntaxTypeNumberLiteral  = 150
	SyntaxTypeStringLiteral  = 160
	SyntaxTypeOperator       = 170
)

///================================================================================================
/// Util
///================================================================================================

func findClosingBracket(tokens Tokens, startIndex int) (int, error) {
	stack := make([]Token, 0)

	opening := map[string]string{
		"{": "}",
		"[": "]",
		"(": ")",
	}

	closing := map[string]bool{
		"}": true,
		"]": true,
		")": true,
	}

	for i := startIndex; i < tokens.Len(); i++ {
		token := tokens.PeekAt(i)

		if _, isOpen := opening[token.Value]; isOpen {
			stack = append(stack, token)
		} else if _, isClose := closing[token.Value]; isClose {
			if len(stack) == 0 {
				return -1, errors.New("closing bracket not found")
			}

			top := stack[len(stack)-1]
			stack = stack[:len(stack)-1]

			if opening[top.Value] != token.Value {
				return -1, errors.New("mismatched closing bracket")
			}

			if len(stack) == 0 {
				return i, nil // Found the matching closing bracket
			}
		}
	}

	return -1, errors.New("closing bracket not found")
}

type Tokens struct {
	tokens []Token
}

func (t *Tokens) Clear() {
	t.tokens = nil
}

func (t *Tokens) String() string {
	return fmt.Sprintf("%v", t.tokens)
}

func (t *Tokens) Len() int {
	return len(t.tokens)
}

func (t *Tokens) PeekFront() *Token {
	return &(t.tokens)[0]
}

func (t *Tokens) PeekBack() *Token {
	return &(t.tokens)[len(t.tokens)-1]
}

func (t *Tokens) PeekAt(index int) Token {
	return (t.tokens)[index]
}

func (t *Tokens) PeekRange(startIndex int, endIndex int) *Tokens {
	if endIndex == -1 {
		endIndex = t.Len()
	}

	return &Tokens{tokens: t.tokens[startIndex:endIndex]}
}

func (t *Tokens) PopAt(index int) Token {
	token := (t.tokens)[index]
	t.tokens = append((t.tokens)[:index], (t.tokens)[index+1:]...)
	return token
}

func (t *Tokens) PopBack() Token {
	var tok Token = (t.tokens)[len(t.tokens)-1]
	t.tokens = t.tokens[:len(t.tokens)-1]
	return tok
}

func (t *Tokens) PopFront() Token {
	var tok Token = (t.tokens)[0]
	t.tokens = t.tokens[1:]
	return tok
}

func (t *Tokens) PopFrontN(n int) Tokens {
	tokens := (t.tokens)[:n]
	t.tokens = (t.tokens)[n:]
	return Tokens{tokens}
}

func (t *Tokens) PopBackN(n int) Tokens {
	tokens := (t.tokens)[len(t.tokens)-n:]
	t.tokens = (t.tokens)[:len(t.tokens)-n]
	return Tokens{tokens}
}

func (t *Tokens) PopRange(startIndex int, endIndex int) Tokens {
	if endIndex == -1 {
		endIndex = t.Len()
	}

	tokens := (t.tokens)[startIndex:endIndex]
	t.tokens = append((t.tokens)[:startIndex], (t.tokens)[endIndex:]...)
	return Tokens{tokens}

}

func (t *Tokens) PushFront(token Token) {
	t.tokens = append([]Token{token}, t.tokens...)
}

func (t *Tokens) PushBack(token Token) {
	t.tokens = append(t.tokens, token)
}

///================================================================================================
/// Checking code types
///================================================================================================

func ParserCheckIsValidExpression(tokens Tokens) (bool, error) {
	// check if literal number
	if tokens.Len() < 1 {
		return false, nil
	}

	fmt.Println("Checking expression: ", tokens.String())

	if tokens.PeekFront().Type == TokenTypeLiteralNumber {
		return true, nil
	}

	return false, nil
}

func ParserCheckIsValidStatement(tokens Tokens) (bool, error) {

	// declare: identifier identifier;
	// assign: identifier = expression;
	// declare and assign: identifier identifier = expression;

	// lets try it

	if tokens.Len() < 3 {
		return false, nil
	}

	// check if first token is an identifier or valid builtin type
	if tokens.PeekFront().Type != TokenTypeIdentifier {
		var found bool = false
		for _, t := range LexBuiltinTypes {
			if tokens.PeekFront().Value == t {
				found = true
				break
			}
		}

		if !found {
			return false, nil
		}
	}

	// check assignmenet
	if tokens.PeekAt(1).Type == TokenTypeOperator && tokens.PeekAt(1).Value == "=" {
		if x, err := ParserCheckIsValidExpression(*tokens.PeekRange(2, -1)); err == nil && x {
			return true, nil
		} else {
			return false, nil
		}
	}

	// declare and assign
	if tokens.PeekAt(1).Type != TokenTypeIdentifier {
		return false, nil
	}

	// verify that the identifier is not a keyword
	var found bool = false
	for _, keyword := range LexKeywords {
		if tokens.PeekAt(1).Value == keyword {
			found = true
			break
		}
	}
	if found {
		return false, nil
	}

	if tokens.PeekAt(2).Type == TokenTypeOperator && tokens.PeekAt(2).Value == "=" {
		if x, err := ParserCheckIsValidExpression(*tokens.PeekRange(3, -1)); err == nil && x {
			return true, nil
		} else {
			return false, nil
		}
	}

	if tokens.PeekAt(2).Type == TokenTypeSeparator && tokens.PeekAt(2).Value == ";" {
		return true, nil
	}

	return false, nil
}

func ParserCheckIsValidMeta(tokens Tokens) (bool, error) {
	return false, nil
}

func ParserCheckIsValidFunction(tokens Tokens) (bool, error) {
	return false, nil
}

func ParserCheckIsValidClass(tokens Tokens) (bool, error) {
	return false, nil
}

func ParserCheckIsValidStruct(tokens Tokens) (bool, error) {
	return false, nil
}

func ParserCheckIsValidUnion(tokens Tokens) (bool, error) {
	return false, nil
}

func ParserCheckIsValidEnum(tokens Tokens) (bool, error) {
	return false, nil
}

func ParserCheckIsValidNamespace(tokens Tokens) (bool, error) {
	return false, nil
}

func ParserCheckIsValidTypedef(tokens Tokens) (bool, error) {
	return false, nil
}

func ParserCheckIsValidForwardDeclare(tokens Tokens) (bool, error) {
	return false, nil
}

func ParserCheckIsValidBlock(tokens Tokens) (bool, error) {
	return false, nil
}

///================================================================================================
/// Parsing
///================================================================================================

func ParseExpression(tokens *Tokens, node *SyntaxNode) error {
	var expression SyntaxNode
	var literal SyntaxNode

	literal.Type = SyntaxTypeNumberLiteral
	literal.Value = tokens.PeekFront().Value

	expression.Type = SyntaxTypeExpression
	expression.Value = "expression"

	expression.AddChildNode(literal)

	node.AddChildNode(expression)

	// consume tokens
	tokens.PopFront()

	if tokens.PeekFront().Type == TokenTypeSeparator && tokens.PeekFront().Value == ";" {
		tokens.PopFront()
	}

	fmt.Println("Tokens after expression: ", tokens.String())

	return nil
}

func ParseStatement(tokens *Tokens, node *SyntaxNode) error {
	var statement SyntaxNode
	var newtype SyntaxNode
	var identifier SyntaxNode

	statement.Type = SyntaxTypeStatement
	statement.Value = "assignment"

	fmt.Println("Tokens before statement: ", tokens.String())

	if tokens.Len() > 3 && tokens.PeekAt(1).Type == TokenTypeOperator && tokens.PeekAt(1).Value == "=" {
		// assignment
		// identifier = expression;
		identifier.Type = SyntaxTypeLValue
		identifier.Value = tokens.PeekFront().Value

		statement.AddChildNode(identifier)

		err := ParseExpression(tokens.PeekRange(2, -1), &statement)
		if err != nil {
			return err
		}

		node.AddChildNode(statement)
		return nil
	} else if tokens.Len() > 3 && tokens.PeekAt(2).Type == TokenTypeOperator && tokens.PeekAt(2).Value == "=" {
		// declare and assignment
		// type identifier = expression;
		newtype.Type = SyntaxTypeType
		newtype.Value = tokens.PeekFront().Value

		identifier.Type = SyntaxTypeIdentifer
		identifier.Value = tokens.PeekAt(1).Value

		statement.AddChildNode(newtype)
		statement.AddChildNode(identifier)

		err := ParseExpression(tokens.PeekRange(3, -1), &statement)
		if err != nil {
			return err
		}
		node.AddChildNode(statement)

		// consume tokens
		tokens.PopFrontN(3)

		fmt.Println("Tokens after expression(statement): ", tokens.String())

		return nil
	} else if tokens.Len() > 2 && tokens.PeekFront().Type == TokenTypeIdentifier && tokens.PeekAt(1).Type == TokenTypeIdentifier && tokens.PeekAt(2).Type == TokenTypeSeparator && tokens.PeekAt(2).Value == ";" {
		// declare
		// type identifier;
		newtype.Type = SyntaxTypeType
		newtype.Value = tokens.PeekFront().Value

		identifier.Type = SyntaxTypeIdentifer
		identifier.Value = tokens.PeekAt(1).Value

		statement.AddChildNode(newtype)
		statement.AddChildNode(identifier)
		node.AddChildNode(statement)

		return nil
	}

	return fmt.Errorf("parsing error: failed to parse statement")
}

func ParseMeta(tokens *Tokens, node *SyntaxNode) error {

	return nil
}

func ParseFunction(tokens *Tokens, node *SyntaxNode) error {

	return nil
}

func ParseClass(tokens *Tokens, node *SyntaxNode) error {

	return nil
}

func ParseStruct(tokens *Tokens, node *SyntaxNode) error {

	return nil
}

func ParseUnion(tokens *Tokens, node *SyntaxNode) error {

	return nil
}

func ParseEnum(tokens *Tokens, node *SyntaxNode) error {

	return nil
}

func ParseNamespace(tokens *Tokens, node *SyntaxNode) error {

	return nil
}

func ParseTypedef(tokens *Tokens, node *SyntaxNode) error {

	return nil
}

func ParseForwardDeclare(tokens *Tokens, node *SyntaxNode) error {

	return nil
}

func ParseBlock(tokens *Tokens, node *SyntaxNode) error {
	if tokens.Len() < 1 {
		return fmt.Errorf("parsing error: failed to parse block. no tokens")
	}

	for tokens.Len() > 0 {
		if x, err := ParserCheckIsValidExpression(*tokens); err == nil && x {
			fmt.Println("Parsing expression")
			if ParseExpression(tokens, node) != nil {
				return fmt.Errorf("parsing error: failed to parse expression")
			}
			continue
		}

		if x, err := ParserCheckIsValidStatement(*tokens); err == nil && x {
			if ParseStatement(tokens, node) != nil {
				return fmt.Errorf("parsing error: failed to parse statement")
			}
			fmt.Println("Tokens after statement: ", tokens.String())
			continue
		}

		if x, err := ParserCheckIsValidMeta(*tokens); err == nil && x {
			if ParseMeta(tokens, node) != nil {
				return fmt.Errorf("parsing error: failed to parse meta")
			}
			continue
		}

		if x, err := ParserCheckIsValidFunction(*tokens); err == nil && x {
			if ParseFunction(tokens, node) != nil {
				return fmt.Errorf("parsing error: failed to parse function")
			}
			continue
		}

		if x, err := ParserCheckIsValidClass(*tokens); err == nil && x {
			if ParseClass(tokens, node) != nil {
				return fmt.Errorf("parsing error: failed to parse class")
			}
			continue
		}

		if x, err := ParserCheckIsValidStruct(*tokens); err == nil && x {
			if ParseStruct(tokens, node) != nil {
				return fmt.Errorf("parsing error: failed to parse struct")
			}
			continue
		}

		if x, err := ParserCheckIsValidUnion(*tokens); err == nil && x {
			if ParseUnion(tokens, node) != nil {
				return fmt.Errorf("parsing error: failed to parse union")
			}
			continue
		}

		if x, err := ParserCheckIsValidEnum(*tokens); err == nil && x {
			if ParseEnum(tokens, node) != nil {
				return fmt.Errorf("parsing error: failed to parse enum")
			}
			continue
		}

		if x, err := ParserCheckIsValidNamespace(*tokens); err == nil && x {
			if ParseNamespace(tokens, node) != nil {
				return fmt.Errorf("parsing error: failed to parse namespace")
			}
			continue
		}

		if x, err := ParserCheckIsValidTypedef(*tokens); err == nil && x {
			if ParseTypedef(tokens, node) != nil {
				return fmt.Errorf("parsing error: failed to parse typedef")
			}
			continue
		}

		if x, err := ParserCheckIsValidForwardDeclare(*tokens); err == nil && x {
			if ParseForwardDeclare(tokens, node) != nil {
				return fmt.Errorf("parsing error: failed to parse forward declare")
			}
			continue
		}

		if x, err := ParserCheckIsValidBlock(*tokens); err == nil && x {
			if ParseBlock(tokens, node) != nil {
				return fmt.Errorf("parsing error: failed to parse block")
			}
			continue
		}
	}

	return nil
}

func BuildSyntaxTree(tokens *Tokens) (SyntaxTree, error) {
	var tree SyntaxTree

	tree.Root.Type = 0
	tree.Root.Value = "root"

	// the code already is wrapped in the JXX:: namespace
	// lets start parsing that block

	fmt.Println("tokens before: ", tokens.String())

	err := ParseBlock(tokens, &tree.Root)

	if err != nil {
		return tree, err
	}

	fmt.Println("tokens after: ", tokens.String())

	return tree, nil
}

func MetaTreeEval(tree SyntaxTree) (SyntaxTree, error) {
	return tree, nil
}

func OptimizeSyntaxTree(tree SyntaxTree, level int) (SyntaxTree, error) {

	// clear children of root node
	// tree.Root.Children = nil
	return tree, nil
}

type SourceCode struct {
	Items []string
}

func GenerateCXXCodeForNode(node SyntaxNode, src *SourceCode) error {
	switch node.Type {
	case SyntaxTypeNamespaceBlock:
		src.Items = append(src.Items, fmt.Sprintf("namespace %s {\n", node.Value))
	case SyntaxTypeStatement:
		{
			var code string

			for _, child := range node.Children {
				switch child.Type {
				case SyntaxTypeType:
					code += child.Value + " "
				case SyntaxTypeIdentifer:
					code += child.Value
				case SyntaxTypeExpression:
					code += fmt.Sprintf(" = %s", child.Children[0].Value)
				}
			}

			src.Items = append(src.Items, fmt.Sprintf("%s;\n", code))
		}
	default:
		return nil
	}

	return nil
}

func GenerateCXXMetaAttributes(attrib []Attribute) (string, error) {
	var code string

	for _, attr := range attrib {
		if attr.Type == AttributeTypeString {
			code += fmt.Sprintf("  constexpr const char *JXX_%s = %s;\n", attr.Name, attr.Value)
		} else {
			code += fmt.Sprintf("  constexpr auto JXX_%s = %s;\n", attr.Name, attr.Value)
		}
	}

	return code, nil
}

func GenerateCXXCode(unit CompilerUnit) (string, error) {
	var code string
	var sourceCode SourceCode

	code = "namespace jxx::globals\n{\n"

	newCode, err := GenerateCXXMetaAttributes(unit.Attributes)
	if err != nil {
		return "", err
	}

	code += newCode

	code += "}\n\nusing namespace jxx::globals;\n"

	// In order to generate hiararchical code, we need to traverse the syntax tree
	// in a depth first manner

	var stack []SyntaxNode = []SyntaxNode{unit.Tree.Root}

	for len(stack) > 0 {
		current := stack[len(stack)-1]
		stack = stack[:len(stack)-1]

		err := GenerateCXXCodeForNode(current, &sourceCode)
		if err != nil {
			return "", err
		}

		for i := len(current.Children) - 1; i >= 0; i-- {
			stack = append(stack, current.Children[i])
		}
	}

	code += strings.Join(sourceCode.Items, "")

	return code, nil
}

func GenerateCode(unit CompilerUnit) (string, error) {
	return GenerateCXXCode(unit)
}

func ConvertASTToYAML(tree []CompilerUnit) ([]byte, error) {
	var m map[string]SyntaxNode = make(map[string]SyntaxNode)

	for _, unit := range tree {
		m[unit.FileName] = unit.Tree.Root
	}

	yaml, err := yaml.Marshal(m)
	if err != nil {
		return nil, err
	}

	return yaml, nil
}

///================================================================================================
/// old
///================================================================================================

// func ParseNamespaceBlock(tokens []Token, root *SyntaxNode) error {
// 	stack := []struct {
// 		tokens []Token
// 		node   *SyntaxNode
// 	}{{
// 		tokens: tokens,
// 		node:   root,
// 	}}

// 	for len(stack) > 0 {
// 		current := stack[len(stack)-1]
// 		stack = stack[:len(stack)-1]

// 		if len(current.tokens) == 0 {
// 			continue // Skip empty token sets
// 		}

// 		offset, err := ParserCheckIsNamespaceBlock(current.tokens)
// 		if err != nil || offset == 0 {
// 			return err
// 		}

// 		newNode := current.node.AddChild(SyntaxTypeNamespaceBlock, current.tokens.PeekAt(1).Value, nil)

// 		// Push the next set of tokens after the current namespace block
// 		stack = append(stack, struct {
// 			tokens []Token
// 			node   *SyntaxNode
// 		}{
// 			tokens: current.tokens[offset+1:],
// 			node:   current.node,
// 		})

// 		// Push the tokens within the namespace block
// 		stack = append(stack, struct {
// 			tokens []Token
// 			node   *SyntaxNode
// 		}{
// 			tokens: current.tokens[3:offset],
// 			node:   newNode,
// 		})
// 	}

// 	return nil
// }

// func ParserCheckIsNamespaceBlock(tokens []Token) (int, error) {
// 	if tokens.Len() < 4 {
// 		return 0, nil
// 	}

// 	// the first token must be a namespace keyword
// 	if tokens[0].Type != TokenTypeKeyword || tokens[0].Value != "namespace" {
// 		return 0, fmt.Errorf("parsing error: failed to parse namespace block. namespace keyword not found")
// 	}

// 	// the second token must be an identifier
// 	if tokens.PeekAt(1).Type != TokenTypeIdentifier {
// 		return 0, fmt.Errorf("parsing error: failed to parse namespace block. namespace identifier not found")
// 	}

// 	// the third token must be a separator
// 	if tokens.PeekAt(2).Type != TokenTypeSeparator || tokens.PeekAt(2).Value != "{" {
// 		return 0, fmt.Errorf("parsing error: failed to parse namespace block. namespace block open separator not found")
// 	}

// 	offset, err := findClosingBracket(tokens, 2)
// 	if err != nil {
// 		return 0, fmt.Errorf("parsing error: failed to parse namespace block. namespace block close separator not found")
// 	}

// 	// the last token must be a separator
// 	if tokens[offset].Type != TokenTypeSeparator || tokens[offset].Value != "}" {
// 		return 0, fmt.Errorf("parsing error: failed to parse namespace block. namespace block close separator not found")
// 	}

// 	return offset, nil
// }
