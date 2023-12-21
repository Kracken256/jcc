#include "parser.hpp"
#include "compile.hpp"
#include <stack>
#include <string>
#include <vector>
#include <iostream>
#include <memory>

///=============================================================================
/// Common
///=============================================================================

static bool is_builtin_type(const std::string &type)
{
    // integers and floating point numbers
    if (type == "bit" || type == "byte" || type == "char" || type == "word" || type == "short" || type == "dword" || type == "int" || type == "qword" || type == "long" || type == "float" || type == "double" || type == "nint" || type == "nuint" || type == "intn" || type == "uintn")
    {
        return true;
    }

    // complex types
    if (type == "address" || type == "bigfloat" || type == "bigint" || type == "biguint" || type == "arbint" || type == "arbuint" || type == "real" || type == "complex" || type == "string" || type == "buffer" || type == "secbuffer" || type == "secstring" || type == "map" || type == "tensor" || type == "routine")
    {
        return true;
    }

    return false;
}

///=============================================================================
/// GenericNode
///=============================================================================

std::string jcc::GenericNode::json_escape(const std::string &str) const
{
    std::string escaped;
    escaped.reserve(str.size());
    for (auto &c : str)
    {
        switch (c)
        {
        case '"':
            escaped += "\\\"";
            break;
        case '\\':
            escaped += "\\\\";
            break;
        case '\b':
            escaped += "\\b";
            break;
        case '\f':
            escaped += "\\f";
            break;
        case '\n':
            escaped += "\\n";
            break;
        case '\r':
            escaped += "\\r";
            break;
        case '\t':
            escaped += "\\t";
            break;
        default:
            escaped += c;
            break;
        }
    }
    return escaped;
}

///=============================================================================
/// Block
///=============================================================================

std::string jcc::Block::to_string() const
{
    std::string str = "Block([";
    for (auto &child : m_children)
    {
        str += child->to_string() + ", ";
    }

    if (m_children.size() > 0)
    {
        str.pop_back();
        str.pop_back();
    }

    str += "])";
    return str;
}

std::string jcc::Block::to_json() const
{
    std::string str = "{\"type\":\"block\",\"children\":[";
    for (auto &child : m_children)
    {
        str += child->to_json() + ",";
    }
    if (m_children.size() > 0)
    {
        str.pop_back();
    }
    str += "]}";
    return str;
}

///=============================================================================
/// StructDeclaration
///=============================================================================

std::string jcc::StructDeclaration::to_string() const
{
    std::string str = "StructDeclaration(" + m_name + ")";
    return str;
}

std::string jcc::StructDeclaration::to_json() const
{
    std::string str = "{\"type\":\"struct_declaration\",\"name\":\"" + json_escape(m_name) + "\"}";
    return str;
}

///=============================================================================
/// UnionDeclaration
///=============================================================================

std::string jcc::UnionDeclaration::to_string() const
{
    std::string str = "UnionDeclaration(" + m_name + ")";
    return str;
}

std::string jcc::UnionDeclaration::to_json() const
{
    std::string str = "{\"type\":\"union_declaration\",\"name\":\"" + json_escape(m_name) + "\"}";
    return str;
}

///=============================================================================
/// EnumDeclaration
///=============================================================================

std::string jcc::EnumDeclaration::to_string() const
{
    std::string str = "EnumDeclaration(" + m_name + ")";
    return str;
}

std::string jcc::EnumDeclaration::to_json() const
{
    std::string str = "{\"type\":\"enum_declaration\",\"name\":\"" + json_escape(m_name) + "\"}";
    return str;
}

///=============================================================================
/// FunctionParameter
///=============================================================================

std::string jcc::FunctionParameter::to_string() const
{
    std::string str = "FunctionParameter(" + m_name + ", " + m_type + ")";
    return str;
}

std::string jcc::FunctionParameter::to_json() const
{
    std::string str = "{\"type\":\"function_parameter\",\"name\":\"" + json_escape(m_name) + "\",\"type\":\"" + json_escape(m_type) + "\"}";
    return str;
}

///=============================================================================
/// FunctionDeclaration
///=============================================================================

std::string jcc::FunctionDeclaration::to_string() const
{
    std::string str = "FunctionDeclaration(" + m_name + ", " + m_return_type + ", [";
    for (auto &parameter : m_parameters)
    {
        str += parameter->to_string() + ", ";
    }

    if (m_parameters.size() > 0)
    {
        str.pop_back();
        str.pop_back();
    }

    str += "])";
    return str;
}

std::string jcc::FunctionDeclaration::to_json() const
{
    std::string str = "{\"type\":\"function_declaration\",\"name\":\"" + json_escape(m_name) + "\",\"return_type\":\"" + json_escape(m_return_type) + "\",\"parameters\":[";
    for (auto &parameter : m_parameters)
    {
        str += parameter->to_json() + ",";
    }
    if (m_parameters.size() > 0)
    {
        str.pop_back();
    }
    str += "]}";
    return str;
}

///=============================================================================
/// ClassDeclaration
///=============================================================================

std::string jcc::ClassDeclaration::to_string() const
{
    std::string str = "ClassDeclaration(" + m_name + ")";
    return str;
}

std::string jcc::ClassDeclaration::to_json() const
{
    std::string str = "{\"type\":\"class_declaration\",\"name\":\"" + json_escape(m_name) + "\"}";
    return str;
}

std::string std::to_string(const std::shared_ptr<jcc::GenericNode> value)
{
    if (value == nullptr)
        return "{}";

    return value->to_json();
}

///=============================================================================
/// NamespaceDefinition
///=============================================================================

std::string jcc::NamespaceDefinition::to_string() const
{
    std::string str = "NamespaceDefinition(" + m_name + ", {" + m_block->to_string() + "})";
    return str;
}

std::string jcc::NamespaceDefinition::to_json() const
{
    std::string str = "{\"type\":\"namespace_definition\",\"name\":\"" + json_escape(m_name) + "\",\"block\":" + m_block->to_json() + "}";
    return str;
}

///=============================================================================
/// StructDefinition
///=============================================================================

std::string jcc::StructDefinition::to_string() const
{
    std::string str = "StructDefinition(" + m_name + ", [";
    for (const auto &child : m_fields)
    {
        str += child->to_string() + ", ";
    }

    if (m_fields.size() > 0)
    {
        str.pop_back();
        str.pop_back();
    }

    str += "])";

    return str;
}

std::string jcc::StructDefinition::to_json() const
{
    std::string str = "{\"type\":\"struct_definition\",\"name\":\"" + json_escape(m_name) + "\",\"fields\":[";
    for (const auto &child : m_fields)
    {
        str += child->to_json() + ",";
    }
    if (m_fields.size() > 0)
    {
        str.pop_back();
    }
    str += "]}";
    return str;
}

///=============================================================================
/// Parser
///=============================================================================

bool jcc::CompilationUnit::parse_block(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node)
{
    if (tokens.size() < 2)
    {
        throw SyntaxError("Expected punctuator on block");
        return false;
    }

    Token next_1 = tokens.peek(0);

    if (next_1.type() != TokenType::Punctuator || std::get<Punctuator>(next_1.value()) != Punctuator::OpenBrace)
    {
        throw SyntaxError("Expected opening brace for block");
        return false;
    }

    tokens.pop(1);

    std::shared_ptr<GenericNode> tmp;
    std::shared_ptr<Block> block = std::make_shared<Block>();

    bool is_looping = true;

    while (!tokens.eof() && is_looping)
    {
        Token curtok = tokens.peek();

        switch (curtok.type())
        {
        case TokenType::Identifier:
            break; // implement this
        case TokenType::Keyword:
            switch (std::get<jcc::Keyword>(curtok.value()))
            {
            case Keyword::Struct:
                if (!parse_struct_keyword(tokens, tmp))
                    return false;
                block->push(tmp);
                break;
            case Keyword::Namespace:
                if (!parse_namespace_keyword(tokens, tmp))
                    return false;
                block->push(tmp);
                break;
            default:
                throw SyntaxError("Unknown keyword: " + std::to_string((int)std::get<jcc::Keyword>(curtok.value())));
            }
            break;
        case TokenType::Punctuator:
            if (std::get<Punctuator>(curtok.value()) == Punctuator::CloseBrace)
            {
                is_looping = false;
                break;
            }
            break;
        case TokenType::MultiLineComment:
        case TokenType::SingleLineComment:
        case TokenType::Whitespace:
            tokens.pop();
            break; // skip comments and whitespace
        case TokenType::Unknown:
            panic("Unknown token type: " + std::to_string((int)curtok.type()), m_messages);
            break;
        default:
            panic("Unknown token type: " + std::to_string((int)curtok.type()), m_messages);
            break;
        }
    }

    Token next_2 = tokens.peek(0);

    if (next_2.type() != TokenType::Punctuator || std::get<Punctuator>(next_2.value()) != Punctuator::CloseBrace)
    {
        throw SyntaxError("Expected closing brace for block");
        return false;
    }

    tokens.pop();

    node = block;

    return true;
}

bool jcc::CompilationUnit::parse_struct_keyword(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node)
{
    if (tokens.size() < 3)
    {
        throw SyntaxError("Expected identifier after struct keyword");
        return false;
    }

    Token next_1 = tokens.peek(1);
    Token next_2 = tokens.peek(2);

    if (next_1.type() != TokenType::Identifier)
    {
        throw SyntaxError("Expected identifier after struct keyword");
        return false;
    }

    if (next_2.type() != TokenType::Punctuator)
    {
        throw SyntaxError("Expected punctuator after struct identifier");
        return false;
    }

    if (std::get<Punctuator>(next_2.value()) == Punctuator::Semicolon)
    {
        std::shared_ptr<StructDeclaration> struct_ptr = std::make_shared<StructDeclaration>(std::get<std::string>(next_1.value()));
        node = struct_ptr;
        tokens.pop(3);

        return true;
    }
    else if (std::get<Punctuator>(next_2.value()) != Punctuator::OpenBrace)
    {
        throw SyntaxError("Expected punctuator after struct identifier");
        return false;
    }

    /*
        varname: type [:bitfield] [ = default value]
    */

    tokens.pop(3);

    if (tokens.size() < 1)
    {
        throw SyntaxError("Expected closing brace for struct");
        return false;
    }

    // should be identifier
    std::vector<std::shared_ptr<StructField>> fields;

    while (1)
    {
        Token curtok = tokens.peek();
        std::shared_ptr<StructField> field = std::make_shared<StructField>();

        if (curtok.type() == TokenType::Punctuator && std::get<Punctuator>(curtok.value()) == Punctuator::CloseBrace)
        {
            tokens.pop();
            break;
        }

        // name
        if (curtok.type() != TokenType::Identifier)
        {
            throw SyntaxError("Expected identifier in struct field");
            return false;
        }
        field->name() = std::get<std::string>(curtok.value());
        tokens.pop();
        if (tokens.eof())
        {
            throw SyntaxError("Expected seperator in struct field");
            return false;
        }

        curtok = tokens.peek();
        if (curtok.type() != TokenType::Punctuator && std::get<Punctuator>(curtok.value()) != Punctuator::Colon)
        {
            throw SyntaxError("Expected seperator in struct field");
            return false;
        }
        tokens.pop();

        if (tokens.eof())
        {
            throw SyntaxError("Expected type in struct field");
            return false;
        }

        // type
        curtok = tokens.peek();
        if (curtok.type() != TokenType::Identifier && curtok.type() != TokenType::Keyword)
        {
            throw SyntaxError("Expected type in struct field");
            return false;
        }

        switch (curtok.type())
        {
        case TokenType::Identifier:
            field->type() = std::get<std::string>(curtok.value());
            break;
        case TokenType::Keyword:
            // check if
            if (is_builtin_type(lexKeywordMapReverse.at(std::get<Keyword>(curtok.value()))))
            {
                field->type() = lexKeywordMapReverse.at(std::get<Keyword>(curtok.value()));
            }
            else
            {
                throw SyntaxError("Expected type in struct field");
                return false;
            }

        default:
            break;
        }

        tokens.pop();
        if (tokens.eof())
        {
            throw SyntaxError("Expected seperator in struct field");
            return false;
        }

        curtok = tokens.peek();

        if (curtok.type() == TokenType::Punctuator && std::get<Punctuator>(curtok.value()) == Punctuator::Colon)
        {
            tokens.pop();

            if (tokens.eof())
            {
                throw SyntaxError("Expected bitfield in struct field");
                return false;
            }

            curtok = tokens.peek();

            if (curtok.type() != TokenType::NumberLiteral)
            {
                throw SyntaxError("Expected bitfield in struct field");
                return false;
            }

            field->bitfield() = std::get<uint64_t>(curtok.value());
            tokens.pop();
        }

        if (tokens.eof())
        {
            throw SyntaxError("Expected seperator in struct field");
            return false;
        }

        curtok = tokens.peek();
        if (curtok.type() == TokenType::Operator && std::get<Operator>(curtok.value()) == Operator::Assign)
        {
            tokens.pop();

            if (tokens.eof())
            {
                throw SyntaxError("Expected default value in struct field");
                return false;
            }

            curtok = tokens.peek();

            if (curtok.type() != TokenType::StringLiteral && curtok.type() != TokenType::NumberLiteral && curtok.type() != TokenType::FloatingPointLiteral)
            {
                throw SyntaxError("Expected default value in struct field");
                return false;
            }

            switch (curtok.type())
            {
            case TokenType::StringLiteral:
                field->default_value() = "\"" + std::get<std::string>(curtok.value()) + "\"";
                break;
            case TokenType::NumberLiteral:
                field->default_value() = std::to_string(std::get<uint64_t>(curtok.value()));
                break;
            case TokenType::FloatingPointLiteral:
                field->default_value() = std::to_string(std::get<double>(curtok.value()));
                break;

            default:
                break;
            }
            tokens.pop();
        }

        fields.push_back(field);
    }

    node = std::make_shared<StructDefinition>(std::get<std::string>(next_1.value()), fields);

    if (fields.empty())
    {
        push_message(CompilerMessageType::Warning, "Empty structure was defined. This is is undefined behavior.");
    }

    return true;
}

bool jcc::CompilationUnit::parse_namespace_keyword(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node)
{
    if (tokens.size() < 3)
    {
        throw SyntaxError("Expected identifier after namespace keyword");
        return false;
    }

    Token next_1 = tokens.peek(1);
    Token next_2 = tokens.peek(2);

    if (next_1.type() != TokenType::Identifier)
    {
        throw SyntaxError("Expected identifier after namespace keyword");
        return false;
    }

    if (next_2.type() != TokenType::Punctuator)
    {
        throw SyntaxError("Expected punctuator after namespace identifier");
        return false;
    }

    if (std::get<Punctuator>(next_2.value()) == Punctuator::Semicolon)
    {
        std::shared_ptr<NamespaceDeclaration> namespace_ptr = std::make_shared<NamespaceDeclaration>(std::get<std::string>(next_1.value()));
        node = namespace_ptr;
        tokens.pop(2);
        return true;
    }
    else if (std::get<Punctuator>(next_2.value()) != Punctuator::OpenBrace)
    {
        throw SyntaxError("Expected punctuator after namespace identifier");
        return false;
    }

    tokens.pop(2);

    std::shared_ptr<GenericNode> block = std::make_shared<Block>();

    if (!parse_block(tokens, block))
    {
        return false;
    }

    node = std::make_shared<NamespaceDefinition>(std::get<std::string>(next_1.value()), std::static_pointer_cast<Block>(block));

    return true;
}

bool jcc::CompilationUnit::parse_code(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node)
{
    while (!tokens.eof())
    {
        Token curtok = tokens.peek();

        switch (curtok.type())
        {
        case TokenType::Identifier:
            break; // implement this
        case TokenType::Keyword:
            switch (std::get<jcc::Keyword>(curtok.value()))
            {
            case Keyword::Struct:
                if (!parse_struct_keyword(tokens, node))
                    return false;
                break;
            case Keyword::Namespace:
                if (!parse_namespace_keyword(tokens, node))
                    return false;
                break;
            default:
                throw SyntaxError("Unknown keyword: " + std::to_string((int)std::get<jcc::Keyword>(curtok.value())));
                break;
            }
            break;
        case TokenType::NumberLiteral:
            break; // implement this
        case TokenType::FloatingPointLiteral:
            break; // implement this
        case TokenType::StringLiteral:
            break; // implement this
        case TokenType::Operator:
            break; // implement this
        case TokenType::Punctuator:
            break; // implement this
        case TokenType::Unknown:
            panic("Unknown token type: " + std::to_string((int)curtok.type()), m_messages);
            break;
        default:
            panic("Unknown token type: " + std::to_string((int)curtok.type()), m_messages);
            break;
        }
    }

    return true;
}

std::shared_ptr<jcc::AbstractSyntaxTree> jcc::CompilationUnit::parse(const jcc::TokenList &tokens)
{
    TokenList tokens_copy = tokens;

    // remove whitespace and comments
    auto shouldRemove = [](const Token &token)
    {
        return token.type() == TokenType::Whitespace ||
               token.type() == TokenType::SingleLineComment ||
               token.type() == TokenType::MultiLineComment;
    };

    // Use remove_if along with erase to remove elements matching the condition
    tokens_copy.m_tokens.erase(std::remove_if(tokens_copy.m_tokens.begin(), tokens_copy.m_tokens.end(), shouldRemove), tokens_copy.m_tokens.end());

    std::shared_ptr<GenericNode> rootnode;

    if (!parse_code(tokens_copy, rootnode))
    {
        return nullptr;
    }

    if (rootnode == nullptr)
    {
        return nullptr;
    }

    std::shared_ptr<AbstractSyntaxTree> nodes = std::make_shared<AbstractSyntaxTree>();

    nodes->root() = rootnode;

    return nodes;
}