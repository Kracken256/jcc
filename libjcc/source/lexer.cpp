#include "lexer.hpp"
#include "compile.hpp"
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <cstring>
#include <regex>
#include <sstream>
#include <cmath>
#include <iomanip>

#define FLOATING_POINT_LITERAL_ROUND_DIGITS 32

///=============================================================================
/// jcc::Token class implementation
///=============================================================================

static std::map<jcc::TokenType, std::string> tokenTypeMap = {
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
    {jcc::TokenType::Raw, "raw"},
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
        return std::string("Keyword(" + std::string(lexKeywordMapReverse.at(std::get<Keyword>(m_value))) + ")");
        break;
    case TokenType::NumberLiteral:
        return std::string("NumberLiteral(" + std::to_string(std::get<uint64_t>(m_value)) + ")");
        break;
    case TokenType::FloatingPointLiteral:
        return std::string("FloatingPointLiteral(" + std::get<std::string>(m_value) + ")");
    case TokenType::StringLiteral:
        return std::string("StringLiteral(\"" + std::get<std::string>(m_value) + "\")");
        break;
    case TokenType::Operator:
        return std::string("Operator('" + std::string(lexOperatorMapReverse.at(std::get<Operator>(m_value))) + "')");
        break;
    case TokenType::Punctuator:
        return std::string("Punctuator('" + std::string(lexPunctuatorMapReverse.at(std::get<Punctuator>(m_value))) + "')");
        break;
    case TokenType::SingleLineComment:
        return std::string("SingleLineComment(\"" + std::get<std::string>(m_value) + "\")");
        break;
    case TokenType::MultiLineComment:
        return std::string("MultiLineComment(\"" + std::get<std::string>(m_value) + "\")");
        break;
    case TokenType::Whitespace:
        return std::string("Whitespace()");
        break;
    case TokenType::Raw:
        return std::string("Raw(\"" + std::get<std::string>(m_value) + "\")");
        break;
    default:
        return std::string("Unknown()");
        break;
    }
}

bool jcc::Token::operator==(const jcc::Token &other) const
{
    return this->m_type == other.m_type && this->m_value == other.m_value;
}

///=============================================================================
/// jcc::TokenList class implementation
///=============================================================================

jcc::TokenList::TokenList()
{
    this->m_tokens = std::vector<Token>();
}

jcc::TokenList::TokenList(const std::vector<jcc::Token> &tokens)
{
    this->m_tokens = tokens;
}

void jcc::TokenList::push_back(const jcc::Token &token)
{
    m_tokens.push_back(token);
}

void jcc::TokenList::push_back(const std::vector<jcc::Token> &tokens)
{
    m_tokens.insert(m_tokens.end(), tokens.begin(), tokens.end());
}

void jcc::TokenList::done()
{
    std::reverse(m_tokens.begin(), m_tokens.end());
}

std::string jcc::TokenList::to_string() const
{
    std::string result = "TokenList(";

    for (size_t i = m_tokens.size(); i > 0; i--)
    {
        result += m_tokens[i - 1].to_string();

        if (i != 1)
        {
            result += ", ";
        }
    }

    return result + ")";
}

std::string jcc::TokenList::to_json() const
{
    std::string result = "[";
    for (auto it = m_tokens.rbegin(); it != m_tokens.rend(); ++it)
    {
        const auto &token = *it;
        std::string dataString = "", escapedString = "";
        if (token.type() == TokenType::Whitespace)
        {
            continue;
        }
        result += "{";
        result += "\"t\":\"" + tokenTypeMap[static_cast<jcc::TokenType>(token.type())] + "\",";

        switch (token.type())
        {
        case TokenType::Identifier:
            dataString = std::get<std::string>(token.value());
            break;
        case TokenType::Keyword:
            dataString = std::string(lexKeywordMapReverse.at(std::get<Keyword>(token.value())));
            break;
        case TokenType::NumberLiteral:
            dataString = std::to_string(std::get<uint64_t>(token.value()));
            break;
        case TokenType::FloatingPointLiteral:
            dataString = std::get<std::string>(token.value());
            break;
        case TokenType::StringLiteral:
            dataString = std::get<std::string>(token.value());
            break;
        case TokenType::Operator:
            dataString = std::string(lexOperatorMapReverse.at(std::get<Operator>(token.value())));
            break;
        case TokenType::Punctuator:
            dataString = std::string(lexPunctuatorMapReverse.at(std::get<Punctuator>(token.value())));
            break;
        case TokenType::SingleLineComment:
            dataString = std::get<std::string>(token.value());
            break;
        case TokenType::MultiLineComment:
            dataString = std::get<std::string>(token.value());
            break;
        case TokenType::Raw:
            dataString = std::get<std::string>(token.value());
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

        if (it + 1 != m_tokens.rend())
        {
            result += ", ";
        }
    }
    return result + "]";
}

const jcc::Token &jcc::TokenList::operator[](size_t index) const
{
    return m_tokens.at(index);
}

void jcc::TokenList::pop(size_t count)
{
    if (count > m_tokens.size())
    {
        panic("Unable to pop from TokenList, count is greater than size", {});
    }

    m_tokens.pop_back();
    for (size_t i = 0; i < count - 1; i++)
    {
        m_tokens.pop_back();
    }
}

bool jcc::TokenList::eof() const
{
    return m_tokens.empty();
}

const jcc::Token &jcc::TokenList::peek(size_t index) const
{
    if (index >= m_tokens.size())
    {
        panic("Unable to peek TokenList, index out of bounds", {});
    }

    return m_tokens[m_tokens.size() - index - 1];
}

size_t jcc::TokenList::size() const
{
    return m_tokens.size();
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
    Whitespace,
    Raw
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
    {".", 1},  // dot
};

static std::vector<std::pair<const char *, unsigned int>> lexOperators = {
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

    {"?", 1}, // ternary
};

static const char lexIdentifierChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_:";
static const size_t lexIdentifierCharsLength = (sizeof(lexIdentifierChars) - 1) / sizeof(char);

enum class NumberLiteralType
{
    Invalid,
    Decimal,
    DecimalExplicit,
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
            if (!((input[i] >= '0' && input[i] <= '9') || (input[i] >= 'a' && input[i] <= 'f')))
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
        return NumberLiteralType::DecimalExplicit;
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
        std::cout << "Floating point number literal detected:" << input << std::endl;
        return NumberLiteralType::Floating;
    }

    return NumberLiteralType::Invalid;
}

static std::string normalize_float(const std::string &input)
{
    double mantissa = 0;
    double exponent = 0;
    double x = 0;

    size_t e_pos = input.find('e');

    if (e_pos == std::string::npos)
    {
        return input;
    }

    mantissa = std::stod(input.substr(0, e_pos));
    exponent = std::stod(input.substr(e_pos + 1));

    x = mantissa * std::pow(10.0, exponent);
    std::stringstream ss;
    ss << std::setprecision(FLOATING_POINT_LITERAL_ROUND_DIGITS) << x;
    return ss.str();
}

uint64_t normalize_number_literal(std::string &number, size_t column, size_t line)
{
    uint64_t x = 0;

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
        for (size_t i = 2; i < number.length(); ++i)
        {
            // check for overflow
            if (x & 0xF000000000000000)
            {
                throw jcc::LexerExceptionInvalidLiteral("Hexadecimal number literal at line " + std::to_string(line) + ", column " + std::to_string(column) + " is too large. Will not fit in 64 bits.");
            }

            if (number[i] >= '0' && number[i] <= '9')
            {
                x = (x << 4) + (number[i] - '0');
            }
            else if (number[i] >= 'a' && number[i] <= 'f')
            {
                x = (x << 4) + (number[i] - 'a' + 10);
            }
            else
            {
                throw jcc::LexerExceptionInvalidLiteral("Hexadecimal number literal not valid at line " + std::to_string(line) + ", column " + std::to_string(column));
            }
        }
        break;
    case NumberLiteralType::Binary:
        for (size_t i = 2; i < number.length(); ++i)
        {
            // check for overflow
            if (x & 0x8000000000000000)
            {
                throw jcc::LexerExceptionInvalidLiteral("Binary number literal at line " + std::to_string(line) + ", column " + std::to_string(column) + " is too large. Will not fit in 64 bits.");
            }

            x = (x << 1) + (number[i] - '0');
        }
        break;
    case NumberLiteralType::Octal:
        for (size_t i = 2; i < number.length(); ++i)
        {
            // check for overflow
            if (x & 0xE000000000000000)
            {
                throw jcc::LexerExceptionInvalidLiteral("Octal number literal at line " + std::to_string(line) + ", column " + std::to_string(column) + " is too large. Will not fit in 64 bits.");
            }

            x = (x << 3) + (number[i] - '0');
        }
        break;
    case NumberLiteralType::DecimalExplicit:
        x = std::stoull(number.substr(2));
        break;
    case NumberLiteralType::Decimal:
        x = std::stoull(number);
        break;
    default:
        break;
    }

    /// TODO: Verify correctness of literal normalization
    return x;
}

/// @brief Lex the source code into a list of tokens
/// @param source JXX source code raw string
/// @param preprocess Whether to preprocess the source code
/// @return A vector of tokens
/// @note This is probably the most complex I have ever written. So expect bugs.
jcc::TokenList jcc::CompilationUnit::lex(const std::string &source)
{
    /// TODO: Run unit tests on this function
    std::string current_token;
    TokenList result;
    LexerState state = LexerState::Default;
    LexerStateModifier modifier = LexerStateModifier::None;
    size_t i = 0, src_length, line = 1, column = 1;
    bool found = false;

    src_length = source.length();

    while (i < src_length)
    {
        char current_char = source[i];

        switch (state)
        {
        case LexerState::Default:
            // Check for single line comment
            if (src_length - i >= 2 && current_char == '/' && source[i + 1] == '/')
            {
                state = LexerState::SingleLineComment;
                i++;
                column++;
                break;
            }

            // Check for multi line comment
            if (src_length - i >= 2 && current_char == '/' && source[i + 1] == '*')
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
                            result.push_back(Token(TokenType::Operator, lexOperatorMap.at(op.first)));
                            i += op.second - 1;
                            found = true;
                        }
                        break;
                    case 2:
                        if (current_char == op.first[0] && source[i + 1] == op.first[1])
                        {
                            result.push_back(Token(TokenType::Operator, lexOperatorMap.at(op.first)));
                            i += op.second - 1;
                            found = true;
                        }
                        break;
                    case 3:
                        if (current_char == op.first[0] && source[i + 1] == op.first[1] && source[i + 2] == op.first[2])
                        {
                            result.push_back(Token(TokenType::Operator, lexOperatorMap.at(op.first)));
                            i += op.second - 1;
                            found = true;
                        }
                        break;
                    case 4:
                        if (current_char == op.first[0] && source[i + 1] == op.first[1] && source[i + 2] == op.first[2] && source[i + 3] == op.first[3])
                        {
                            result.push_back(Token(TokenType::Operator, lexOperatorMap.at(op.first)));
                            i += op.second - 1;
                            found = true;
                        }
                        break;
                    case 6:
                        if (current_char == op.first[0] && source[i + 1] == op.first[1] && source[i + 2] == op.first[2] && source[i + 3] == op.first[3] && source[i + 4] == op.first[4] && source[i + 5] == op.first[5])
                        {
                            result.push_back(Token(TokenType::Operator, lexOperatorMap.at(op.first)));
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
                            result.push_back(Token(TokenType::Punctuator, lexPunctuatorMap.at(sep.first)));
                            found = true;
                        }
                        break;
                    case 2:
                        if (current_char == sep.first[0] && source[i + 1] == sep.first[1])
                        {
                            result.push_back(Token(TokenType::Punctuator, lexPunctuatorMap.at(sep.first)));
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
            for (auto kw : lexKeywordMap)
            {
                size_t kw_length = strlen(kw.first);
                if (src_length - i >= kw_length && source.substr(i, kw_length) == kw.first)
                {
                    // verify next character is not an identifier character
                    if (src_length - i > kw_length && std::strchr(lexIdentifierChars, source[i + kw_length]))
                    {
                        break;
                    }

                    result.push_back(Token(TokenType::Keyword, kw.second));
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
                if (std::isalpha(current_char) || current_char == '_' || current_char == ':')
                {
                    state = LexerState::Identifier;
                    continue;
                }
            }

            // Check for number literal
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

            if (current_char == '`')
            {
                state = LexerState::Raw;
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
                if (!std::isdigit(current_char) && (current_char != 'b' && current_char != 'o' && current_char != 'x' && current_char != 'd' && current_char != '.'))
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
            if (current_token.find('.') != std::string::npos || current_token.find('e') != std::string::npos)
            {
                result.push_back(Token(TokenType::FloatingPointLiteral, normalize_float(current_token)));
            }
            else
            {
                result.push_back(Token(TokenType::NumberLiteral, normalize_number_literal(current_token, column, line)));
            }
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
            if (src_length - i >= 2 && current_char == '*' && source[i + 1] == '/')
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
            if (std::isalnum(current_char) || current_char == '_' || current_char == ':')
            {
                // look ahead and see if the next character is a colon
                if (current_char == ':')
                {
                    if ((src_length - i < 2 || source[i + 1] != ':'))
                    {
                        result.push_back(Token(TokenType::Identifier, current_token));
                        current_token.clear();
                        state = LexerState::Default;
                        continue;
                    }
                    else
                    {
                        current_token += "::";
                        i++;
                        column++;
                        break;
                    }
                }

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
        case LexerState::Raw:
            if (current_char == '`')
            {
                if (modifier == LexerStateModifier::StringEscape)
                {
                    result.push_back(Token(TokenType::Raw, current_token));
                    current_token.clear();
                    state = LexerState::Default;
                    modifier = LexerStateModifier::None;
                    break;
                }
                modifier = LexerStateModifier::StringEscape;
            }
            else
            {
                current_token += current_char;
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

    result.done();

    return result;
}