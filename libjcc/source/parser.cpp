#include "parser.hpp"
#include <stack>
#include <iostream>
#include <memory>

std::string std::to_string(jcc::SyntaxNodeType type)
{
    switch (type)
    {
    case jcc::SyntaxNodeType::Identifier:
        return "Identifier";
    case jcc::SyntaxNodeType::Keyword:
        return "Keyword";
    case jcc::SyntaxNodeType::StringLiteral:
        return "StringLiteral";
    case jcc::SyntaxNodeType::NumberLiteral:
        return "NumberLiteral";
    case jcc::SyntaxNodeType::FloatingPointLiteral:
        return "FloatingPointLiteral";
    case jcc::SyntaxNodeType::Operator:
        return "Operator";
    case jcc::SyntaxNodeType::Punctuator:
        return "Punctuator";
    case jcc::SyntaxNodeType::Root:
        return "Root";
    default:
    {
        return "Unknown";
    }
    }
}

std::string std::to_string(const jcc::SyntaxNodeValue &value)
{
    std::string result = "Node(";
    result += std::to_string(value.type);
    result += ", ";

    switch (value.type)
    {
    case jcc::SyntaxNodeType::Identifier:
        result += std::get<std::string>(value.value);
        break;
    case jcc::SyntaxNodeType::Keyword:
        result += std::get<const char *>(value.value);
        break;
    case jcc::SyntaxNodeType::StringLiteral:
        result += "\"" + std::get<std::string>(value.value) + "\"";
        break;
    case jcc::SyntaxNodeType::NumberLiteral:
        result += std::to_string(std::get<uint64_t>(value.value));
        break;
    case jcc::SyntaxNodeType::FloatingPointLiteral:
        result += std::to_string(std::get<double>(value.value));
        break;
    case jcc::SyntaxNodeType::Operator:
        result += std::get<const char *>(value.value);
        break;
    case jcc::SyntaxNodeType::Punctuator:
        result += "'" + std::string(std::get<const char *>(value.value)) + "'";
        break;
    case jcc::SyntaxNodeType::Root:
        return "RootNode()";
        break;
    default:
        result += "Unknown";
        break;
    }

    result += ")";

    return result;
}

jcc::AbstractSyntaxTree jcc::Parser::parse(const jcc::TokenList &tokens, std::vector<std::shared_ptr<jcc::CompilerMessage>> &messages)
{
    /// TODO: Implement this

    AbstractSyntaxTree ast;
    
    (void)messages;
    (void)tokens;

    return ast;
}