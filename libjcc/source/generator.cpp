#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include "compile.hpp"

using namespace jcc;

static std::string generate_node_cxx(const std::shared_ptr<jcc::GenericNode> &node);

static std::string generate_block_cxx(const std::shared_ptr<jcc::GenericNode> &node)
{
    std::string result = "{\n";

    auto block = std::static_pointer_cast<Block>(node);

    for (const auto &child : block->children())
    {
        result += generate_node_cxx(child);
    }

    result += "}\n";

    return result;
}

static std::string generate_typedef_cxx(const std::shared_ptr<jcc::GenericNode> &node)
{
    auto typedefdef = std::static_pointer_cast<TypeDeclaration>(node);
    std::string result = "typedef " + typedefdef->type_name() + " " + typedefdef->alias() + ";\n";

    return result;
}

static std::string generate_struct_declaration_cxx(const std::shared_ptr<jcc::GenericNode> &node)
{
    auto structdef = std::static_pointer_cast<StructDeclaration>(node);
    std::string result = "struct " + structdef->name() + ";\n";

    return result;
}

static std::string generate_union_declaration_cxx(const std::shared_ptr<jcc::GenericNode> &node)
{
    auto uniondef = std::static_pointer_cast<UnionDeclaration>(node);
    std::string result = "union " + uniondef->name() + ";\n";

    return result;
}

static std::string generate_enum_declaration_cxx(const std::shared_ptr<jcc::GenericNode> &node)
{
    auto enumdef = std::static_pointer_cast<EnumDeclaration>(node);
    std::string result = "enum " + enumdef->name() + ";\n";

    return result;
}

static std::string generate_function_parameter_cxx(const std::shared_ptr<jcc::GenericNode> &node)
{
    auto param = std::static_pointer_cast<FunctionParameter>(node);
    std::string result = param->type() + " " + param->name();

    if (!param->default_value().empty())
    {
        result += " = " + param->default_value();
    }

    return result;
}

static std::string generate_function_declaration_cxx(const std::shared_ptr<jcc::GenericNode> &node)
{
    auto func = std::static_pointer_cast<FunctionDeclaration>(node);
    std::string result = func->return_type() + " " + func->name() + "(";

    for (const auto &param : func->parameters())
    {
        result += generate_function_parameter_cxx(param);
    }

    result += ");\n";

    return result;
}

static std::string generate_class_declaration_cxx(const std::shared_ptr<jcc::GenericNode> &node)
{
    auto classdef = std::static_pointer_cast<ClassDeclaration>(node);
    std::string result = "class " + classdef->name() + ";\n";

    return result;
}

static std::string generate_extern_declaration_cxx(const std::shared_ptr<jcc::GenericNode> &node)
{
    auto externdef = std::static_pointer_cast<ExternalDeclaration>(node);

    /// TODO: Implement this function.

    throw std::runtime_error("Not implemented");
    return "";
}

static std::string generate_namespace_declaration_cxx(const std::shared_ptr<jcc::GenericNode> &node)
{
    auto namespacedef = std::static_pointer_cast<NamespaceDeclaration>(node);
    std::string result = "namespace " + namespacedef->name() + ";\n";

    return result;
}

static std::string generate_namespace_definition_cxx(const std::shared_ptr<jcc::GenericNode> &node)
{
    auto namespacedef = std::static_pointer_cast<NamespaceDefinition>(node);
    std::string result = "namespace " + namespacedef->name() + "\n";

    result += generate_block_cxx(namespacedef->block());

    return result;
}

static std::string generate_struct_definition_cxx(const std::shared_ptr<jcc::GenericNode> &node)
{
    auto structdef = std::static_pointer_cast<StructDefinition>(node);
    std::string result = "struct " + structdef->name() + " {\n";

    for (const auto &field : structdef->fields())
    {
        result += "  " + field->type() + " " + field->name();

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

    return result + "};\n";
}

static std::string generate_node_cxx(const std::shared_ptr<jcc::GenericNode> &node)
{
    if (node == nullptr)
    {
        throw std::runtime_error("Node is null");
    }

    switch (node->type())
    {
    case NodeType::Block:
        return generate_block_cxx(node);
    case NodeType::TypeDeclaration:
        return generate_typedef_cxx(node);
    case NodeType::StructDeclaration:
        return generate_struct_declaration_cxx(node);
    case NodeType::UnionDeclaration:
        return generate_union_declaration_cxx(node);
    case NodeType::EnumDeclaration:
        return generate_enum_declaration_cxx(node);
    case NodeType::FunctionDeclaration:
        return generate_function_declaration_cxx(node);
    case NodeType::ClassDeclaration:
        return generate_class_declaration_cxx(node);
    case NodeType::ExternalDeclaration:
        return generate_extern_declaration_cxx(node);
    case NodeType::NamespaceDeclaration:
        return generate_namespace_declaration_cxx(node);

    case NodeType::NamespaceDefinition:
        return generate_namespace_definition_cxx(node);
    case NodeType::StructDefinition:
        return generate_struct_definition_cxx(node);

    default:
        return "/* NA */";
    }
}

std::string jcc::CompilationUnit::generate(const std::shared_ptr<jcc::AbstractSyntaxTree> &ast, TargetLanguage target)
{
    std::string result;

    if (target != TargetLanguage::CXX)
    {
        panic("Unsupported target language", m_messages);
    }

    result += generate_node_cxx(ast->root());

    /// TODO: Remove this
    result += "int main() { return 0; }\n";

    return result;
}