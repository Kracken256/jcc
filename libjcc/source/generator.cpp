#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include "compile.hpp"

#define INDENT_SIZE 4

using namespace jcc;

static std::string mkpadding(uint32_t indent)
{
    std::string result;

    for (uint32_t i = 0; i < indent; i++)
    {
        result += " ";
    }

    return result;
}

static std::string generate_node_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent);

static std::string generate_block_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent)
{

    std::string result = mkpadding(indent) + "{\n";

    indent += INDENT_SIZE;

    auto block = std::static_pointer_cast<Block>(node);

    for (const auto &child : block->children())
    {
        result += generate_node_cxx(child, indent);
    }

    indent -= INDENT_SIZE;

    result += mkpadding(indent) + "}\n";

    return result;
}

static std::string generate_typedef_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent)
{
    auto typedefdef = std::static_pointer_cast<TypeDeclaration>(node);
    std::string result = mkpadding(indent) + "typedef " + typedefdef->type_name() + " " + typedefdef->alias() + ";\n";

    return result;
}

static std::string generate_struct_declaration_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent)
{
    auto structdef = std::static_pointer_cast<StructDeclaration>(node);
    std::string result = mkpadding(indent) + "struct " + structdef->name() + ";\n";

    return result;
}

static std::string generate_union_declaration_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent)
{
    auto uniondef = std::static_pointer_cast<UnionDeclaration>(node);
    std::string result = mkpadding(indent) + "union " + uniondef->name() + ";\n";

    return result;
}

static std::string generate_enum_declaration_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent)
{
    auto enumdef = std::static_pointer_cast<EnumDeclaration>(node);
    std::string result = mkpadding(indent) + "enum " + enumdef->name() + ";\n";

    return result;
}

static std::string generate_function_parameter_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent)
{
    auto param = std::static_pointer_cast<FunctionParameter>(node);
    std::string result = param->type() + " " + param->name();

    (void)indent;

    if (!param->default_value().empty())
    {
        result += " = " + param->default_value();
    }

    return result;
}

static std::string generate_function_declaration_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent)
{
    auto func = std::static_pointer_cast<FunctionDeclaration>(node);
    std::string result = mkpadding(indent) + func->return_type() + " " + func->name() + "(";

    for (const auto &param : func->parameters())
    {
        result += generate_function_parameter_cxx(param, indent);
    }

    result += ");\n";

    return result;
}

static std::string generate_class_declaration_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent)
{
    auto classdef = std::static_pointer_cast<ClassDeclaration>(node);
    std::string result = mkpadding(indent) + "class " + classdef->name() + ";\n";

    return result;
}

static std::string generate_extern_declaration_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent)
{
    auto externdef = std::static_pointer_cast<ExternalDeclaration>(node);

    (void)indent;

    /// TODO: Implement this function.

    throw std::runtime_error("Not implemented");
    return "";
}

static std::string generate_namespace_declaration_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent)
{
    auto namespacedef = std::static_pointer_cast<NamespaceDeclaration>(node);
    std::string result = mkpadding(indent) + "namespace " + namespacedef->name() + ";\n";

    return result;
}

static std::string generate_namespace_definition_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent)
{
    auto namespacedef = std::static_pointer_cast<NamespaceDefinition>(node);
    std::string result = mkpadding(indent) + "namespace " + namespacedef->name() + "\n";

    result += generate_block_cxx(namespacedef->block(), indent) + "\n";

    return result;
}

static std::string generate_struct_definition_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent)
{
    auto structdef = std::static_pointer_cast<StructDefinition>(node);
    std::string result = mkpadding(indent) + "struct " + structdef->name() + "\n"+ mkpadding(indent) + "{\n";

    indent += INDENT_SIZE;

    for (const auto &field : structdef->fields())
    {
        result += mkpadding(indent) + field->type() + " " + field->name();

        if (field->bitfield() > 0)
        {
            result += " : " + std::to_string(field->bitfield());
        }

        if (!field->default_value().empty())
        {
            result += " = " + field->default_value();
        }

        result += ";\n";
    }

    indent -= INDENT_SIZE;

    result += mkpadding(indent) + "};\n\n";

    return result;
}

static std::string generate_node_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent)
{
    if (node == nullptr)
    {
        throw std::runtime_error("Node is null");
    }

    switch (node->type())
    {
    case NodeType::Block:
        return generate_block_cxx(node, indent);
    case NodeType::TypeDeclaration:
        return generate_typedef_cxx(node, indent);
    case NodeType::StructDeclaration:
        return generate_struct_declaration_cxx(node, indent);
    case NodeType::UnionDeclaration:
        return generate_union_declaration_cxx(node, indent);
    case NodeType::EnumDeclaration:
        return generate_enum_declaration_cxx(node, indent);
    case NodeType::FunctionDeclaration:
        return generate_function_declaration_cxx(node, indent);
    case NodeType::ClassDeclaration:
        return generate_class_declaration_cxx(node, indent);
    case NodeType::ExternalDeclaration:
        return generate_extern_declaration_cxx(node, indent);
    case NodeType::NamespaceDeclaration:
        return generate_namespace_declaration_cxx(node, indent);

    case NodeType::NamespaceDefinition:
        return generate_namespace_definition_cxx(node, indent);
    case NodeType::StructDefinition:
        return generate_struct_definition_cxx(node, indent);

    default:
        return "/* NA */";
    }
}

std::string jcc::CompilationUnit::generate(const std::shared_ptr<jcc::AbstractSyntaxTree> &ast, TargetLanguage target)
{
    std::string result;
    uint32_t indent = 0;

    if (target != TargetLanguage::CXX)
    {
        panic("Unsupported target language", m_messages);
    }

    result += generate_node_cxx(ast->root(), indent);

    return result;
}