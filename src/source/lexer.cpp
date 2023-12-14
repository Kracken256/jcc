#include "lexer.hpp"

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
        return std::string("Identifier(" + m_value + ")");
        break;
    case TokenType::Keyword:
        return std::string("Keyword(" + m_value + ")");
        break;
    case TokenType::NumberLiteral:
        return std::string("NumberLiteral(" + m_value + ")");
        break;
    case TokenType::StringLiteral:
        return std::string("StringLiteral(" + m_value + ")");
        break;
    case TokenType::Operator:
        return std::string("Operator(" + m_value + ")");
        break;
    case TokenType::Punctuator:
        return std::string("Punctuator(" + m_value + ")");
        break;
    case TokenType::Comment:
        return std::string("Comment(" + m_value + ")");
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
    for (auto &token : m_tokens)
    {
        result += token.to_string() + ", ";
    }
    result = result.substr(0, result.length() - 2);
    result += ")";
    return result;
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

std::vector<jcc::Token> jcc::Lexer::lex(const std::string &source)
{
    return {};
}