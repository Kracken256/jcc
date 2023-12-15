#include "lexer.hpp"
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <cstring>
#include <regex>
#include <sstream>
#include <cmath>

///=============================================================================
/// jcc::Token class implementation
///=============================================================================

static std::map<int, std::string> tokenTypeMap = {
    {jcc::TokenType::Unknown, "unknown"},
    {jcc::TokenType::Identifier, "identifier"},
    {jcc::TokenType::Keyword, "keyword"},
    {jcc::TokenType::NumberLiteral, "number_literal"},
    {jcc::TokenType::FloatingPointLiteral, "floating_point_literal"},
    {jcc::TokenType::StringLiteral, "string_literal"},
    {jcc::TokenType::Operator, "operator"},
    {jcc::TokenType::Punctuator, "punctuator"},
    {jcc::TokenType::MultiLineComment, "multiline_comment"},
    {jcc::TokenType::SingleLineComment, "singleline_comment"},
    {jcc::TokenType::Whitespace, "whitespace"},
};

jcc::Token::Token()
{
    m_type = jcc::TokenType::Unknown;
}

jcc::Token::Token(jcc::TokenType type, TokenValueType value)
{
    m_type = type;
    m_value = value;
}

jcc::TokenType jcc::Token::type() const
{
    return m_type;
}

const jcc::TokenValueType &jcc::Token::value() const
{
    return m_value;
}

std::string jcc::Token::to_string() const
{
    switch (this->m_type)
    {
    case TokenType::Identifier:
        return std::string("Identifier(\"" + std::get<std::string>(m_value) + "\")");
        break;
    case TokenType::Keyword:
        return std::string("Keyword(" + std::string(std::get<const char *>(m_value)) + ")");
        break;
    case TokenType::NumberLiteral:
        return std::string("NumberLiteral(" + std::to_string(std::get<uint64_t>(m_value)) + ")");
        break;
    case TokenType::FloatingPointLiteral:
        return std::string("FloatingPointLiteral(" + std::to_string(std::get<double>(m_value)) + ")");
    case TokenType::StringLiteral:
        return std::string("StringLiteral(\"" + std::get<std::string>(m_value) + "\")");
        break;
    case TokenType::Operator:
        return std::string("Operator('" + std::string(std::get<const char *>(m_value)) + "')");
        break;
    case TokenType::Punctuator:
        return std::string("Punctuator('" + std::string(std::get<const char *>(m_value)) + "')");
        break;
    case TokenType::SingleLineComment:
        return std::string("SingleLineComment(\"" + std::get<std::string>(m_value) + "\")");
        break;
    case TokenType::MultiLineComment:
        return std::string("MultiLineComment(\"" + std::get<std::string>(m_value) + "\")");
        break;
    case TokenType::Whitespace:
        return std::string("Whitespace(" + std::get<std::string>(m_value) + ")");
        break;
    default:
        return std::string("Unknown()");
        break;
    }
}

///=============================================================================
/// jcc::TokenList class implementation
///=============================================================================

jcc::TokenList::TokenList()
{
    this->m_tokens = std::vector<Token>();
    this->m_locked = false;
}

jcc::TokenList::TokenList(const std::vector<jcc::Token> &tokens)
{
    this->m_tokens = tokens;
    this->m_locked = false;
}

void jcc::TokenList::push_back(const jcc::Token &token)
{
    if (this->m_locked)
    {
        throw std::runtime_error("Unable to push_back to TokenList, it is locked");
    }

    m_tokens.push_back(token);
}

void jcc::TokenList::push_back(const std::vector<jcc::Token> &tokens)
{
    if (this->m_locked)
    {
        throw std::runtime_error("Unable to push_back vector to TokenList, it is locked");
    }

    m_tokens.insert(m_tokens.end(), tokens.begin(), tokens.end());
}

void jcc::TokenList::lock()
{
    this->m_locked = true;
}

bool jcc::TokenList::is_locked() const
{
    return this->m_locked;
}

const std::vector<jcc::Token> &jcc::TokenList::tokens() const
{
    return this->m_tokens;
}

std::string jcc::TokenList::to_string() const
{
    std::string result = "TokenList(";

    for (size_t i = 0; i < m_tokens.size(); i++)
    {
        if (m_tokens[i].type() == TokenType::Whitespace || m_tokens[i].type() == TokenType::SingleLineComment || m_tokens[i].type() == TokenType::MultiLineComment)
        {
            continue;
        }
        result += m_tokens[i].to_string();

        if (i != m_tokens.size() - 1)
        {
            result += ", ";
        }
    }

    return result + ")";
}

std::string jcc::TokenList::to_json() const
{
    std::string result = "[";
    for (size_t i = 0; i < m_tokens.size(); i++)
    {
        if (m_tokens[i].type() == TokenType::Whitespace || m_tokens[i].type() == TokenType::SingleLineComment || m_tokens[i].type() == TokenType::MultiLineComment)
        {
            continue;
        }

        std::string dataString = "", escapedString = "";
        result += "{";
        result += "\"t\":\"" + tokenTypeMap[static_cast<int>(m_tokens[i].type())] + "\",";

        switch (m_tokens[i].type())
        {
        case TokenType::Identifier:
            dataString = std::get<std::string>(m_tokens[i].value());
            break;
        case TokenType::Keyword:
            dataString = std::string(std::get<const char *>(m_tokens[i].value()));
            break;
        case TokenType::NumberLiteral:
            dataString = std::to_string(std::get<uint64_t>(m_tokens[i].value()));
            break;
        case TokenType::FloatingPointLiteral:
            dataString = std::to_string(std::get<double>(m_tokens[i].value()));
            break;
        case TokenType::StringLiteral:
            dataString = std::get<std::string>(m_tokens[i].value());
            break;
        case TokenType::Operator:
            dataString = std::string(std::get<const char *>(m_tokens[i].value()));
            break;
        case TokenType::Punctuator:
            dataString = std::string(std::get<const char *>(m_tokens[i].value()));
            break;
        case TokenType::SingleLineComment:
            dataString = std::get<std::string>(m_tokens[i].value());
            break;
        case TokenType::MultiLineComment:
            dataString = std::get<std::string>(m_tokens[i].value());
            break;
        case TokenType::Whitespace:
            dataString = std::get<std::string>(m_tokens[i].value());
            break;
        default:
            break;
        }

        for (auto c : dataString)
        {
            switch (c)
            {
            case '\n':
                escapedString += "\\\\n";
                break;
            case '\t':
                escapedString += "\\\\t";
                break;
            case '\r':
                escapedString += "\\\\r";
                break;
            case '\0':
                escapedString += "\\\\0";
                break;
            case '\\':
                escapedString += "\\\\";
                break;
            case '"':
                escapedString += "\\\"";
                break;
            default:
                escapedString += c;
                break;
            }
        }

        result += "\"v\":\"" + escapedString + "\"";

        result += "}";

        if (i != m_tokens.size() - 1)
        {
            result += ",";
        }
    }
    return result + "]";
}

const jcc::Token &jcc::TokenList::operator[](size_t index) const
{
    return m_tokens.at(index);
}

void jcc::TokenList::pop_front()
{
    if (this->m_locked)
    {
        throw std::runtime_error("Unable to pop_front from TokenList, it is locked");
    }

    m_tokens.erase(m_tokens.begin());
}

size_t jcc::TokenList::size() const
{
    return m_tokens.size();
}

std::vector<jcc::Token> &jcc::TokenList::data()
{
    if (this->m_locked)
    {
        throw std::runtime_error("Unable to get data from TokenList, it is locked");
    }

    return m_tokens;
}

///=============================================================================
/// jcc::Lexer class implementation
///=============================================================================

enum class LexerState
{
    Default,
    StringLiteral,
    NumberLiteral,
    SingleLineComment,
    MultiLineComment,
    Identifier,
    Operator,
    Punctuator,
    Whitespace
};

enum class LexerStateModifier
{
    None,
    StringEscape,
    StringSingleQuote,
    StringSingleQuoteEscape
};

static std::vector<std::pair<const char *, unsigned int>> lexPunctuators = {
    {"(", 1},  // left parenthesis
    {")", 1},  // right parenthesis
    {"{", 1},  // left brace
    {"}", 1},  // right brace
    {"[", 1},  // left bracket
    {"]", 1},  // right bracket
    {";", 1},  // semicolon
    {",", 1},  // comma
    {"::", 2}, // scope resolution
    {":", 1},  // colon
};

static std::vector<std::pair<const char *, unsigned int>> lexOperators = {
    // "+=",  // plus equals
    // "-=",  // minus equals
    // "*=",  // times equals
    // "/=",  // floating divide equals
    // "%=",  // modulus equals
    // "//=", // floor divide equals

    // "^^=",  // xor equals
    // "||=",  // or equals
    // "&&=",  // and equals
    // "<<=",  // left shift equals
    // ">>=",  // arithmetic right shift equals
    // ">>>=", // unsigned right shift equals
    // "|=",   // bitwise or equals
    // "&=",   // bitwise and equals
    // "^=",   // bitwise xor equals

    // "<<", // left shift
    // ">>", // right shift
    // "==", // equals
    // "!=", // not equals
    // "&&", // and
    // "||", // or
    // "^^", // xor
    // "<=", // less than or equal
    // ">=", // greater than or equal
    // "<",  // less than
    // ">",  // greater than

    // "=", // assign
    // "??",
    // "@",

    // "//", // floor divide
    // "++", // increment
    // "--", // decrement
    // "+",  // plus
    // "-",  // minus
    // "*",  // times
    // "/",  // floating divide
    // "%",  // modulus
    // "&",  // bitwise and
    // "|",  // bitwise or
    // "^",  // bitwise xor
    // "~",  // bitwise not
    // "!",  // not

    // "?",      // ternary
    // "#",      // preprocessor
    // ".",      // member access
    // ",",      // comma
    // "new",    // dynamic allocation
    // "delete", // dynamic deallocation

    {"+=", 2}, // plus equals
    {"-=", 2}, // minus equals
    {"*=", 2}, // times equals
    {"/=", 2}, // floating divide equals
    {"%=", 2}, // modulus equals

    {"^^=", 3},  // xor equals
    {"||=", 3},  // or equals
    {"&&=", 3},  // and equals
    {"<<=", 3},  // left shift equals
    {">>=", 3},  // arithmetic right shift equals
    {">>>=", 4}, // unsigned right shift equals
    {"|=", 2},   // bitwise or equals
    {"&=", 2},   // bitwise and equals
    {"^=", 2},   // bitwise xor equals

    {"<<", 2}, // left shift
    {">>", 2}, // right shift
    {"==", 2}, // equals
    {"!=", 2}, // not equals
    {"&&", 2}, // and
    {"||", 2}, // or
    {"^^", 2}, // xor
    {"<=", 2}, // less than or equal
    {">=", 2}, // greater than or equal
    {"<", 1},  // less than
    {">", 1},  // greater than

    {"=", 1}, // assign
    {"??", 2},
    {"@", 1},

    {"//", 2}, // floor divide
    {"++", 2}, // increment
    {"--", 2}, // decrement
    {"+", 1},  // plus
    {"-", 1},  // minus
    {"*", 1},  // times
    {"/", 1},  // floating divide
    {"%", 1},  // modulus
    {"&", 1},  // bitwise and
    {"|", 1},  // bitwise or
    {"^", 1},  // bitwise xor
    {"~", 1},  // bitwise not
    {"!", 1},  // not

    {"?", 1},      // ternary
    {"#", 1},      // preprocessor
    {".", 1},      // member access
    {",", 1},      // comma
    {"new", 3},    // dynamic allocation
    {"delete", 6}, // dynamic deallocation
};

static std::set<const char *> lexKeywords = {
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
    "auto"};

static const char lexIdentifierChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
static const size_t lexIdentifierCharsLength = (sizeof(lexIdentifierChars) - 1) / sizeof(char);

enum class NumberLiteralType
{
    Invalid,
    Decimal,
    Hexadecimal,
    Binary,
    Octal,
    Floating,
};

static NumberLiteralType check_number_literal_type(std::string input)
{
    if (input[0] == '-')
    {
        input = input.substr(1);
    }

    if (input.length() < 3)
    {
        if (std::isdigit(input[0]))
        {
            return NumberLiteralType::Decimal;
        }
        else
        {
            return NumberLiteralType::Invalid;
        }
    }

    std::string prefix = input.substr(0, 2);

    if (prefix == "0x")
    {
        for (size_t i = 2; i < input.length(); i++)
        {
            if (!((input[i] >= '0' && input[i] <= '9') || (input[i] >= 'a' && input[i] <= 'f') || (input[i] >= 'A' && input[i] <= 'F')))
            {
                return NumberLiteralType::Invalid;
            }
        }
        return NumberLiteralType::Hexadecimal;
    }
    else if (prefix == "0b")
    {
        for (size_t i = 2; i < input.length(); i++)
        {
            if (!(input[i] == '0' || input[i] == '1'))
            {
                return NumberLiteralType::Invalid;
            }
        }
        return NumberLiteralType::Binary;
    }
    else if (prefix == "0o")
    {
        for (size_t i = 2; i < input.length(); i++)
        {
            if (!(input[i] >= '0' && input[i] <= '7'))
            {
                return NumberLiteralType::Invalid;
            }
        }
        return NumberLiteralType::Octal;
    }
    else if (prefix == "0d")
    {
        for (size_t i = 2; i < input.length(); i++)
        {
            if (!(input[i] >= '0' && input[i] <= '9'))
            {
                return NumberLiteralType::Invalid;
            }
        }
        return NumberLiteralType::Decimal;
    }
    else
    {
        for (size_t i = 0; i < input.length(); i++)
        {
            if (!(input[i] >= '0' && input[i] <= '9'))
            {
                goto test_float;
            }
        }
        return NumberLiteralType::Decimal;
    }

test_float:
    auto regexpFloat = std::regex("^([0-9]+(\\.[0-9]+)?)?(e[+-]?([0-9]+(\\.?[0-9]+)?)+)*$");

    // slow operation
    if (std::regex_match(input, regexpFloat))
    {
        return NumberLiteralType::Floating;
    }

    return NumberLiteralType::Invalid;
}

void normalize_number_literal(std::string &number, size_t column, size_t line)
{
    uint64_t value = 0;
    std::stringstream ss;

    for (size_t i = 0; i < number.length(); i++)
    {
        number[i] = std::tolower(number[i]);
    }

    NumberLiteralType type = check_number_literal_type(number);
    if (type == NumberLiteralType::Invalid)
    {
        throw jcc::LexerExceptionInvalidLiteral("Number literal not valid at line " + std::to_string(line) + ", column " + std::to_string(column));
    }

    switch (type)
    {
    case NumberLiteralType::Hexadecimal:
        break;
    case NumberLiteralType::Binary:
        // convert binary string to uint64_t
        for (size_t i = 2; i < number.length(); ++i)
        {
            // check for overflow
            if (value & 0x8000000000000000)
            {
                throw jcc::LexerExceptionInvalidLiteral("Binary number literal at line " + std::to_string(line) + ", column " + std::to_string(column) + " is too large. Will not fit in 64 bits.");
            }

            value = (value << 1) + (number[i] - '0');
        }

        ss << "0x" << std::hex << value;
        number = ss.str();
        break;
    case NumberLiteralType::Octal:
        for (size_t i = 2; i < number.length(); ++i)
        {
            // check for overflow
            if (value & 0xE000000000000000)
            {
                throw jcc::LexerExceptionInvalidLiteral("Octal number literal at line " + std::to_string(line) + ", column " + std::to_string(column) + " is too large. Will not fit in 64 bits.");
            }

            value = (value << 3) + (number[i] - '0');
        }
        ss << "0x" << std::hex << value;
        number = ss.str();
        break;
    case NumberLiteralType::Decimal:
        break;
    case NumberLiteralType::Floating:
        break;
    default:
        break;
    }
}

/// @brief Lex the source code into a list of tokens
/// @param source JXX source code raw string
/// @param preprocess Whether to preprocess the source code
/// @return A vector of tokens
/// @note This is probably the most complex I have ever written. So expect bugs.
jcc::TokenList jcc::Lexer::lex(const std::string &source, bool preprocess)
{
    /// TODO: implement
    std::string preprocessed = source, current_token;
    TokenList result;
    LexerState state = LexerState::Default;
    LexerStateModifier modifier = LexerStateModifier::None;
    size_t i = 0, src_length, line = 1, column = 1;
    bool found = false;

    if (preprocess)
    {
        preprocessed += "\n";
    }

    src_length = preprocessed.length();

    result.data().reserve(src_length / 3);

    while (i < src_length)
    {
        char current_char = preprocessed[i];

        switch (state)
        {
        case LexerState::Default:
            // Check for single line comment
            if (src_length - i >= 2 && current_char == '/' && preprocessed[i + 1] == '/')
            {
                state = LexerState::SingleLineComment;
                i++;
                column++;
                break;
            }

            // Check for multi line comment
            if (src_length - i >= 2 && current_char == '/' && preprocessed[i + 1] == '*')
            {
                state = LexerState::MultiLineComment;
                i++;
                column++;
                break;
            }

            // Check for operator
            found = false;
            for (const auto &op : lexOperators)
            {
                if (src_length - i >= op.second)
                {
                    switch (op.second)
                    {
                    case 1:
                        if (current_char == op.first[0])
                        {
                            result.push_back(Token(TokenType::Operator, op.first));
                            i += op.second - 1;
                            found = true;
                        }
                        break;
                    case 2:
                        if (current_char == op.first[0] && preprocessed[i + 1] == op.first[1])
                        {
                            result.push_back(Token(TokenType::Operator, op.first));
                            i += op.second - 1;
                            found = true;
                        }
                        break;
                    case 3:
                        if (current_char == op.first[0] && preprocessed[i + 1] == op.first[1] && preprocessed[i + 2] == op.first[2])
                        {
                            result.push_back(Token(TokenType::Operator, op.first));
                            i += op.second - 1;
                            found = true;
                        }
                        break;
                    case 4:
                        if (current_char == op.first[0] && preprocessed[i + 1] == op.first[1] && preprocessed[i + 2] == op.first[2] && preprocessed[i + 3] == op.first[3])
                        {
                            result.push_back(Token(TokenType::Operator, op.first));
                            i += op.second - 1;
                            found = true;
                        }
                        break;
                    case 6:
                        if (current_char == op.first[0] && preprocessed[i + 1] == op.first[1] && preprocessed[i + 2] == op.first[2] && preprocessed[i + 3] == op.first[3] && preprocessed[i + 4] == op.first[4] && preprocessed[i + 5] == op.first[5])
                        {
                            result.push_back(Token(TokenType::Operator, op.first));
                            i += op.second - 1;
                            found = true;
                        }

                    default:
                        break;
                    }

                    if (found)
                    {
                        break;
                    }
                }
            }
            if (found)
            {
                break;
            }

            // Check for Punctuator
            found = false;
            for (const auto &sep : lexPunctuators)
            {
                if (src_length - i >= sep.second)
                {
                    switch (sep.second)
                    {
                    case 1:
                        if (current_char == sep.first[0])
                        {
                            result.push_back(Token(TokenType::Punctuator, sep.first));
                            found = true;
                        }
                        break;
                    case 2:
                        if (current_char == sep.first[0] && preprocessed[i + 1] == sep.first[1])
                        {
                            result.push_back(Token(TokenType::Punctuator, sep.first));
                            i++;
                            found = true;
                        }
                        break;

                    default:
                        break;
                    }

                    if (found)
                    {
                        break;
                    }
                }
            }
            if (found)
            {
                break;
            }

            // Check for string literals
            if (current_char == '"')
            {
                state = LexerState::StringLiteral;
                modifier = LexerStateModifier::None;
                break;
            }
            else if (current_char == '\'')
            {
                state = LexerState::StringLiteral;
                modifier = LexerStateModifier::StringSingleQuote;
                break;
            }

            // Check for keyword
            found = false;
            for (auto kw : lexKeywords)
            {
                size_t kw_length = strlen(kw);
                if (src_length - i >= kw_length && preprocessed.substr(i, kw_length) == kw)
                {
                    result.push_back(Token(TokenType::Keyword, kw));
                    i += kw_length - 1;
                    found = true;
                    break;
                }
            }
            if (found)
            {
                break;
            }

            // Check for identifier
            if (!std::isdigit(current_char))
            {
                if (std::isalpha(current_char) || current_char == '_')
                {
                    state = LexerState::Identifier;
                    continue;
                }
            }

            // Check for number literal
            /// TODO: Check for number literal
            if (std::isdigit(current_char))
            {
                state = LexerState::NumberLiteral;
                continue;
            }

            // Check for whitespace
            if (std::isspace(current_char))
            {
                state = LexerState::Whitespace;
                continue;
            }

            // invalid state
            throw LexerExceptionUnexpected("Unexpected token at line " + std::to_string(line) + ", column " + std::to_string(column) + ": '" + current_char + "': '" + current_token + "'");

            break;
        case LexerState::StringLiteral:
            if (current_char == '\n' || src_length - i == 0)
            {
                throw LexerExceptionInvalidLiteral("String literal not terminated. Expected '\"' at line " + std::to_string(line) + ", column " + std::to_string(column));
            }

            if (modifier == LexerStateModifier::StringSingleQuote || modifier == LexerStateModifier::StringSingleQuoteEscape)
            {
                if (modifier == LexerStateModifier::StringSingleQuoteEscape)
                {
                    switch (current_char)
                    {
                    case 'n':
                        current_token += '\n';
                        break;
                    case 't':
                        current_token += '\t';
                        break;
                    case 'r':
                        current_token += '\r';
                        break;
                    case '0':
                        current_token += '\0';
                        break;
                    case '\\':
                        current_token += '\\';
                        break;

                    default:
                        current_token += current_char;
                        break;
                    }
                    modifier = LexerStateModifier::StringSingleQuote;
                }
                else if (current_char == '\\')
                {
                    modifier = LexerStateModifier::StringSingleQuoteEscape;
                }
                else if (current_char == '\'')
                {
                    result.push_back(Token(TokenType::StringLiteral, current_token));
                    current_token.clear();
                    state = LexerState::Default;
                    modifier = LexerStateModifier::None;
                }
                else
                {
                    current_token += current_char;
                }

                break;
            }
            else
            {

                if (modifier == LexerStateModifier::StringEscape)
                {
                    switch (current_char)
                    {
                    case 'n':
                        current_token += '\n';
                        break;
                    case 't':
                        current_token += '\t';
                        break;
                    case 'r':
                        current_token += '\r';
                        break;
                    case '0':
                        current_token += '\0';
                        break;
                    case '\\':
                        current_token += '\\';
                        break;

                    default:
                        current_token += current_char;
                        break;
                    }
                    modifier = LexerStateModifier::None;
                }
                else if (current_char == '\\')
                {
                    modifier = LexerStateModifier::StringEscape;
                }
                else if (current_char == '"')
                {
                    result.push_back(Token(TokenType::StringLiteral, current_token));
                    current_token.clear();
                    state = LexerState::Default;
                }
                else
                {
                    current_token += current_char;
                }
            }
            break;
        case LexerState::NumberLiteral:
            if ((current_token.empty() || current_token.size() == 1))
            {
                if (!std::isdigit(current_char) && current_char != 'b' && current_char != 'o' && current_char != 'x' && current_char != 'd')
                {
                    if (current_token.size() > 1)
                    {
                        throw LexerExceptionInvalidLiteral("Number literal not valid at line " + std::to_string(line) + ", column " + std::to_string(column));
                    }
                    else
                    {
                        goto finish_number_literal;
                    }
                }

                current_token += current_char;
                break;
            }

            switch (current_token[0])
            {
            case '0':
                if (current_token.size() >= 2 && current_token[1] == 'x')
                {
                    if ((current_char >= '0' && current_char <= '9') || (current_char >= 'a' && current_char <= 'f') || (current_char >= 'A' && current_char <= 'F'))
                    {
                        current_token += current_char;
                    }
                    else if (current_token.length() == 2)
                    {
                        throw LexerExceptionInvalidLiteral("Hexadecimal number literal not valid at line " + std::to_string(line) + ", column " + std::to_string(column));
                    }
                    else if (isalnum(current_char) || current_char == '_')
                    {
                        throw LexerExceptionInvalidLiteral("Hexadecimal number literal not valid at line " + std::to_string(line) + ", column " + std::to_string(column));
                    }
                    else
                    {
                        goto finish_number_literal;
                    }
                }
                else if (current_token.size() >= 2 && current_token[1] == 'b')
                {
                    if (current_char == '0' || current_char == '1')
                    {
                        current_token += current_char;
                    }
                    else if (current_token.length() == 2)
                    {
                        throw LexerExceptionInvalidLiteral("Binary number literal not valid at line " + std::to_string(line) + ", column " + std::to_string(column));
                    }
                    else if (isalnum(current_char) || current_char == '_')
                    {
                        throw LexerExceptionInvalidLiteral("Binary number literal not valid at line " + std::to_string(line) + ", column " + std::to_string(column));
                    }
                    else
                    {
                        goto finish_number_literal;
                    }
                }
                else if (current_token.size() >= 2 && current_token[1] == 'o')
                {
                    if (current_char >= '0' && current_char <= '7')
                    {
                        current_token += current_char;
                    }
                    else if (current_token.length() == 2)
                    {
                        throw LexerExceptionInvalidLiteral("Octal number literal not valid at line " + std::to_string(line) + ", column " + std::to_string(column));
                    }
                    else if (isalnum(current_char) || current_char == '_')
                    {
                        throw LexerExceptionInvalidLiteral("Octal number literal not valid at line " + std::to_string(line) + ", column " + std::to_string(column));
                    }
                    else
                    {
                        goto finish_number_literal;
                    }
                }
                else if (current_token.size() >= 2 && current_token[1] == 'd')
                {
                    if (current_char >= '0' && current_char <= '9')
                    {
                        current_token += current_char;
                    }
                    else if (current_token.length() == 2)
                    {
                        throw LexerExceptionInvalidLiteral("Decimal number literal not valid at line " + std::to_string(line) + ", column " + std::to_string(column));
                    }
                    else if (isalnum(current_char) || current_char == '_')
                    {
                        throw LexerExceptionInvalidLiteral("Decimal number literal not valid at line " + std::to_string(line) + ", column " + std::to_string(column));
                    }
                    else
                    {
                        goto finish_number_literal;
                    }
                }
                else if ((current_char >= '0' && current_char <= '9') || current_char == '.')
                {
                    current_token += current_char;
                }
                else
                {
                    goto finish_number_literal;
                }
                break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                if ((current_char >= '0' && current_char <= '9') || current_char == '.' || current_char == 'e')
                {
                    current_token += current_char;
                }
                else if (isalnum(current_char) || current_char == '_')
                {
                    throw LexerExceptionInvalidLiteral("Number literal not valid at line " + std::to_string(line) + ", column " + std::to_string(column));
                }
                else
                {
                    goto finish_number_literal;
                }
                break;
            default:
                throw LexerExceptionInvalidLiteral("Number literal not valid at line " + std::to_string(line) + ", column " + std::to_string(column));
            }
            break;

        finish_number_literal:
            normalize_number_literal(current_token, column, line);
            /// TODO: Fix this
            result.push_back(Token(TokenType::NumberLiteral, (uint64_t)0));
            current_token.clear();
            state = LexerState::Default;
            continue;
        case LexerState::SingleLineComment:
            if (current_char != '\n' || src_length - i == 0)
            {
                current_token += current_char;
            }
            else
            {
                result.push_back(Token(TokenType::SingleLineComment, current_token));
                current_token.clear();
                state = LexerState::Default;
                line++;
                column = 1;
                continue;
            }
            break;
        case LexerState::MultiLineComment:
            if (src_length - i >= 2 && current_char == '*' && preprocessed[i + 1] == '/')
            {
                i += 2;
                column += 2;
                result.push_back(Token(TokenType::MultiLineComment, current_token));
                current_token.clear();
                state = LexerState::Default;
                continue;
            }
            else
            {
                current_token += current_char;
            }
            break;
        case LexerState::Identifier:
            if (std::isalnum(current_char) || current_char == '_')
            {
                current_token += current_char;
            }
            else
            {
                result.push_back(Token(TokenType::Identifier, current_token));
                current_token.clear();
                state = LexerState::Default;
                i--;
            }
            break;
        case LexerState::Operator:
            /* code */
            break;
        case LexerState::Punctuator:
            /* code */
            break;
        case LexerState::Whitespace:

            if (std::isspace(current_char))
            {
                current_token += current_char;
            }
            else
            {
                result.push_back(Token(TokenType::Whitespace, current_token));
                current_token.clear();
                state = LexerState::Default;
                continue;
            }

            // handle newlines
            if (current_char == '\n')
            {
                line++;
                column = 0;
                break;
            }

            break;

        default:
            throw LexerExceptionUnexpected("Lexer invalid state");
            break;
        }

        i++;
        column++;
    }

    if (state != LexerState::Default)
    {
        switch (state)
        {
        case LexerState::StringLiteral:
            if (modifier == LexerStateModifier::StringSingleQuote || modifier == LexerStateModifier::StringSingleQuoteEscape)
            {
                throw LexerExceptionInvalidLiteral("String literal not terminated. Expected \"'\" at line " + std::to_string(line) + ", column " + std::to_string(column));
            }
            else
            {
                throw LexerExceptionInvalidLiteral("String literal not terminated. Expected '\"' at line " + std::to_string(line) + ", column " + std::to_string(column));
            }
            break;
        case LexerState::NumberLiteral:
            throw LexerExceptionInvalidLiteral("Number literal not valid at line " + std::to_string(line) + ", column " + std::to_string(column));
            break;
        case LexerState::SingleLineComment:
            throw LexerExceptionInvalid("Single line comment not valid at line " + std::to_string(line) + ", column " + std::to_string(column));
            break;
        case LexerState::MultiLineComment:
            throw LexerExceptionInvalid("Multi line comment not terminated. Expected '*/' at line " + std::to_string(line) + ", column " + std::to_string(column));
            break;
        case LexerState::Identifier:
            throw LexerExceptionInvalidIdentifier("Invalid identifier \"" + current_token + "\" at line " + std::to_string(line) + ", column " + std::to_string(column));
            break;
        case LexerState::Operator:
            throw LexerExceptionInvalidOperator("Invalid operator \"" + current_token + "\" at line " + std::to_string(line) + ", column " + std::to_string(column));
            break;
        case LexerState::Punctuator:
            throw LexerExceptionInvalidPunctuator("Invalid punctuator \"" + current_token + "\" at line " + std::to_string(line) + ", column " + std::to_string(column));
            break;

        default:
            break;
        }
    }

    return result;
}