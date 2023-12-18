#ifndef _JCC_PARSER_HPP_
#define _JCC_PARSER_HPP_

#include <string>
#include <vector>
#include <exception>
#include <stdexcept>
#include <variant>
#include <memory>
#include "lexer.hpp"
#include "compile.hpp"

namespace jcc
{

    enum class SyntaxNodeType
    {
        Root,
        Identifier,
        Keyword,
        StringLiteral,
        NumberLiteral,
        FloatingPointLiteral,
        Operator,
        Punctuator,
    };

    struct SyntaxNodeValue
    {
        SyntaxNodeType type;
        std::variant<std::string, uint64_t, double, const char *> value;
    };
}

namespace std
{
    std::string to_string(jcc::SyntaxNodeType type);
    std::string to_string(const jcc::SyntaxNodeValue &value);
}

#include "node.hpp"

namespace jcc
{
    typedef Node<SyntaxNodeValue> ASTNode;
    typedef ASTNode AbstractSyntaxTree;

    class ParserException : public std::runtime_error
    {
    public:
        ParserException(const std::string &message) : std::runtime_error(message) {}
    };

    class UnexpectedTokenError : public ParserException
    {
    public:
        UnexpectedTokenError(const std::string &message) : ParserException(message) {}
    };

    class SyntaxError : public ParserException
    {
    public:
        SyntaxError(const std::string &message) : ParserException(message) {}
    };

    class SemanticError : public ParserException
    {
    public:
        SemanticError(const std::string &message) : ParserException(message) {}
    };

    class Parser
    {
    public:
        /// @brief Parse a list of tokens into an abstract syntax tree.
        /// @param tokens Tokens to parse.
        /// @return Abstract syntax tree.
        /// @throw UnexpectedTokenError
        static AbstractSyntaxTree parse(const TokenList &tokens, std::vector<std::shared_ptr<CompilerMessage>> &messages);

    private:
        Parser() = delete;
        ~Parser() = delete;
    };
};

#endif // _JCC_PARSER_HPP_
