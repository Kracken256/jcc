#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <limits>
#include <map>
#include <cstdint>
#include <cstddef>
#include <mutex>

#define _JCC_BACKEND_
#include "sha256.hpp"
#include "compile.hpp"

#define INDENT_SIZE 4

using namespace jcc;

static std::map<size_t, std::string> g_typenames_mapping;
static std::mutex g_typenames_mapping_mutex;

uint32_t unix_timestamp();

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
    std::string result = mkpadding(indent);

    if (func->return_type().empty())
    {
        result += "[[noreturn]] void";
    }
    else
    {
        result += func->return_type();
    }

    result += " " + func->name() + "(";

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
    static size_t object_id = 0;
    static std::mutex object_id_mutex;

    std::string result;

    auto structdef = std::static_pointer_cast<StructDefinition>(node);

    result += mkpadding(indent) + "/* Begin Structure " + structdef->name() + " */\n";

    if (structdef->packed())
    {
        result += mkpadding(indent) + "#pragma pack(push, 1)\n";
    }

    object_id_mutex.lock();
    g_typenames_mapping_mutex.lock();
    result += mkpadding(indent) + "class " + structdef->name() + " : public _j::StructGeneric<" + std::to_string(object_id) + ">\n" + mkpadding(indent) + "{\n";

    g_typenames_mapping.insert({object_id, structdef->name()});
    object_id++;

    g_typenames_mapping_mutex.unlock();
    object_id_mutex.unlock();

    result += mkpadding(indent) + "public:\n";

    indent += INDENT_SIZE;

    result += mkpadding(indent) + structdef->name() + "()\n" + mkpadding(indent) + "{\n";
    indent += INDENT_SIZE;
    for (size_t i = 0; i < structdef->fields().size(); i++)
    {
        auto field = structdef->fields()[i];

        if (field->attributes().size() == 0)
        {
            continue;
        }

        result += mkpadding(indent) + "/* attributes for field " + field->name() + " */\n";

        for (const auto &attribute : field->attributes())
        {
            result += mkpadding(indent) + "this->_set(\"" + field->name() + "_" + attribute->name() + "\", " + attribute->value() + ");\n";
        }

        result += "\n";
    }

    result += mkpadding(indent) + "/* auto-generated attributes */\n";
    std::map<std::string, std::string> auto_attributes;

    // index_names attribute contains list of all fields
    // index_types attribute contains list of all fields types
    // index
    std::string index_names;
    std::string index_types;
    std::string index;
    for (size_t i = 0; i < structdef->fields().size(); i++)
    {
        auto field = structdef->fields()[i];

        std::string typetmp;

        index_names += field->name();
        if (field->arr_size() > 0)
        {
            typetmp = "vector<" + field->type() + ">";
        }
        else
        {
            typetmp += field->type();
            if (field->bitfield() > 0)
            {
                typetmp += ":" + std::to_string(field->bitfield());
            }
        }
        if (!field->default_value().empty())
        {
            typetmp += "=" + field->default_value();
        }

        index += field->name() + ":" + typetmp;
        index_types += typetmp;
        if (i != structdef->fields().size() - 1)
        {
            index_names += ",";
            index_types += ",";
            index += ",";
        }
    }

    auto_attributes.insert({"_index_names", "\"" + index_names + "\""});
    auto_attributes.insert({"_index_types", "\"" + index_types + "\""});
    auto_attributes.insert({"_index", "\"" + index + "\""});

    for (const auto &attr : auto_attributes)
    {
        result += mkpadding(indent) + "this->_set(\"" + attr.first + "\", " + attr.second + ");\n";
    }

    indent -= INDENT_SIZE;

    result += mkpadding(indent) + "}\n\n";

    for (const auto &field : structdef->fields())
    {
        if (field->arr_size() != std::numeric_limits<uint64_t>::max())
        {
            result += mkpadding(indent) + field->type() + " " + field->name();

            if (field->arr_size() > 0)
            {
                result += "[" + std::to_string(field->arr_size()) + "]";
            }

            if (field->bitfield() > 0)
            {
                result += " : " + std::to_string(field->bitfield());
            }

            if (!field->default_value().empty())
            {
                result += " = " + field->default_value();
            }
        }
        else
        {
            result += mkpadding(indent) + "vector<" + field->type() + "> " + field->name();

            if (!field->default_value().empty())
            {
                result += " = " + field->default_value();
            }
        }

        result += ";\n";
    }

    indent -= INDENT_SIZE;

    result += mkpadding(indent) + "};\n";

    if (structdef->packed())
    {
        result += mkpadding(indent) + "#pragma pack(pop)\n";
    }

    result += mkpadding(indent) + "constexpr auto j_" + structdef->name() + "_size = sizeof(" + structdef->name() + ");\n" + mkpadding(indent) + "/* End Structure " + structdef->name() + " */\n\n";

    return result;
}

static std::string generate_function_definition_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent)
{
    auto funcdef = std::static_pointer_cast<FunctionDefinition>(node);
    std::string result = mkpadding(indent);

    if (funcdef->return_type().empty())
    {
        result += "[[noreturn]] void";
    }
    else
    {
        result += funcdef->return_type();
    }

    result += " " + funcdef->name() + "(";

    for (size_t i = 0; i < funcdef->parameters().size(); i++)
    {
        result += generate_function_parameter_cxx(funcdef->parameters()[i], indent);

        if (i != funcdef->parameters().size() - 1)
        {
            result += ", ";
        }
    }

    result += ")\n";

    result += generate_block_cxx(funcdef->block(), indent);

    if (funcdef->return_type().empty())
    {
        indent += INDENT_SIZE;
        result = result.substr(0, result.size() - 2);
        result += "\n" + mkpadding(indent) + "/* make undefined behavior defined */\n" + mkpadding(indent) + "while (true) {}\n";
        indent -= INDENT_SIZE;
        result += mkpadding(indent) + "}\n\n";
    }
    else
    {
        result += "\n";
    }

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
    case NodeType::FunctionDefinition:
        return generate_function_definition_cxx(node, indent);

    case NodeType::ReturnStatement:
    {
        auto retstmt = std::static_pointer_cast<ReturnStatement>(node);
        std::string result = mkpadding(indent) + "return";

        if (retstmt->expression() != nullptr)
        {
            result += " " + generate_node_cxx(retstmt->expression(), indent);
        }

        result += ";\n";

        return result;
    }

    case NodeType::BinaryExpression:
        return generate_node_cxx(std::static_pointer_cast<BinaryExpression>(node)->left(), indent) + " " + std::static_pointer_cast<BinaryExpression>(node)->op() + " " + generate_node_cxx(std::static_pointer_cast<BinaryExpression>(node)->right(), indent);
    case NodeType::UnaryExpression:
        return std::static_pointer_cast<UnaryExpression>(node)->op() + generate_node_cxx(std::static_pointer_cast<UnaryExpression>(node)->expression(), indent);
    case NodeType::CastExpression:
        return "(" + std::static_pointer_cast<CastExpression>(node)->type() + ")" + generate_node_cxx(std::static_pointer_cast<CastExpression>(node)->expression(), indent);
    case NodeType::LiteralExpression:
        return std::static_pointer_cast<LiteralExpression>(node)->value();
    case NodeType::CallExpression:
    {
        auto callexpr = std::static_pointer_cast<CallExpression>(node);
        std::string result = callexpr->name() + "(";

        for (const auto &arg : callexpr->arguments())
        {
            result += generate_node_cxx(arg, indent);
        }

        result += ")";

        return result;
    }
    case NodeType::StringLiteralExpression:
        return std::static_pointer_cast<StringLiteralExpression>(node)->value();
    case NodeType::CharLiteralExpression:
        return std::static_pointer_cast<CharLiteralExpression>(node)->value();
    case NodeType::IntegerLiteralExpression:
        return std::static_pointer_cast<IntegerLiteralExpression>(node)->value();
    case NodeType::FloatingPointLiteralExpression:
        return std::static_pointer_cast<FloatingPointLiteralExpression>(node)->value();
    case NodeType::BooleanLiteralExpression:
        return std::static_pointer_cast<BooleanLiteralExpression>(node)->value();
    case NodeType::RawNode:
        return mkpadding(indent) + std::static_pointer_cast<RawNode>(node)->value() + "\n";

    default:
        throw std::runtime_error("Unknown node type");
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

const std::string typedef_commons = R"(#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <map>
#include <cstring>
#include <iostream>
#include <cstdlib>

namespace _jxx {
    typedef bool bit;
    typedef uint8_t byte;
    typedef uint16_t word;
    typedef uint32_t dword;
    typedef uint64_t qword;
    typedef int nint;
    typedef unsigned int nuint;
    typedef int intn;
    typedef unsigned int uintn;
    typedef uintptr_t address;
    typedef const char* string;
    typedef std::vector<uint8_t> buffer;
    typedef void* routine;
    #define null nullptr
    using std::vector;
};)";

const std::string structure_generic_baseclass = R"(    /* Begin Common Sink functions */

#ifdef __linux__
#include <execinfo.h>

    [[noreturn]] void _panic(_jxx::string message)
    {
        std::cerr << "PANIC: " << message << std::endl;

        void *array[10];
        size_t size;
        char **strings;

        size = backtrace(array, 10);
        strings = backtrace_symbols(array, size);

        std::cerr << "Begin Backtrace:" << std::endl;
        for (size_t i = 0; i < size; i++)
        {
            std::cerr << strings[i] << std::endl;
        }
        std::cerr << "End Backtrace" << std::endl;

        // Don't free strings, just leak it

        std::_Exit(1);
    }  
#else
    [[noreturn]] void _panic(_jxx::string message)
    {
        std::cerr << "PANIC: " << message << std::endl;
        abort();

        while (true) {}
    }  
#endif
    
    
    /* Begin Generic Structure Base Class */
    typedef _jxx::qword typeid_t;

    static std::map<typeid_t, _jxx::string> g_typenames_mapping = {!!!/* JCC_TYPENAMES_MAPPING */!!!};

    template <typeid_t T>
    class StructGeneric
    {
    protected:
        static const typeid_t m_typeid = T;
        static std::map<typeid_t, std::map<_jxx::string, const void *>> m_attributes;

    public:
        typeid_t _typeid()
        {
            return m_typeid;
        }

        _jxx::string _typename()
        {
            return g_typenames_mapping[m_typeid];
        }

        bool _has(_jxx::string name)
        {
            return m_attributes[m_typeid].find(name) != m_attributes[m_typeid].end();
        }

        bool _hasfield(_jxx::string name)
        {
            _jxx::string index_names = this->_get("_index_names");
            int state = 0;

            while (*index_names)
            {
                // ',' reset
                if (*index_names == ',')
                {
                    state = 0;
                    index_names++;
                    continue;
                }

                if (strcmp(index_names, name) == 0)
                {
                    return true;
                }

                index_names++;
            }

            return false;
        }

        void _set(_jxx::string name, _jxx::string value)
        {
            m_attributes[m_typeid][name] = value;
        }

        void _set(_jxx::string name, long value)
        {
            m_attributes[m_typeid][name] = (void *)value;
        }

        _jxx::string _get(_jxx::string name)
        {
            if (!this->_has(name))
                _panic("meta _get() failed: Attribute not found");
            return (_jxx::string)m_attributes[m_typeid][name];
        }

        long _getint(_jxx::string name)
        {
            if (!this->_has(name))
                _panic("meta _getint() failed: Attribute not found");
            return (_jxx::qword)m_attributes[m_typeid][name];
        }
    };

    template <typeid_t T>
    std::map<typeid_t, std::map<_jxx::string, const void *>> StructGeneric<T>::m_attributes;
    template <typeid_t T>
    const typeid_t StructGeneric<T>::m_typeid;
    /* End Generic Structure Base Class */)";

bool jcc::CompilationUnit::join_to_output_cxx(const std::vector<std::string> &cxx_files, const std::string &output_cxx)
{
    std::ofstream output_cxx_stream(output_cxx, std::ios::binary);

    if (!output_cxx_stream.is_open())
    {
        return false;
    }

    jcc::crypto::SHA256_CTX sha256_ctx;
    jcc::crypto::sha256_init(sha256_ctx);

    uint32_t _unix_timestamp = ::unix_timestamp();

    // print header
    output_cxx_stream << "//==================================================================//\n"
                      << "// Type: J++ Transpiled Code                                        //\n"
                      << "// Date: " << std::put_time(std::localtime((const time_t *)&_unix_timestamp), "%c %Z") << "                               //\n"
                      << "//==================================================================//\n"
                      << "\n";

    output_cxx_stream << typedef_commons << "\n\n";
    // replace '!!!/* JCC_TYPENAMES_MAPPING */!!!' with the actual mapping
    // only replace first occurrence
    std::string tmp = structure_generic_baseclass;
    std::string new_value;
    for (auto it = g_typenames_mapping.begin(); it != g_typenames_mapping.end(); ++it)
    {
        new_value += "{" + std::to_string(it->first) + ", \"" + it->second + "\"}";

        if (std::next(it) != g_typenames_mapping.end())
        {
            new_value += ", ";
        }
    }
    size_t pos = tmp.find("!!!/* JCC_TYPENAMES_MAPPING */!!!");
    if (pos != std::string::npos)
    {
        tmp.replace(pos, 33, new_value);
    }

    output_cxx_stream << "namespace _j {\n";
    output_cxx_stream << tmp << "\n";
    output_cxx_stream << "};\n\n";

    for (const auto &file : cxx_files)
    {
        std::ifstream input_cxx_stream(file, std::ios::binary);

        if (!input_cxx_stream.is_open())
        {
            output_cxx_stream.close();
            return false;
        }

        std::string fname_padded = file;
        if (fname_padded.size() > 58)
        {
            fname_padded = "..." + fname_padded.substr(fname_padded.size() - 55);
        }
        else
        {
            fname_padded.resize(58, ' ');
        }

        output_cxx_stream << "//==================================================================//\n"
                          << "// File: " << fname_padded << " //\n"
                          << "//==================================================================//\n"
                          << "\n";

        std::stringstream input_cxx_stream_copy;
        input_cxx_stream_copy << input_cxx_stream.rdbuf();
        std::string file_contents = input_cxx_stream_copy.str();

        // update hash
        jcc::crypto::sha256_update(sha256_ctx, (uint8_t *)file_contents.c_str(), file_contents.size());

        // write to output
        output_cxx_stream << file_contents;

        output_cxx_stream << "\n//==================================================================//\n"
                          << "// EOF: " << fname_padded << "  //\n"
                          << "//==================================================================//\n\n";

        input_cxx_stream.close();
    }

    // append _start() symbol
    output_cxx_stream << "\nint main(int argc, char **argv)\n{\n    return _jxx::_main();\n}\n";

    // hex encode hash
    uint8_t hash_bytes[32];
    jcc::crypto::sha256_final(sha256_ctx, hash_bytes);
    std::string encoded;
    for (int i = 0; i < 32; i++)
    {
        encoded += "0123456789abcdef"[hash_bytes[i] >> 4];
        encoded += "0123456789abcdef"[hash_bytes[i] & 0xF];
    }

    // print footer
    output_cxx_stream << "\n//==================================================================//\n"
                      << "// EOF: J++ Transpiled Code                                         //\n"
                      << "// SHA256:                                                          //\n"
                      << "// " << encoded << " //\n"
                      << "//==================================================================//\n";

    output_cxx_stream.close();

    return true;
}