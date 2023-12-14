#include "lexer.hpp"
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <cstring>

///=============================================================================
/// jcc::Token class implementation
///=============================================================================

jcc::Token::Token()
{
    m_type = jcc::TokenType::Unknown;
    m_value = "";
}

jcc::Token::Token(jcc::TokenType type, const std::string &value)
{
    m_type = type;
    m_value = value;
}

jcc::TokenType jcc::Token::type() const
{
    return m_type;
}

const std::string &jcc::Token::value() const
{
    return m_value;
}

std::string jcc::Token::to_string() const
{
    switch (this->m_type)
    {
    case TokenType::Identifier:
        return std::string("Identifier(\"" + m_value + "\")");
        break;
    case TokenType::Keyword:
        return std::string("Keyword(" + m_value + ")");
        break;
    case TokenType::NumberLiteral:
        return std::string("NumberLiteral(" + m_value + ")");
        break;
    case TokenType::StringLiteral:
        return std::string("StringLiteral(\"" + m_value + "\")");
        break;
    case TokenType::Operator:
        return std::string("Operator('" + m_value + "')");
        break;
    case TokenType::Punctuator:
        return std::string("Punctuator('" + m_value + "')");
        break;
    case TokenType::SingleLineComment:
        return std::string("SingleLineComment(\"" + m_value + "\")");
        break;
    case TokenType::MultiLineComment:
        return std::string("MultiLineComment(\"" + m_value + "\")");
        break;
    case TokenType::Whitespace:
        return std::string("Whitespace(" + m_value + ")");
        break;
    default:
        return std::string("Unknown(" + m_value + ")");
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
        result += m_tokens[i].to_string();

        if (i != m_tokens.size() - 1)
        {
            result += ", ";
        }
    }

    return result + ")";
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
};

static std::set<char> lexSeperators = {'(', ')', '{', '}', '[', ']', ';', ',', ':'};

static std::set<std::string> lexOperators = {
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
};

static std::set<std::string> lexKeywords = {
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
static const size_t lexIdentifierCharsLength = sizeof(lexIdentifierChars) / sizeof(char);

jcc::TokenList jcc::Lexer::lex(const std::string &source)
{
    /// TODO: implement
    TokenList result;
    std::string current_token;
    LexerState state;
    LexerStateModifier modifier;
    size_t i, src_length, line, column;
    bool found = false;

    current_token = "";
    state = LexerState::Default;
    modifier = LexerStateModifier::None;
    i = 0;
    line = 1;
    column = 1;
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

            // Check for separator
            if (lexSeperators.find(current_char) != lexSeperators.end())
            {
                result.push_back(Token(TokenType::Punctuator, std::string(1, current_char)));
                break;
            }

            // Check for operator
            found = false;
            for (const auto &op : lexOperators)
            {
                if (src_length - i >= op.length() && source.substr(i, op.length()) == op)
                {
                    result.push_back(Token(TokenType::Operator, op));
                    i += op.length() - 1;
                    found = true;
                    break;
                }
            }
            if (found)
            {
                break;
            }

            // Check for string literal
            if (current_char == '"')
            {
                state = LexerState::StringLiteral;
                break;
            }

            // Check for keyword
            found = false;
            for (const auto &kw : lexKeywords)
            {
                if (src_length - i >= kw.length() && source.substr(i, kw.length()) == kw)
                {
                    result.push_back(Token(TokenType::Keyword, kw));
                    i += kw.length() - 1;
                    found = true;
                    break;
                }
            }
            if (found)
            {
                break;
            }

            // Check for identifier
            if (!std::isdigit(current_char) && current_char != 'e')
            {
                found = false;
                for (size_t j = 0; j < lexIdentifierCharsLength; j++)
                {
                    if (current_char == lexIdentifierChars[j])
                    {
                        found = true;
                        break;
                    }
                }
                if (found)
                {
                    state = LexerState::Identifier;
                    continue;
                }
            }

            // Check for number literal
            /// TODO: Check for number literal

            // Check for whitespace
            if (std::isspace(current_char))
            {
                state = LexerState::Whitespace;
                current_token += current_char;
                break;
            }

            // invalid state
            throw LexerExceptionUnexpected("Unexpected token at line " + std::to_string(line) + ", column " + std::to_string(column) + ": '" + current_char + "': '" + current_token + "'");

            break;
        case LexerState::StringLiteral:
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
                current_token = "";
                state = LexerState::Default;
            }
            else
            {
                current_token += current_char;
            }
            break;
        case LexerState::NumberLiteral:
            /* code */
            break;
        case LexerState::SingleLineComment:
            if (current_char != '\n')
            {
                current_token += current_char;
            }
            else
            {
                result.push_back(Token(TokenType::SingleLineComment, current_token));
                current_token = "";
                state = LexerState::Default;
            }
            break;
        case LexerState::MultiLineComment:
            if (src_length - i >= 2 && current_char == '*' && source[i + 1] == '/')
            {
                i += 2;
                column += 2;
                result.push_back(Token(TokenType::MultiLineComment, current_token));
                current_token = "";
                state = LexerState::Default;
                continue;
            }
            else
            {
                current_token += current_char;
            }
            break;
        case LexerState::Identifier:
            found = false;
            for (size_t j = 0; j < lexIdentifierCharsLength; j++)
            {
                if (current_char == lexIdentifierChars[j])
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                result.push_back(Token(TokenType::Identifier, current_token));
                current_token = "";
                state = LexerState::Default;
            }
            else
            {
                current_token += current_char;
            }
            break;
        case LexerState::Operator:
            /* code */
            break;
        case LexerState::Punctuator:
            /* code */
            break;
        case LexerState::Whitespace:

            // handle newlines
            if (current_char == '\n')
            {
                line++;
                column = 1;
            }

            if (!std::isspace(current_char))
            {
                result.push_back(Token(TokenType::Whitespace, current_token));
                current_token = "";
                state = LexerState::Default;
                continue;
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
            throw LexerExceptionInvalidLiteral("String literal not terminated. Expected '\"' at line " + std::to_string(line) + ", column " + std::to_string(column));
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
        case LexerState::Whitespace:
            throw LexerExceptionInvalid("Invalid whitespace \"" + current_token + "\" at line " + std::to_string(line) + ", column " + std::to_string(column));
            break;

        default:
            break;
        }
    }

    return result;
}