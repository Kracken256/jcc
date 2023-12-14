#ifndef _JCC_LEXER_HPP_
#define _JCC_LEXER_HPP_

#include <string>
#include <vector>
#include <variant>
#include <exception>
#include <stdexcept>

namespace jcc
{
    enum TokenType
    {
        Unknown = -1,
        Identifier,
        Keyword,
        NumberLiteral,
        StringLiteral,
        Operator,
        Punctuator,
        Comment,
        Whitespace,
    };

    class Token
    {
    public:
        Token();

        ~Token() = default;

        /// @brief Construct a new Token object
        /// @param type The type of the token
        /// @param value The value of the token
        Token(TokenType type, const std::string &value);

        /// @brief Get the type of the token
        /// @return TokenType
        TokenType type() const;

        /// @brief Get the value of the token
        /// @return std::string
        const std::string &value() const;

        /// @brief Convert the token to a string
        /// @return std::string
        std::string to_string() const;

    private:
        TokenType m_type;
        std::string m_value;
    };

    class TokenList
    {
    public:
        /// @brief Construct a new TokenList object
        TokenList();

        /// @brief Construct a new TokenList object
        TokenList(const std::vector<Token> &tokens);

        ~TokenList() = default;
        TokenList(const TokenList &) = delete;
        TokenList &operator=(const TokenList &) = delete;

        /// @brief Push a token to the back of the list
        /// @param token The token to push
        void push_back(const Token &token);

        /// @brief Push a vector of tokens to the back of the list
        /// @param tokens The vector of tokens to push
        void push_back(const std::vector<Token> &tokens);

        /// @brief Lock the list
        /// @details Once the list is locked, no more tokens can be pushed.
        void lock();

        /// @brief Check if the list is locked
        /// @return true if the list is locked
        bool is_locked() const;

        /// @brief Get the tokens
        /// @return std::vector<Token>
        const std::vector<Token> &tokens() const;

        /// @brief Convert TokenList to a human-readable string
        /// @return std::string
        std::string to_string() const;

        /// @brief Get the token at the specified index
        /// @param index The index of the token
        /// @return Token
        const Token &operator[](size_t index) const;

        /// @brief Pop the first token off the list
        void pop_front();

        /// @brief Get the size of the list
        /// @return size_t
        size_t size() const;

    private:
        std::vector<Token> m_tokens;
        bool m_locked;
    };

    class LexerException : public std::exception
    {
    public:
        LexerException(const std::string &message)
            : m_message(message)
        {
        }

        const char *what() const noexcept override
        {
            return std::string("LexerException: " + m_message).c_str();
        }

    protected:
        std::string m_message;
    };

    class LexerExceptionInvalid : public LexerException
    {
    public:
        LexerExceptionInvalid(const std::string &message)
            : LexerException(message)
        {
        }
    };

    class LexerExceptionUnexpected : public LexerException
    {
    public:
        LexerExceptionUnexpected(const std::string &message)
            : LexerException(message)
        {
        }
    };

    class LexerExceptionInvalidLiteral : public LexerExceptionInvalid
    {
    public:
        LexerExceptionInvalidLiteral(const std::string &message)
            : LexerExceptionInvalid(message)
        {
        }
    };

    class LexerExceptionInvalidIdentifier : public LexerExceptionInvalid
    {
    public:
        LexerExceptionInvalidIdentifier(const std::string &message)
            : LexerExceptionInvalid(message)
        {
        }
    };

    class LexerExceptionInvalidOperator : public LexerExceptionInvalid
    {
    public:
        LexerExceptionInvalidOperator(const std::string &message)
            : LexerExceptionInvalid(message)
        {
        }
    };

    class LexerExceptionInvalidPunctuator : public LexerExceptionInvalid
    {
    public:
        LexerExceptionInvalidPunctuator(const std::string &message)
            : LexerExceptionInvalid(message)
        {
        }
    };

    class LexerExceptionUnexpectedEOF : public LexerExceptionUnexpected
    {
    public:
        LexerExceptionUnexpectedEOF(const std::string &message)
            : LexerExceptionUnexpected(message)
        {
        }
    };

    class Lexer
    {
    public:
        /// @brief Perform lexical analysis on JXX source code
        /// @param source JXX source code raw string
        /// @return A vector of tokens
        /// @throw LexerException
        static std::vector<Token> lex(const std::string &source);

    private:
        Lexer() = default;
        ~Lexer() = default;
    };
}

#endif // _JCC_LEXER_HPP_