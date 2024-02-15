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
namespace jcc
{
    enum class FunctionParseMode
    {
        DeclarationOnly,
        DefinitionOnly,
        DeclarationOrDefinition,
    };

    struct ExpNode
    {
        jcc::Token value;
        std::vector<ExpNode> children;
        ExpNode() = default;
        ExpNode(const jcc::Token &value, const std::vector<ExpNode> &children) : value(value), children(children) {}

        std::string print_tree(const ExpNode node, const std::string prefix, bool is_tail, std::string str) const
        {
            str += prefix + (is_tail ? "└── " : "├── ");
            switch (node.value.type())
            {
            case jcc::TokenType::Identifier:
                str += std::get<std::string>(value.value());
                break;
            case jcc::TokenType::Operator:
                str += jcc::lexOperatorMapReverse.at(std::get<jcc::Operator>(value.value()));
                break;
            case jcc::TokenType::NumberLiteral:
                str += std::get<std::string>(value.value());
                break;
            case jcc::TokenType::StringLiteral:
                str += std::get<std::string>(value.value());
                break;
            case jcc::TokenType::Keyword:
                str += lexKeywordMapReverse.at(std::get<jcc::Keyword>(value.value()));
                break;
            case jcc::TokenType::Punctuator:
                str += jcc::lexPunctuatorMapReverse.at(std::get<jcc::Punctuator>(value.value()));
                break;
            case jcc::TokenType::FloatingPointLiteral:
                str += std::get<std::string>(value.value());
                break;
            default:
                str += "Unknown";
                break;
            }
            str += "\n";
            for (size_t i = 0; i < node.children.size(); i++)
            {
                str = print_tree(node.children[i], prefix + (is_tail ? "    " : "│   "), i == node.children.size() - 1, str);
            }
            return str;
        }

        std::string to_string() const
        {
            return print_tree(*this, "", true, "");
        }
    };
}

static bool is_builtin_type(const std::string &type)
{
    static std::set<const char *> builtin_types = {
        "bit",
        "byte",
        "char",
        "word",
        "short",
        "dword",
        "int",
        "qword",
        "long",
        "intn",
        "uintn",

        "float",
        "double",

        "address",
        "routine",

        "real",
        "complex",

        "string",

        "void",
    };

    return builtin_types.contains(type.c_str());
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

std::string jcc::StructDeclaration::to_json() const
{
    std::string str = "{\"type\":\"struct_declaration\",\"name\":\"" + json_escape(m_name) + "\"}";
    return str;
}

std::string jcc::FunctionParameter::to_json() const
{
    std::string str = "{\"type\":\"function_parameter\",\"name\":\"" + json_escape(m_name) + "\",\"dtype\":\"" + json_escape(m_type) + "\"}";
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

std::string std::to_string(const std::shared_ptr<jcc::GenericNode> value)
{
    if (value == nullptr)
        return "{}";

    return value->to_json();
}

std::string jcc::SubsystemDefinition::to_json() const
{
    std::string str = "{\"type\":\"subsystem_definition\",\"name\":\"" + json_escape(m_name) + "\",\"block\":" + m_block->to_json() + "}";
    return str;
}

std::string jcc::StructField::to_json() const
{
    std::string str = "{\"type\":\"struct_field\",\"name\":\"" + json_escape(m_name) + "\",\"dtype\":\"" + json_escape(m_type) + "\",\"default_value\":\"" + json_escape(m_default_value) + "\",\"bitfield\":" + std::to_string(m_bitfield) + ",\"arr_size\":";
    if (!m_arr_size)
    {
        str += "\"dynamic\"";
    }
    else
    {
        str += std::to_string(m_arr_size);
    }
    str += ",\"attributes\":[";
    for (auto &attribute : m_attributes)
    {
        str += attribute->to_json();

        if (attribute != m_attributes.back())
        {
            str += ",";
        }
    }

    str += "]}";

    return str;
}

std::string jcc::StructMethod::to_json() const
{
    std::string str = "{\"type\":\"struct_member\",\"name\":\"" + json_escape(m_name) + "\",\"return_type\":\"" + json_escape(m_type) + "\",\"parameters\":[";
    for (auto &parameter : m_parameters)
    {
        str += parameter->to_json() + ",";
    }
    if (m_parameters.size() > 0)
    {
        str.pop_back();
    }
    str += "],\"block\":" + m_block->to_json() + "}";
    return str;
}

std::string jcc::StructDefinition::to_json() const
{
    std::string str = "{\"type\":\"struct_definition\",\"name\":\"" + json_escape(m_name) + "\",\"fields\":[";
    for (const auto &child : m_fields)
    {
        str += child->to_json() + ",";
    }
    for (const auto &child : m_methods)
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

std::string jcc::FunctionDefinition::to_json() const
{
    std::string str = "{\"type\":\"function_definition\",\"name\":\"" + json_escape(m_name) + "\",\"return_type\":\"" + json_escape(m_return_type) + "\",\"parameters\":[";
    for (auto &parameter : m_parameters)
    {
        str += parameter->to_json() + ",";
    }
    if (m_parameters.size() > 0)
    {
        str.pop_back();
    }
    str += "],\"block\":" + m_block->to_json() + "}";
    return str;
}

std::string jcc::CallExpression::to_json() const
{
    std::string str = "{\"type\":\"call_expression\",\"name\":\"" + json_escape(m_name) + "\",\"arguments\":[";
    for (auto &child : m_arguments)
    {
        str += child->to_json() + ",";
    }
    if (m_arguments.size() > 0)
    {
        str.pop_back();
    }
    str += "]}";
    return str;
}

std::string jcc::TypeNode::to_json() const
{
    std::string str = "{\"type\":\"type_node\",\"name\":\"" + json_escape(m_typename) + "\",\"is_mutable\":" + std::to_string(m_is_mutable) + ",\"is_ref\":" + std::to_string(m_is_reference) + ",";
    if (m_arr_size == std::numeric_limits<size_t>::max())
    {
        str += "\"arr_size\":\"dynamic\"";
    }
    else
    {
        str += "\"arr_size\":" + std::to_string(m_arr_size);
    }
    str += ",\"bitfield\":" + std::to_string(m_bitfield);

    if (m_default_value)
    {
        str += ",\"default_value\":" + m_default_value->to_json();
    }
    str += "}";
    return str;
}

std::string jcc::LetDeclaration::to_json() const
{
    std::string str = "{\"type\":\"let_declaration\",\"name\":\"" + json_escape(m_name) + "\",\"dtype\":" + m_type->to_json() + "}";

    return str;
}

std::string jcc::VarDeclaration::to_json() const
{
    std::string str = "{\"type\":\"var_declaration\",\"name\":\"" + json_escape(m_name) + "\",\"dtype\":" + m_type->to_json() + "}";

    return str;
}

std::string jcc::ConstDeclaration::to_json() const
{
    std::string str = "{\"type\":\"const_declaration\",\"name\":\"" + json_escape(m_name) + "\",\"dtype\":" + m_type->to_json() + "}";

    return str;
}

namespace jcc
{

    static bool parse_struct_keyword(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node, bool packed);
    static bool parse_class_keyword(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node);
    static bool parse_enum_keyword(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node);
    static bool parse_typedef_keyword(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node);
    static bool parse_subsystem_keyword(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node);
    static bool parse_function_parameters(jcc::TokenList &tokens, std::vector<std::shared_ptr<jcc::FunctionParameter>> &params);
    static bool parse_func_keyword(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node, jcc::FunctionParseMode mode);
    static bool parse_return_keyword(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node);
    static bool parse_block(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node, bool functional);
    static bool parse_expression(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node);
    static bool parse_expression_helper(jcc::TokenList &tokens, jcc::ExpNode &output);
    static bool parse_structural_block(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node);
    static bool parse_functional_block(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node);
    static bool parse_var_keyword(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node);
    static bool parse_let_keyword(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node);
    static bool parse_const_keyword(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node);
    static bool parse_export_keyword(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node);
    static bool parse_import_keyword(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node);
    static bool parse_volatile_keyword(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node);
    static bool parse_type(jcc::TokenList &tokens, bool allow_bitfield, bool allow_arr_size, bool allow_default_value, std::shared_ptr<jcc::TypeNode> &node);
}

bool jcc::parse_type(jcc::TokenList &tokens, bool allow_bitfield, bool allow_arr_size, bool allow_default_value, std::shared_ptr<jcc::TypeNode> &node)
{
    // [const] [ref] {typename} [[arr_size]|bitfield] [= default_value]

    bool is_const = false;
    bool is_ref = false;
    bool is_bitfield = false;
    std::string type;
    std::shared_ptr<GenericNode> default_value;
    size_t arr_size = 0;
    size_t bitfield = 0;

    if (tokens.size() < 1)
    {
        throw SyntaxError("Expected typename");
        return false;
    }

    // check for const
    Token curtok = tokens.peek();
    if (curtok.type() == TokenType::Keyword && std::get<Keyword>(curtok.value()) == Keyword::Const)
    {
        is_const = true;
        tokens.pop();
        if (tokens.eof())
        {
            throw SyntaxError("Expected typename");
            return false;
        }
        curtok = tokens.peek();
    }

    // check for ref
    if (curtok.type() == TokenType::Keyword && std::get<Keyword>(curtok.value()) == Keyword::Ref)
    {
        is_ref = true;
        tokens.pop();
        if (tokens.eof())
        {
            throw SyntaxError("Expected typename");
            return false;
        }
        curtok = tokens.peek();
    }

    // check for typename
    if (curtok.type() == TokenType::Identifier)
    {
        type = std::get<std::string>(curtok.value());
        tokens.pop();
        if (tokens.eof())
        {
            throw SyntaxError("Expected typename");
            return false;
        }
        curtok = tokens.peek();
    }
    else if (curtok.type() == TokenType::Keyword)
    {
        type = lexKeywordMapReverse.at(std::get<Keyword>(curtok.value()));
        tokens.pop();
        if (tokens.eof())
        {
            throw SyntaxError("Expected typename");
            return false;
        }
        curtok = tokens.peek();
    }
    else
    {
        throw SyntaxError("Expected typename");
        return false;
    }

    // check for bitfield
    if (allow_bitfield && curtok.type() == TokenType::Punctuator && std::get<Punctuator>(curtok.value()) == Punctuator::Colon)
    {
        is_bitfield = true;
        tokens.pop();
        if (tokens.eof())
        {
            throw SyntaxError("Expected bitfield");
            return false;
        }
        curtok = tokens.peek();
        if (curtok.type() != TokenType::NumberLiteral)
        {
            throw SyntaxError("Expected bitfield");
            return false;
        }
        bitfield = std::stoll(std::get<std::string>(curtok.value()));
        tokens.pop();
        if (tokens.eof())
        {
            throw SyntaxError("Expected typename");
            return false;
        }
        curtok = tokens.peek();
    }

    // check for arr_size
    if (allow_arr_size && curtok.type() == TokenType::Punctuator && std::get<Punctuator>(curtok.value()) == Punctuator::OpenBracket)
    {
        // we can't have both bitfield and arr_size
        if (is_bitfield)
        {
            throw SyntaxError("Unexpected arr_size");
            return false;
        }

        tokens.pop();
        if (tokens.eof())
        {
            throw SyntaxError("Expected arr_size");
            return false;
        }
        curtok = tokens.peek();
        if (curtok.type() == TokenType::Punctuator && std::get<Punctuator>(curtok.value()) == Punctuator::CloseBracket)
        {
            // dynamic array
            tokens.pop();
            if (tokens.eof())
            {
                throw SyntaxError("Expected typename");
                return false;
            }
            curtok = tokens.peek();
            arr_size = std::numeric_limits<size_t>::max();
        }
        else if (curtok.type() == TokenType::NumberLiteral)
        {
            // fixed array
            arr_size = std::stoll(std::get<std::string>(curtok.value()));
            tokens.pop();
            if (tokens.eof())
            {
                throw SyntaxError("Expected typename");
                return false;
            }
            curtok = tokens.peek();
            if (curtok.type() != TokenType::Punctuator || std::get<Punctuator>(curtok.value()) != Punctuator::CloseBracket)
            {
                throw SyntaxError("Expected closing bracket");
                return false;
            }
            tokens.pop();
            if (tokens.eof())
            {
                throw SyntaxError("Expected typename");
                return false;
            }
            curtok = tokens.peek();
        }
        else
        {
            throw SyntaxError("Expected arr_size");
            return false;
        }
    }

    // check for default value
    if (allow_default_value && curtok.type() == TokenType::Operator && std::get<Operator>(curtok.value()) == Operator::Assign)
    {
        tokens.pop();
        if (!parse_expression(tokens, default_value))
        {
            throw SyntaxError("Expected default value");
            return false;
        }
    }

    node = std::make_shared<TypeNode>(type, is_const, is_ref, arr_size, bitfield, std::static_pointer_cast<Expression>(default_value));

    return true;
}

static bool jcc::parse_structural_block(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node)
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
            throw SyntaxError("Unexpected identifier: " + std::get<std::string>(curtok.value()));
            break; // implement this
        case TokenType::Keyword:
            switch (std::get<jcc::Keyword>(curtok.value()))
            {
            case Keyword::Subsystem:
                if (!parse_subsystem_keyword(tokens, tmp))
                    return false;
                block->push(tmp);
                break;

            case Keyword::Import:
                if (!parse_import_keyword(tokens, tmp))
                    return false;
                block->push(tmp);
                break;
            case Keyword::Export:
                if (!parse_export_keyword(tokens, tmp))
                    return false;
                block->push(tmp);
                break;
            case Keyword::Let:
                if (!parse_let_keyword(tokens, tmp))
                    return false;
                block->push(tmp);
                break;
            case Keyword::Var:
                if (!parse_var_keyword(tokens, tmp))
                    return false;
                block->push(tmp);
                break;
            case Keyword::Const:
                if (!parse_const_keyword(tokens, tmp))
                    return false;
                block->push(tmp);
                break;

            case Keyword::Struct:
                if (!parse_struct_keyword(tokens, tmp, false))
                    return false;
                block->push(tmp);
                break;
            case Keyword::Region:
                if (!parse_struct_keyword(tokens, tmp, true))
                    return false;
                block->push(tmp);
                break;

            case Keyword::Func:
                if (!parse_func_keyword(tokens, tmp, FunctionParseMode::DeclarationOrDefinition))
                    return false;
                block->push(tmp);
                break;

            case Keyword::Typedef:
                if (!parse_typedef_keyword(tokens, tmp))
                    return false;
                block->push(tmp);
                break;
            case Keyword::Volatile:
                if (!parse_volatile_keyword(tokens, tmp))
                    return false;
                block->push(tmp);
                break;

            case Keyword::Class:
                if (!parse_class_keyword(tokens, tmp))
                    return false;
                block->push(tmp);
                break;

            case Keyword::Enum:
                if (!parse_enum_keyword(tokens, tmp))
                    return false;
                block->push(tmp);
                break;
            default:
                throw SyntaxError("Unexpected keyword: " + std::string(lexKeywordMapReverse.at(std::get<jcc::Keyword>(curtok.value()))));
            }
            break;
        case TokenType::Punctuator:
            switch (std::get<Punctuator>(curtok.value()))
            {
            case Punctuator::OpenBrace:
                throw SyntaxError("Unexpected opening brace");
                break;
            case Punctuator::CloseBrace:
                is_looping = false;
                break;
            case Punctuator::OpenParen:
                throw SyntaxError("Unexpected opening parenthesis");
                break;
            case Punctuator::CloseParen:
                throw SyntaxError("Unexpected closing parenthesis");
                break;
            case Punctuator::OpenBracket:
                throw SyntaxError("Unexpected opening bracket");
                break;
            case Punctuator::CloseBracket:
                throw SyntaxError("Unexpected closing bracket");
                break;
            case Punctuator::Semicolon:
                // ignore semicolons
                tokens.pop();
                break;
            case Punctuator::Colon:
                throw SyntaxError("Unexpected colon");
                break;
            case Punctuator::Comma:
                throw SyntaxError("Unexpected comma");
                break;
            case Punctuator::Period:
                throw SyntaxError("Unexpected period");
                break;

            default:
                throw SyntaxError("Unexpected punctuator: " + std::string(lexPunctuatorMapReverse.at(std::get<Punctuator>(curtok.value()))));
            }
            break;
        case TokenType::MultiLineComment:
        case TokenType::SingleLineComment:
        case TokenType::Whitespace:
            tokens.pop();
            break; // skip comments and whitespace
        case TokenType::Raw:
            block->push(std::make_shared<RawNode>(std::get<std::string>(curtok.value())));
            tokens.pop();
            break;
        default:
            panic("Unknown token type: " + std::to_string((int)curtok.type()));
            break;
        }
    }

    if (tokens.eof())
    {
        throw SyntaxError("Expected closing brace for block");
        return false;
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

static bool jcc::parse_functional_block(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node)
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
            throw SyntaxError("Unexpected identifier: " + std::get<std::string>(curtok.value()));
            break; // implement this
        case TokenType::Keyword:
            switch (std::get<jcc::Keyword>(curtok.value()))
            {
            case Keyword::Import:
                if (!parse_import_keyword(tokens, tmp))
                    return false;
                block->push(tmp);
                break;

            case Keyword::Let:
                if (!parse_let_keyword(tokens, tmp))
                    return false;
                block->push(tmp);
                break;
            case Keyword::Var:
                if (!parse_var_keyword(tokens, tmp))
                    return false;
                block->push(tmp);
                break;
            case Keyword::Const:
                if (!parse_const_keyword(tokens, tmp))
                    return false;
                block->push(tmp);
                break;

            case Keyword::Volatile:
                if (!parse_volatile_keyword(tokens, tmp))
                    return false;
                block->push(tmp);
                break;

            default:
                throw SyntaxError("Unexpected keyword: " + std::string(lexKeywordMapReverse.at(std::get<jcc::Keyword>(curtok.value()))));
            }
            break;
        case TokenType::Punctuator:
            switch (std::get<Punctuator>(curtok.value()))
            {
            case Punctuator::OpenBrace:
                throw SyntaxError("Unexpected opening brace");
                break;
            case Punctuator::CloseBrace:
                is_looping = false;
                break;
            case Punctuator::OpenParen:
                throw SyntaxError("Unexpected opening parenthesis");
                break;
            case Punctuator::CloseParen:
                throw SyntaxError("Unexpected closing parenthesis");
                break;
            case Punctuator::OpenBracket:
                throw SyntaxError("Unexpected opening bracket");
                break;
            case Punctuator::CloseBracket:
                throw SyntaxError("Unexpected closing bracket");
                break;
            case Punctuator::Semicolon:
                // ignore semicolons
                tokens.pop();
                break;
            case Punctuator::Colon:
                throw SyntaxError("Unexpected colon");
                break;
            case Punctuator::Comma:
                throw SyntaxError("Unexpected comma");
                break;
            case Punctuator::Period:
                throw SyntaxError("Unexpected period");
                break;

            default:
                throw SyntaxError("Unexpected punctuator: " + std::string(lexPunctuatorMapReverse.at(std::get<Punctuator>(curtok.value()))));
            }
            break;
        case TokenType::MultiLineComment:
        case TokenType::SingleLineComment:
        case TokenType::Whitespace:
            tokens.pop();
            break; // skip comments and whitespace
        case TokenType::Raw:
            block->push(std::make_shared<RawNode>(std::get<std::string>(curtok.value())));
            tokens.pop();
            break;
        default:
            panic("Unknown token type: " + std::to_string((int)curtok.type()));
            break;
        }
    }

    if (tokens.eof())
    {
        throw SyntaxError("Expected closing brace for block");
        return false;
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

static bool jcc::parse_var_keyword(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node)
{
    if (tokens.size() < 2)
    {
        throw SyntaxError("Expected identifier after var keyword");
        return false;
    }

    tokens.pop();

    Token curtok = tokens.peek();
    if (curtok.type() != TokenType::Identifier)
    {
        throw SyntaxError("Expected identifier after var keyword");
        return false;
    }
    std::string name = std::get<std::string>(curtok.value());
    tokens.pop();
    if (tokens.eof())
    {
        throw SyntaxError("Expected typename after var keyword");
        return false;
    }
    curtok = tokens.peek();
    if (curtok.type() != TokenType::Punctuator || std::get<Punctuator>(curtok.value()) != Punctuator::Colon)
    {
        throw SyntaxError("Expected colon after var keyword");
        return false;
    }
    tokens.pop();

    std::shared_ptr<TypeNode> type;

    if (!parse_type(tokens, false, true, true, type))
    {
        throw SyntaxError("Expected typename after var keyword");
        return false;
    }

    node = std::make_shared<VarDeclaration>(type, name);

    return true;
}

static bool jcc::parse_let_keyword(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node)
{
    if (tokens.size() < 2)
    {
        throw SyntaxError("Expected identifier after let keyword");
        return false;
    }

    tokens.pop();

    Token curtok = tokens.peek();
    if (curtok.type() != TokenType::Identifier)
    {
        throw SyntaxError("Expected identifier after let keyword");
        return false;
    }
    std::string name = std::get<std::string>(curtok.value());
    tokens.pop();
    if (tokens.eof())
    {
        throw SyntaxError("Expected typename after let keyword");
        return false;
    }
    curtok = tokens.peek();
    if (curtok.type() != TokenType::Punctuator || std::get<Punctuator>(curtok.value()) != Punctuator::Colon)
    {
        throw SyntaxError("Expected colon after let keyword");
        return false;
    }
    tokens.pop();

    std::shared_ptr<TypeNode> type;

    if (!parse_type(tokens, false, true, true, type))
    {
        throw SyntaxError("Expected typename after let keyword");
        return false;
    }

    node = std::make_shared<LetDeclaration>(type, name);

    return true;
}

static bool jcc::parse_const_keyword(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node)
{
    if (tokens.size() < 2)
    {
        throw SyntaxError("Expected identifier after const keyword");
        return false;
    }

    tokens.pop();

    Token curtok = tokens.peek();
    if (curtok.type() != TokenType::Identifier)
    {
        throw SyntaxError("Expected identifier after const keyword");
        return false;
    }
    std::string name = std::get<std::string>(curtok.value());
    tokens.pop();
    if (tokens.eof())
    {
        throw SyntaxError("Expected typename after const keyword");
        return false;
    }
    curtok = tokens.peek();
    if (curtok.type() != TokenType::Punctuator || std::get<Punctuator>(curtok.value()) != Punctuator::Colon)
    {
        throw SyntaxError("Expected colon after const keyword");
        return false;
    }
    tokens.pop();

    std::shared_ptr<TypeNode> type;

    if (!parse_type(tokens, false, true, true, type))
    {
        throw SyntaxError("Expected typename after const keyword");
        return false;
    }

    node = std::make_shared<ConstDeclaration>(type, name);

    return true;
}

static bool jcc::parse_import_keyword(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node)
{
    (void)tokens;
    (void)node;
    /// TODO: implement
    return false;
}

static bool jcc::parse_export_keyword(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node)
{
    (void)tokens;
    (void)node;
    /// TODO: implement
    return false;
}

static bool jcc::parse_volatile_keyword(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node)
{
    (void)tokens;
    (void)node;
    /// TODO: implement
    return false;
}

static bool jcc::parse_block(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node, bool functional)
{
    return functional ? parse_functional_block(tokens, node) : parse_structural_block(tokens, node);
}

static bool jcc::parse_struct_keyword(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node, bool packed)
{
    using namespace jcc;

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

    std::shared_ptr<StructField> field = std::make_shared<StructField>();
    std::vector<std::shared_ptr<StructMethod>> functions;

    while (1)
    {
        Token curtok = tokens.peek();

        if (curtok.type() == TokenType::Punctuator && std::get<Punctuator>(curtok.value()) == Punctuator::CloseBrace)
        {
            tokens.pop();
            break;
        }

        if (curtok.type() == TokenType::Operator && std::get<Operator>(curtok.value()) == Operator::At)
        {
            tokens.pop();
            if (tokens.eof())
            {
                throw SyntaxError("Expected attribute in struct field");
                return false;
            }

            curtok = tokens.peek();
            if (curtok.type() != TokenType::Punctuator || std::get<Punctuator>(curtok.value()) != Punctuator::Colon)
            {
                throw SyntaxError("Expected attribute in struct field");
                return false;
            }
            tokens.pop();
            if (tokens.eof())
            {
                throw SyntaxError("Expected attribute name in struct field");
                return false;
            }

            curtok = tokens.peek();
            if (curtok.type() != TokenType::Identifier)
            {
                throw SyntaxError("Expected attribute name in struct field");
                return false;
            }
            tokens.pop();

            std::shared_ptr<StructAttribute> attribute = std::make_shared<StructAttribute>();
            attribute->name() = std::get<std::string>(curtok.value());

            if (tokens.eof())
            {
                throw SyntaxError("Expected attribute value in struct field");
                return false;
            }
            curtok = tokens.peek();
            if (curtok.type() != TokenType::Punctuator || std::get<Punctuator>(curtok.value()) != Punctuator::OpenParen)
            {
                throw SyntaxError("Expected attribute value in struct field");
                return false;
            }
            tokens.pop();

            if (tokens.eof())
            {
                throw SyntaxError("Expected attribute value in struct field");
                return false;
            }
            curtok = tokens.peek();
            if (curtok.type() != TokenType::StringLiteral && curtok.type() != TokenType::NumberLiteral)
            {
                throw SyntaxError("Expected attribute value in struct field");
                return false;
            }

            switch (curtok.type())
            {
            case TokenType::StringLiteral:
                attribute->value() = "\"" + std::get<std::string>(curtok.value()) + "\"";
                break;
            case TokenType::NumberLiteral:
                attribute->value() = std::get<std::string>(curtok.value());
                break;
            default:
                break;
            }

            tokens.pop();

            if (tokens.eof())
            {
                throw SyntaxError("Expected attribute value in struct field");
                return false;
            }

            curtok = tokens.peek();

            if (curtok.type() != TokenType::Punctuator || std::get<Punctuator>(curtok.value()) != Punctuator::CloseParen)
            {
                throw SyntaxError("Expected attribute value in struct field");
                return false;
            }

            tokens.pop();

            field->attributes().push_back(attribute);
        }
        else
        {
            if (curtok.type() == TokenType::Punctuator && std::get<Punctuator>(curtok.value()) == Punctuator::Semicolon)
            {
                tokens.pop();
                continue;
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
            if (curtok.type() != TokenType::Identifier && curtok.type() != TokenType::Keyword && curtok.type() != TokenType::Punctuator)
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
                break;
            case TokenType::Punctuator:
            {
                std::vector<std::shared_ptr<FunctionParameter>> params;
                if (!parse_function_parameters(tokens, params))
                {
                    return false;
                }

                if (tokens.eof())
                {
                    throw SyntaxError("Expected block in struct field");
                    return false;
                }

                curtok = tokens.peek();

                std::string return_type = "void";

                if (curtok.type() == TokenType::Punctuator && std::get<Punctuator>(curtok.value()) == Punctuator::Colon)
                {
                    tokens.pop();

                    if (tokens.eof())
                    {
                        throw SyntaxError("Expected type in struct field");
                        return false;
                    }

                    curtok = tokens.peek();

                    if (curtok.type() != TokenType::Identifier && curtok.type() != TokenType::Keyword)
                    {
                        throw SyntaxError("Expected type in struct field");
                        return false;
                    }

                    switch (curtok.type())
                    {
                    case TokenType::Identifier:
                        return_type = std::get<std::string>(curtok.value());
                        break;
                    case TokenType::Keyword:
                        // check if
                        if (is_builtin_type(lexKeywordMapReverse.at(std::get<Keyword>(curtok.value()))))
                        {
                            return_type = lexKeywordMapReverse.at(std::get<Keyword>(curtok.value()));
                        }
                        else
                        {
                            throw SyntaxError("Expected type in struct field");
                            return false;
                        }
                        break;
                    default:
                        break;
                    }

                    if (tokens.eof())
                    {
                        throw SyntaxError("Expected block in struct field");
                        return false;
                    }
                    tokens.pop();
                    curtok = tokens.peek();
                }

                std::shared_ptr<GenericNode> block;
                if (!parse_block(tokens, block, true))
                {
                    return false;
                }

                std::shared_ptr<StructMethod> function = std::make_shared<StructMethod>(field->name(), return_type, params, std::static_pointer_cast<Block>(block));
                functions.push_back(function);
                continue;
            }
            break;

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

            if (curtok.type() == TokenType::Punctuator && std::get<Punctuator>(curtok.value()) == Punctuator::OpenBracket)
            {
                if (tokens.eof())
                {
                    throw SyntaxError("Expected number in array size");
                    return false;
                }

                tokens.pop();

                curtok = tokens.peek();

                if (curtok.type() == TokenType::Punctuator && std::get<Punctuator>(curtok.value()) == Punctuator::CloseBracket)
                {
                    field->arr_size() = std::numeric_limits<uint64_t>::max();
                    goto skip_array_size;
                }

                if (curtok.type() != TokenType::NumberLiteral)
                {
                    throw SyntaxError("Expected number in array size");
                    return false;
                }

                field->arr_size() = std::stoi(std::get<std::string>(curtok.value()));
                tokens.pop();

                if (tokens.eof())
                {
                    throw SyntaxError("Expected closing bracket in array size");
                    return false;
                }

                curtok = tokens.peek();

                if (curtok.type() != TokenType::Punctuator || std::get<Punctuator>(curtok.value()) != Punctuator::CloseBracket)
                {
                    throw SyntaxError("Expected closing bracket in array size");
                    return false;
                }

            skip_array_size:

                tokens.pop();

                if (tokens.eof())
                {
                    throw SyntaxError("Expected seperator in struct field");
                    return false;
                }

                curtok = tokens.peek();
            }

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

                field->bitfield() = std::stoi(std::get<std::string>(curtok.value()));
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

                if (curtok.type() != TokenType::StringLiteral && curtok.type() != TokenType::NumberLiteral && curtok.type() != TokenType::FloatingPointLiteral && curtok.type() != TokenType::Identifier && curtok.type() != TokenType::Keyword)
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
                    field->default_value() = std::get<std::string>(curtok.value());
                    break;
                case TokenType::FloatingPointLiteral:
                    field->default_value() = std::get<std::string>(curtok.value());
                    break;
                case TokenType::Identifier:
                    field->default_value() = std::get<std::string>(curtok.value());
                    break;
                case TokenType::Keyword:
                    if (is_builtin_type(lexKeywordMapReverse.at(std::get<Keyword>(curtok.value()))))
                    {
                        field->default_value() = lexKeywordMapReverse.at(std::get<Keyword>(curtok.value()));
                    }
                    else
                    {
                        throw SyntaxError("Expected default value in struct field");
                        return false;
                    }
                    break;

                default:
                    break;
                }
                tokens.pop();
            }

            fields.push_back(field);
            field = std::make_shared<StructField>();
        }
    }

    node = std::make_shared<StructDefinition>(std::get<std::string>(next_1.value()), fields, functions, packed);

    return true;
}

static bool jcc::parse_class_keyword(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node)
{
    (void)tokens;
    (void)node;
    /// TODO: implement this
    return false;
}

static bool jcc::parse_enum_keyword(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node)
{
    (void)tokens;
    (void)node;
    return false;
}

static bool jcc::parse_typedef_keyword(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node)
{
    (void)tokens;
    (void)node;
    return false;
}

static bool jcc::parse_subsystem_keyword(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node)
{
    if (tokens.size() < 3)
    {
        throw SyntaxError("Expected identifier after subsystem keyword");
        return false;
    }

    Token next_1 = tokens.peek(1);
    Token next_2 = tokens.peek(2);

    if (next_1.type() != TokenType::Identifier)
    {
        throw SyntaxError("Expected identifier after subsystem keyword");
        return false;
    }

    if (next_2.type() != TokenType::Punctuator)
    {
        node = std::make_shared<SubsystemDeclaration>(std::get<std::string>(next_1.value()));
        tokens.pop(2);
        return true;
    }

    if (std::get<Punctuator>(next_2.value()) == Punctuator::OpenBrace)
    {
        tokens.pop(2);
        std::shared_ptr<GenericNode> block = std::make_shared<Block>();

        if (!parse_block(tokens, block, false))
        {
            return false;
        }

        node = std::make_shared<SubsystemDefinition>(std::get<std::string>(next_1.value()), std::static_pointer_cast<Block>(block));
        return true;
    }
    else if (std::get<Punctuator>(next_2.value()) != Punctuator::Colon)
    {
        throw SyntaxError("Expected punctuator after subsystem identifier");
        return false;
    }

    tokens.pop(3);

    std::vector<std::string> dependencies;

    // name1, name2, name3, name4...
    while (1)
    {
        Token curtok = tokens.peek();

        if (curtok.type() != TokenType::Identifier)
        {
            throw SyntaxError("Expected identifier in subsystem dependencies");
            return false;
        }

        dependencies.push_back(std::get<std::string>(curtok.value()));

        tokens.pop();

        if (tokens.eof())
        {
            throw SyntaxError("Expected seperator in subsystem dependencies");
            return false;
        }

        curtok = tokens.peek();

        if (curtok.type() == TokenType::Punctuator && std::get<Punctuator>(curtok.value()) == Punctuator::Comma)
        {
            tokens.pop();
            continue;
        }
        else if (curtok.type() == TokenType::Punctuator && std::get<Punctuator>(curtok.value()) == Punctuator::OpenBrace)
        {
            break;
        }
        else
        {
            // done with declaration
            node = std::make_shared<SubsystemDeclaration>(std::get<std::string>(next_1.value()), dependencies);
            return true;
        }
    }

    std::shared_ptr<GenericNode> block = std::make_shared<Block>();
    if (!parse_block(tokens, block, false))
    {
        return false;
    }

    node = std::make_shared<SubsystemDefinition>(std::get<std::string>(next_1.value()), std::static_pointer_cast<Block>(block), dependencies);
    return true;
}

static bool jcc::parse_function_parameters(jcc::TokenList &tokens, std::vector<std::shared_ptr<jcc::FunctionParameter>> &params)
{
    if (tokens.size() < 2)
    {
        throw SyntaxError("Expected closing parenthesis in function parameters");
        return false;
    }

    Token curtok = tokens.peek();

    if (curtok.type() != TokenType::Punctuator || std::get<Punctuator>(curtok.value()) != Punctuator::OpenParen)
    {
        throw SyntaxError("Expected opening parenthesis in function parameters");
        return false;
    }
    tokens.pop();
    curtok = tokens.peek();

    bool is_looping = true;
    while (is_looping)
    {
        Token curtok = tokens.peek();

        if (curtok.type() == TokenType::Punctuator && std::get<Punctuator>(curtok.value()) == Punctuator::CloseParen)
        {
            tokens.pop();
            is_looping = false;
            break;
        }
        if (curtok.type() != TokenType::Identifier)
        {
            throw SyntaxError("Expected identifier in function parameter");
            return false;
        }

        std::shared_ptr<FunctionParameter> parameter = std::make_shared<FunctionParameter>();
        parameter->name() = std::get<std::string>(curtok.value());

        tokens.pop();
        if (tokens.eof())
        {
            throw SyntaxError("Expected seperator in function parameter");
            return false;
        }
        curtok = tokens.peek();
        if (curtok.type() != TokenType::Punctuator || std::get<Punctuator>(curtok.value()) != Punctuator::Colon)
        {
            throw SyntaxError("Expected seperator in function parameter");
            return false;
        }
        tokens.pop();

        int state = 2;

        while (state > 0)
        {
            if (tokens.eof())
            {
                throw SyntaxError("Expected seperator in function parameter");
                return false;
            }
            curtok = tokens.peek();
            if (curtok.type() != TokenType::Identifier && curtok.type() != TokenType::Keyword)
            {
                throw SyntaxError("Expected type in function parameter type");
                return false;
            }

            switch (curtok.type())
            {
            case TokenType::Identifier:
                parameter->type() = std::get<std::string>(curtok.value());
                tokens.pop();
                state = 0;
                break;
            case TokenType::Keyword:
                // check if
                {
                    if (state == 2)
                    {
                        if (std::get<Keyword>(curtok.value()) == Keyword::Const)
                        {
                            parameter->is_const() = true;
                            tokens.pop();
                            state = 1;
                            continue;
                        }

                        if (std::get<Keyword>(curtok.value()) == Keyword::Ref)
                        {
                            parameter->is_reference() = true;
                            parameter->is_const() = false;
                            tokens.pop();
                            state = 1;
                            continue;
                        }
                    }

                    std::string tmp = lexKeywordMapReverse.at(std::get<Keyword>(curtok.value()));
                    if (is_builtin_type(tmp))
                    {
                        if (tmp == "null")
                        {
                            throw SyntaxError("Parameter type cannot be null");
                        }
                        else if (tmp == "void")
                        {
                            throw SyntaxError("Parameter type cannot be void");
                        }
                        parameter->type() = tmp;
                        tokens.pop();
                        state = 0;
                        continue;
                    }
                    else
                    {
                        throw SyntaxError("Expected type annotation in function parameter");
                        return false;
                    }
                }
                break;
            default:
                panic("Unknown token type: " + std::to_string((int)curtok.type()));
                break;
            }
        }
        if (tokens.eof())
        {
            throw SyntaxError("Expected seperator in function declaration");
            return false;
        }
        curtok = tokens.peek();

        // check for array
        if (curtok.type() == TokenType::Punctuator && std::get<Punctuator>(curtok.value()) == Punctuator::OpenBracket)
        {
            tokens.pop();
            if (tokens.eof())
            {
                throw SyntaxError("Expected closing bracket in function parameter");
                return false;
            }
            curtok = tokens.peek();
            if (curtok.type() == TokenType::NumberLiteral)
            {
                parameter->arr_size() = std::stoi(std::get<std::string>(curtok.value()));
                tokens.pop();
                if (tokens.eof())
                {
                    throw SyntaxError("Expected closing bracket in function parameter");
                    return false;
                }
                curtok = tokens.peek();
            }
            else
            {
                parameter->arr_size() = std::numeric_limits<uint64_t>::max();
            }

            if (curtok.type() == TokenType::Punctuator && std::get<Punctuator>(curtok.value()) == Punctuator::CloseBracket)
            {
                tokens.pop();
            }
            else
            {
                throw SyntaxError("Expected closing bracket in function parameter");
                return false;
            }
            if (tokens.eof())
            {
                throw SyntaxError("Expected closing bracket in function parameter");
                return false;
            }
            curtok = tokens.peek();
        }

        if (curtok.type() == TokenType::Operator && std::get<Operator>(curtok.value()) == Operator::Assign)
        {
            tokens.pop();
            if (tokens.eof())
            {
                throw SyntaxError("Expected default value in function parameter");
                return false;
            }
            curtok = tokens.peek();
            if (curtok.type() != TokenType::StringLiteral && curtok.type() != TokenType::NumberLiteral && curtok.type() != TokenType::FloatingPointLiteral && curtok.type() != TokenType::Identifier && curtok.type() != TokenType::Keyword && curtok.type() != TokenType::Punctuator)
            {
                throw SyntaxError("Expected default value in function parameter");
                return false;
            }

            switch (curtok.type())
            {
            case TokenType::StringLiteral:
                parameter->default_value() = std::make_shared<StringLiteralExpression>(std::get<std::string>(curtok.value()));
                break;
            case TokenType::NumberLiteral:
                parameter->default_value() = std::make_shared<IntegerLiteralExpression>(std::get<std::string>(curtok.value()));
                break;
            case TokenType::FloatingPointLiteral:
                parameter->default_value() = std::make_shared<FloatingPointLiteralExpression>(std::get<std::string>(curtok.value()));
                break;
            case TokenType::Identifier:
            {
                std::shared_ptr<GenericNode> expr;
                if (!parse_expression(tokens, expr))
                {
                    return false;
                }
                parameter->default_value() = std::static_pointer_cast<Expression>(expr);
                break;
            }
            case TokenType::Keyword:
                if (std::get<Keyword>(curtok.value()) == Keyword::Null)
                {
                    parameter->default_value() = std::make_shared<NullExpression>();
                }
                else
                {
                    throw SyntaxError("Expected default value in function parameter");
                    return false;
                }
                break;
            default:
            {
                std::shared_ptr<GenericNode> expr;
                if (!parse_expression(tokens, expr))
                {
                    return false;
                }
                parameter->default_value() = std::static_pointer_cast<Expression>(expr);
            }
            }

            tokens.pop();
        }

        params.push_back(parameter);

        curtok = tokens.peek();
        if (curtok.type() == TokenType::Punctuator && std::get<Punctuator>(curtok.value()) == Punctuator::Comma)
        {
            tokens.pop();
        }
    }

    return true;
}

static bool jcc::parse_func_keyword(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node, jcc::FunctionParseMode mode)
{
    // func name ([[name:type[=default]]...]) [-> return_type] [block]

    if (tokens.size() < 4)
    {
        throw SyntaxError("Expected identifier after func keyword");
        return false;
    }

    Token next_1 = tokens.peek(1);
    Token next_2 = tokens.peek(2);

    if (next_1.type() != TokenType::Identifier)
    {
        throw SyntaxError("Expected identifier after func keyword");
        return false;
    }

    // parse parameters
    std::vector<std::shared_ptr<FunctionParameter>> parameters;

    tokens.pop(2);

    if (!parse_function_parameters(tokens, parameters))
    {
        return false;
    }

    if (tokens.eof())
    {
        throw SyntaxError("Expected seperator in function declaration");
        return false;
    }

    Token curtok = tokens.peek();
    std::string return_type;
    uint64_t return_arr_size = 0;

    if (curtok.type() == TokenType::Punctuator && std::get<Punctuator>(curtok.value()) == Punctuator::Colon)
    {
        tokens.pop();
        if (tokens.eof())
        {
            throw SyntaxError("Expected colon in function declaration");
            return false;
        }
        curtok = tokens.peek();

        // parse return type
        if (tokens.eof())
        {
            throw SyntaxError("Expected return type in function declaration");
            return false;
        }
        curtok = tokens.peek();

        if (curtok.type() != TokenType::Identifier && curtok.type() != TokenType::Keyword)
        {
            throw SyntaxError("Expected return type in function declaration");
            return false;
        }

        if (curtok.type() == TokenType::Identifier)
        {
            return_type = std::get<std::string>(curtok.value());
        }
        else if (curtok.type() == TokenType::Keyword)
        {
            if (is_builtin_type(lexKeywordMapReverse.at(std::get<Keyword>(curtok.value()))))
            {
                return_type = lexKeywordMapReverse.at(std::get<Keyword>(curtok.value()));
                if (return_type == "null")
                {
                    return_type = "";
                }
            }
            else
            {
                throw SyntaxError("Expected return type in function declaration");
                return false;
            }
        }
        tokens.pop();

        // check for array
        if (tokens.eof())
        {
            throw SyntaxError("Expected token in function parameter");
            return false;
        }
        curtok = tokens.peek();
        if (curtok.type() == TokenType::Punctuator && std::get<Punctuator>(curtok.value()) == Punctuator::OpenBracket)
        {
            tokens.pop();
            if (tokens.eof())
            {
                throw SyntaxError("Expected closing bracket in function parameter");
                return false;
            }
            curtok = tokens.peek();
            if (curtok.type() == TokenType::NumberLiteral)
            {
                return_arr_size = std::stoi(std::get<std::string>(curtok.value()));
                tokens.pop();
                if (tokens.eof())
                {
                    throw SyntaxError("Expected closing bracket in function parameter");
                    return false;
                }
                curtok = tokens.peek();
            }
            else
            {
                return_arr_size = std::numeric_limits<uint64_t>::max();
            }

            if (curtok.type() == TokenType::Punctuator && std::get<Punctuator>(curtok.value()) == Punctuator::CloseBracket)
            {
                tokens.pop();
            }
            else
            {
                throw SyntaxError("Expected closing bracket in function parameter");
                return false;
            }
            if (tokens.eof())
            {
                throw SyntaxError("Expected closing bracket in function parameter");
                return false;
            }
            curtok = tokens.peek();
        }
    }
    else if (curtok.type() != TokenType::Punctuator || std::get<Punctuator>(curtok.value()) != Punctuator::OpenBrace)
    {
        return_type = "void"; // implicit void return type
        std::shared_ptr<FunctionDeclaration> func_ptr = std::make_shared<FunctionDeclaration>(std::get<std::string>(next_1.value()), return_type, parameters, return_arr_size);
        node = func_ptr;

        return true;
    }
    else
    {
        return_type = "void"; // implicit void return type
    }

    if (tokens.eof())
    {
        throw SyntaxError("Expected opening brace in function declaration");
        return false;
    }

    curtok = tokens.peek();

    if (curtok.type() != TokenType::Punctuator || std::get<Punctuator>(curtok.value()) != Punctuator::OpenBrace)
    {
        if (mode != FunctionParseMode::DeclarationOnly && mode != FunctionParseMode::DeclarationOrDefinition)
        {
            throw SyntaxError("Function definition expected but only declaration found");
            return false;
        }

        node = std::make_shared<FunctionDeclaration>(std::get<std::string>(next_1.value()), return_type, parameters, return_arr_size);
    }
    else
    {
        if (mode != FunctionParseMode::DefinitionOnly && mode != FunctionParseMode::DeclarationOrDefinition)
        {
            throw SyntaxError("Function declaration expected but definition found");
            return false;
        }
        std::shared_ptr<GenericNode> block = std::make_shared<Block>();

        if (!parse_block(tokens, block, true))
        {
            return false;
        }

        node = std::make_shared<FunctionDefinition>(std::get<std::string>(next_1.value()), return_type, parameters, std::static_pointer_cast<Block>(block), return_arr_size);
    }

    return true;
}

static bool jcc::parse_return_keyword(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node)
{
    if (tokens.size() < 1)
    {
        throw SyntaxError("Expected return value after return keyword");
        return false;
    }

    tokens.pop();

    if (tokens.eof())
    {
        node = std::make_shared<ReturnStatement>(nullptr);
        return true;
    }

    std::shared_ptr<GenericNode> expr;
    if (!parse_expression(tokens, expr))
    {
        node = std::make_shared<ReturnStatement>(nullptr);
    }
    else
    {
        node = std::make_shared<ReturnStatement>(std::static_pointer_cast<Expression>(expr));
    }

    return true;
}

std::map<std::string, uint32_t> operator_precedence = {
    {"++", 13},
    {"--", 13},
    {"+=", 13},
    {"-=", 13},
    {"*=", 13},
    {"/=", 13},
    {"%=", 13},
    {"^^=", 13},
    {"||=", 13},
    {"&&=", 13},
    {"<<=", 13},
    {">>=", 13},
    {">>>=", 13},
    {"|=", 13},
    {"&=", 13},
    {"^=", 13},
    {"<<", 12},
    {">>", 12},
    {"==", 11},
    {"!=", 11},
    {"&&", 10},
    {"||", 9},
    {"^^", 8},
    {"<=", 7},
    {">=", 7},
    {"<", 7},
    {">", 7},
    {"=", 6},
    {"??", 5},
    {"@", 5},
    {"//", 4},
    {"+", 3},
    {"-", 3},
    {"*", 3},
    {"/", 3},
    {"%", 3},
    {"&", 2},
    {"|", 2},
    {"^", 2},
    {"~", 2},
    {"!", 2},
    {"?", 1},
    {"#", 1},
    {".", 1},
    {",", 1},
    // Add other custom operators here with their respective precedence
};

static bool jcc::parse_expression_helper(jcc::TokenList &tokens, jcc::ExpNode &output)
{
    /// TODO: implement this
    (void)tokens;
    output = ExpNode(Token(TokenType::NumberLiteral, ""), {});

    tokens.pop();

    return true;
}

static bool jcc::parse_expression(jcc::TokenList &tokens, std::shared_ptr<jcc::GenericNode> &node)
{
    ExpNode output;
    parse_expression_helper(tokens, output);

    auto item = std::make_shared<LiteralExpression>(std::get<std::string>(output.value.value()));

    node = std::static_pointer_cast<GenericNode>(item);

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

    // reuse parse block code by
    // wrapping source in a block
    // and parsing it
    // tokens array is reversed
    tokens_copy.m_tokens.insert(tokens_copy.m_tokens.begin(), Token(TokenType::Punctuator, Punctuator::CloseBrace));
    tokens_copy.m_tokens.push_back(Token(TokenType::Punctuator, Punctuator::OpenBrace));

    if (!parse_block(tokens_copy, rootnode, false))
    {
        return nullptr;
    }

    if (rootnode->type() != NodeType::Block)
    {
        throw ParserException("Expected block as root node");
        return nullptr;
    }

    std::static_pointer_cast<Block>(rootnode)->render_braces() = false;

    if (rootnode == nullptr)
    {
        return nullptr;
    }

    std::shared_ptr<AbstractSyntaxTree> nodes = std::make_shared<AbstractSyntaxTree>();

    nodes->root() = rootnode;

    return nodes;
}