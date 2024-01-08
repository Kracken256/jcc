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
static bool g_has_main = false;
static std::mutex g_has_main_mutex;

struct ReflectiveEntry
{
    std::string field_name;
    std::string type;
    uint count = 1;
};

static std::map<size_t, std::vector<ReflectiveEntry>> g_reflective_entries;
static std::mutex g_reflective_entries_mutex;

uint32_t unix_timestamp();

static std::string rectify_name(const std::string &name)
{
    return "_" + name;
}

static std::string rectify_type(const std::string &type)
{
    std::string tmp = "_" + type;
    std::string string = "_";

    for (size_t i = 1; i < tmp.size(); i++)
    {
        if (tmp[i] == ':')
        {
            if (tmp.size() > i + 1 && tmp[i + 1] == ':')
            {
                string += "::_";
                i += 1;
                continue;
            }
        }

        string += tmp[i];
    }

    return string;
}

static std::string get_qualified_typename(const std::string &name, const std::string &_namespace)
{
    return rectify_name(_namespace) + "::" + rectify_name(name);
}

static std::string mkpadding(uint32_t indent)
{
    std::string result;

    for (uint32_t i = 0; i < indent; i++)
    {
        result += " ";
    }

    return result;
}

static std::string generate_node_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent, std::string &_namespace);

static std::string generate_block_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent, std::string &_namespace)
{
    std::string result = mkpadding(indent) + "{\n";

    indent += INDENT_SIZE;

    auto block = std::static_pointer_cast<Block>(node);

    for (const auto &child : block->children())
    {
        result += generate_node_cxx(child, indent, _namespace);
    }

    indent -= INDENT_SIZE;

    result += mkpadding(indent) + "}\n";

    return result;
}

static std::string generate_typedef_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent, std::string &_namespace)
{
    (void)_namespace;

    auto typedefdef = std::static_pointer_cast<TypeDeclaration>(node);
    std::string result = mkpadding(indent) + "typedef " + rectify_type(typedefdef->type_name()) + " " + rectify_type(typedefdef->alias()) + ";\n";

    return result;
}

static std::string generate_struct_declaration_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent, std::string &_namespace)
{
    (void)_namespace;

    auto structdef = std::static_pointer_cast<StructDeclaration>(node);
    std::string result = mkpadding(indent) + "struct " + rectify_name(structdef->name()) + ";\n";

    return result;
}

static std::string generate_union_declaration_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent, std::string &_namespace)
{
    (void)_namespace;

    auto uniondef = std::static_pointer_cast<UnionDeclaration>(node);
    std::string result = mkpadding(indent) + "union " + rectify_name(uniondef->name()) + ";\n";

    return result;
}

static std::string generate_enum_declaration_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent, std::string &_namespace)
{
    (void)_namespace;

    auto enumdef = std::static_pointer_cast<EnumDeclaration>(node);
    std::string result = mkpadding(indent) + "enum " + rectify_name(enumdef->name()) + ";\n";

    return result;
}

static std::string generate_function_parameter_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent, std::string &_namespace)
{
    auto param = std::static_pointer_cast<FunctionParameter>(node);
    std::string result;

    if (param->arr_size() == std::numeric_limits<uint64_t>::max())
    {
        if (!param->is_reference() || param->is_const())
        {
            result += "const ";
        }

        result += "vector<" + rectify_type(param->type()) + ">";
        if (param->is_reference() || (!param->is_reference() || param->is_const()))
        {
            result += "&";
        }
        result += " " + rectify_name(param->name());
    }
    else if (param->arr_size() > 0)
    {
        if (param->is_const())
        {
            result += "const ";
        }
        result += rectify_type(param->type()) + " " + rectify_name(param->name()) + "[" + std::to_string(param->arr_size()) + "]";
    }
    else
    {
        if (!param->is_reference() || param->is_const())
        {
            result += "const ";
        }

        result += rectify_type(param->type());
        if (param->is_reference() || (!param->is_reference() || param->is_const()))
        {
            result += "&";
        }
        result += " " + rectify_name(param->name());
    }

    if (param->default_value())
    {
        result += " = " + generate_node_cxx(param->default_value(), indent, _namespace);
    }

    return result;
}

static std::string generate_function_declaration_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent, std::string &_namespace)
{
    auto func = std::static_pointer_cast<FunctionDeclaration>(node);
    std::string result = mkpadding(indent);

    if (func->return_type().empty())
    {
        result += "[[noreturn]] _void";
    }
    else
    {
        if (func->return_arr_size() == std::numeric_limits<uint64_t>::max())
        {
            result += "vector<" + rectify_type(func->return_type()) + ">";
        }
        else if (func->return_arr_size() > 0)
        {
            result += "std::array<" + rectify_type(func->return_type()) + ", " + std::to_string(func->return_arr_size()) + "> ";
        }
        else
        {
            result += rectify_type(func->return_type());
        }
    }

    result += " " + rectify_name(func->name()) + "(";

    for (const auto &param : func->parameters())
    {
        result += generate_function_parameter_cxx(param, indent, _namespace);

        if (param != func->parameters().back())
        {
            result += ", ";
        }
    }

    result += ");\n";

    return result;
}

static std::string generate_class_declaration_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent, std::string &_namespace)
{
    (void)_namespace;

    auto classdef = std::static_pointer_cast<ClassDeclaration>(node);
    std::string result = mkpadding(indent) + "class " + rectify_name(classdef->name()) + ";\n";

    return result;
}

static std::string generate_extern_declaration_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent, std::string &_namespace)
{
    (void)_namespace;

    auto externdef = std::static_pointer_cast<ExternalDeclaration>(node);

    (void)indent;

    /// TODO: Implement this function.

    throw std::runtime_error("Not implemented");
    return "";
}

static std::string generate_namespace_declaration_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent, std::string &_namespace)
{
    (void)_namespace;

    auto namespacedef = std::static_pointer_cast<NamespaceDeclaration>(node);
    std::string result = mkpadding(indent) + "namespace " + rectify_name(namespacedef->name()) + ";\n";

    return result;
}

static std::string generate_namespace_definition_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent, std::string &_namespace)
{
    auto namespacedef = std::static_pointer_cast<NamespaceDefinition>(node);
    std::string ns_name = rectify_name(namespacedef->name());
    std::string result = mkpadding(indent) + "namespace " + ns_name + "\n";

    _namespace += ns_name + "::";

    result += generate_block_cxx(namespacedef->block(), indent, _namespace) + "\n";

    _namespace = _namespace.substr(0, _namespace.size() - ns_name.size() - 2);

    return result;
}

static std::string generate_struct_definition_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent, std::string &_namespace)
{
    static size_t object_id = 0;
    static std::mutex object_id_mutex;
    std::string result;

    auto structdef = std::static_pointer_cast<StructDefinition>(node);
    std::string struct_name = rectify_name(structdef->name());

    result += mkpadding(indent) + "/* Begin Structure " + struct_name + " */\n";

    if (structdef->packed())
    {
        result += mkpadding(indent) + "#pragma pack(push, 1)\n";
    }

    object_id_mutex.lock();
    g_typenames_mapping_mutex.lock();
    g_reflective_entries_mutex.lock();
    result += mkpadding(indent) + "class " + struct_name + " : public _j::StructGeneric<" + std::to_string(object_id) + ">\n" + mkpadding(indent) + "{\n";

    g_typenames_mapping.insert({object_id, get_qualified_typename(structdef->name(), _namespace)});
    ReflectiveEntry reflective_entry;
    reflective_entry.type = get_qualified_typename(structdef->name(), _namespace);
    for (const auto &field : structdef->fields())
    {
        reflective_entry.field_name = rectify_name(field->name());
        reflective_entry.type = rectify_type(field->type());
        if (field->arr_size() > 0)
        {
            reflective_entry.count = field->arr_size();
        }
        else
        {
            reflective_entry.count = 1;
        }
        g_reflective_entries[object_id].push_back(reflective_entry);
    }

    object_id++;

    g_reflective_entries_mutex.unlock();
    g_typenames_mapping_mutex.unlock();
    object_id_mutex.unlock();

    result += mkpadding(indent) + "public:\n";

    indent += INDENT_SIZE;

    result += mkpadding(indent) + struct_name + "()\n" + mkpadding(indent) + "{\n";
    indent += INDENT_SIZE;
    for (size_t i = 0; i < structdef->fields().size(); i++)
    {
        auto field = structdef->fields()[i];

        if (field->attributes().size() == 0)
        {
            continue;
        }

        result += mkpadding(indent) + "/* attributes for field " + rectify_name(field->name()) + " */\n";

        for (const auto &attribute : field->attributes())
        {
            result += mkpadding(indent) + "this->_set(\"" + rectify_name(field->name()) + "_" + attribute->name() + "\", " + attribute->value() + ");\n";
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

        index_names += rectify_name(field->name());
        if (field->arr_size() > 0)
        {
            typetmp = "vector<" + rectify_type(field->type()) + ">";
        }
        else
        {
            typetmp += rectify_type(field->type());
            if (field->bitfield() > 0)
            {
                typetmp += ":" + std::to_string(field->bitfield());
            }
        }
        if (!field->default_value().empty())
        {
            typetmp += "=" + field->default_value();
        }

        index += rectify_name(field->name()) + ":" + typetmp;
        index_types += typetmp;
        index_names += ",";
        index_types += ",";
        index += ",";
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

    for (const auto &members : structdef->methods())
    {
        result += generate_node_cxx(members, indent, _namespace);
    }

    for (const auto &field : structdef->fields())
    {
        if (field->arr_size() != std::numeric_limits<uint64_t>::max())
        {
            result += mkpadding(indent) + rectify_type(field->type()) + " " + rectify_name(field->name());

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
            result += mkpadding(indent) + "vector<" + rectify_type(field->type()) + "> " + rectify_name(field->name());

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

    result += mkpadding(indent) + "constexpr auto j_" + struct_name + "_size = sizeof(" + struct_name + ");\n" + mkpadding(indent) + "/* End Structure " + struct_name + " */\n\n";

    return result;
}

static std::string generate_function_definition_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent, std::string &_namespace)
{
    auto funcdef = std::static_pointer_cast<FunctionDefinition>(node);
    std::string result = mkpadding(indent);

    if (funcdef->name() == "main" && _namespace == "_jxx::")
    {
        g_has_main_mutex.lock();
        if (g_has_main)
        {
            g_has_main_mutex.unlock();
            throw std::runtime_error("Multiple main() functions defined");
        }
        g_has_main = true;
        g_has_main_mutex.unlock();
    }

    if (funcdef->return_type().empty())
    {
        result += "[[noreturn]] _void";
    }
    else
    {
        if (funcdef->return_arr_size() == std::numeric_limits<uint64_t>::max())
        {
            result += "vector<" + rectify_type(funcdef->return_type()) + ">";
        }
        else if (funcdef->return_arr_size() > 0)
        {
            result += "std::array<" + rectify_type(funcdef->return_type()) + ", " + std::to_string(funcdef->return_arr_size()) + "> ";
        }
        else
        {
            result += rectify_type(funcdef->return_type());
        }
    }

    result += " " + rectify_name(funcdef->name()) + "(";

    for (size_t i = 0; i < funcdef->parameters().size(); i++)
    {
        result += generate_function_parameter_cxx(funcdef->parameters()[i], indent, _namespace);

        if (i != funcdef->parameters().size() - 1)
        {
            result += ", ";
        }
    }

    result += ")\n";

    result += generate_block_cxx(funcdef->block(), indent, _namespace);

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

static std::string generate_struct_method_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent, std::string &_namespace)
{
    auto funcdef = std::static_pointer_cast<StructMethod>(node);
    std::string result = mkpadding(indent);

    if (funcdef->type().empty())
    {
        result += "[[noreturn]] _void";
    }
    else
    {
        result += rectify_type(funcdef->type());
    }

    result += " " + rectify_name(funcdef->name()) + "(";

    for (size_t i = 0; i < funcdef->parameters().size(); i++)
    {
        result += generate_function_parameter_cxx(funcdef->parameters()[i], indent, _namespace);

        if (i != funcdef->parameters().size() - 1)
        {
            result += ", ";
        }
    }

    result += ")\n";

    result += generate_block_cxx(funcdef->block(), indent, _namespace);

    result += "\n";

    return result;
}

static std::string generate_node_cxx(const std::shared_ptr<jcc::GenericNode> &node, uint32_t &indent, std::string &_namespace)
{
    if (node == nullptr)
    {
        throw std::runtime_error("Node is null");
    }

    switch (node->type())
    {
    case NodeType::Block:
        return generate_block_cxx(node, indent, _namespace);
    case NodeType::TypeDeclaration:
        return generate_typedef_cxx(node, indent, _namespace);
    case NodeType::StructDeclaration:
        return generate_struct_declaration_cxx(node, indent, _namespace);
    case NodeType::UnionDeclaration:
        return generate_union_declaration_cxx(node, indent, _namespace);
    case NodeType::EnumDeclaration:
        return generate_enum_declaration_cxx(node, indent, _namespace);
    case NodeType::FunctionDeclaration:
        return generate_function_declaration_cxx(node, indent, _namespace);
    case NodeType::ClassDeclaration:
        return generate_class_declaration_cxx(node, indent, _namespace);
    case NodeType::ExternalDeclaration:
        return generate_extern_declaration_cxx(node, indent, _namespace);
    case NodeType::NamespaceDeclaration:
        return generate_namespace_declaration_cxx(node, indent, _namespace);

    case NodeType::NamespaceDefinition:
        return generate_namespace_definition_cxx(node, indent, _namespace);
    case NodeType::StructDefinition:
        return generate_struct_definition_cxx(node, indent, _namespace);
    case NodeType::StructMethod:
        return generate_struct_method_cxx(node, indent, _namespace);
    case NodeType::FunctionDefinition:
        return generate_function_definition_cxx(node, indent, _namespace);

    case NodeType::ReturnStatement:
    {
        auto retstmt = std::static_pointer_cast<ReturnStatement>(node);
        std::string result = mkpadding(indent) + "return";

        if (retstmt->expression() != nullptr)
        {
            result += " " + generate_node_cxx(retstmt->expression(), indent, _namespace);
        }

        result += ";\n";

        return result;
    }

    case NodeType::BinaryExpression:
        return generate_node_cxx(std::static_pointer_cast<BinaryExpression>(node)->left(), indent, _namespace) + " " + std::static_pointer_cast<BinaryExpression>(node)->op() + " " + generate_node_cxx(std::static_pointer_cast<BinaryExpression>(node)->right(), indent, _namespace);
    case NodeType::UnaryExpression:
        return std::static_pointer_cast<UnaryExpression>(node)->op() + generate_node_cxx(std::static_pointer_cast<UnaryExpression>(node)->expression(), indent, _namespace);
    case NodeType::CastExpression:
        return "(" + std::static_pointer_cast<CastExpression>(node)->type() + ")" + generate_node_cxx(std::static_pointer_cast<CastExpression>(node)->expression(), indent, _namespace);
    case NodeType::LiteralExpression:
        return std::static_pointer_cast<LiteralExpression>(node)->value();
    case NodeType::CallExpression:
    {
        auto callexpr = std::static_pointer_cast<CallExpression>(node);
        std::string result = rectify_name(callexpr->name()) + "(";

        for (const auto &arg : callexpr->arguments())
        {
            result += generate_node_cxx(arg, indent, _namespace);
        }

        result += ")";

        return result;
    }
    case NodeType::StringLiteralExpression:
        return "\"" + std::static_pointer_cast<StringLiteralExpression>(node)->value() + "\"";
    case NodeType::CharLiteralExpression:
        return "'" + std::static_pointer_cast<CharLiteralExpression>(node)->value() + "'";
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
    std::string current_namespace;

    if (target != TargetLanguage::CXX)
    {
        panic("Unsupported target language", m_messages);
    }

    result += generate_node_cxx(ast->root(), indent, current_namespace);

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

namespace _jxx
{
    typedef bool _bit;
    typedef int8_t _char;
    typedef uint8_t _byte;
    typedef uint16_t _short;
    typedef uint16_t _word;
    typedef int32_t _int;
    typedef uint32_t _dword;
    typedef int64_t _long;
    typedef uint64_t _qword;
    typedef float _float;
    typedef double _double;
    typedef ssize_t _intn;
    typedef size_t _uintn;
    typedef uintptr_t _address;
    typedef const char *_string;
    typedef void *_routine;
#define _null nullptr
#define _void void
#define _vector std::vector
};)";

const std::string structure_generic_baseclass = R"(    /* Begin Common Sink functions */

#ifdef __linux__
#include <execinfo.h>

    // no warn unused
    [[gnu::used]] [[noreturn]] static void _panic(_jxx::_string message)
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
    [[gnu::used]] [[noreturn]] static void _panic(_jxx::_string message)
    {
        std::cerr << "PANIC: " << message << std::endl;
        abort();

        while (true) {}
    }  
#endif
    
    
    /* Begin Generic Structure Base Class */
    typedef _jxx::_qword typeid_t;

    static std::map<typeid_t, _jxx::_string> g_typenames_mapping = {!!!/* JCC_TYPENAMES_MAPPING */!!!};
    static std::map<_jxx::_string, typeid_t> g_typenames_mapping_reverse = {!!!/* JCC_TYPENAMES_MAPPING_REVERSE */!!!};
    static std::map<std::string, _jxx::_uintn> g_builtin_sizes = {{"_bit", 1}, {"_byte", 1}, {"_char", 1}, {"_word", 2}, {"_short", 2}, {"_dword", 4}, {"_int", 4}, {"_float", 4}, {"_double", 8}, {"_qword", 8}, {"_long", 8}, {"_string", 8}, {"_routine", 8}, {"_address", 8}};
    struct ReflectiveEntry {
        _jxx::_string field_name;
        _jxx::_string type;
        _jxx::_uintn count;
    };

    static std::map<typeid_t, std::vector<ReflectiveEntry>> g_reflective_entries = {!!!/* JCC_REFLECTIVE_ENTRIES */!!!};

    template <typeid_t T>
    class StructGeneric
    {
    protected:
        static const typeid_t m_typeid = T;
        static std::map<typeid_t, std::map<_jxx::_string, const void *>> m_attributes;

    public:
        inline typeid_t _typeid() const
        {
            return m_typeid;
        }

        inline _jxx::_string _typename() const
        {
            return g_typenames_mapping.at(m_typeid);
        }

        inline bool _has(_jxx::_string name) const
        {
            return m_attributes.at(m_typeid).find(name) != m_attributes.at(m_typeid).end();
        }

        bool _hasfield(_jxx::_string name) const
        {
            _jxx::_string index_names = this->_get("_index_names");
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

                if (strncmp(index_names, name, strlen(name)) == 0)
                {
                    return true;
                }

                index_names++;
            }

            return false;
        }

        inline void _set(_jxx::_string name, _jxx::_string value) const
        {
            m_attributes[m_typeid][name] = (void *)value;
        }

        inline void _set(_jxx::_string name, long value) const
        {
            m_attributes[m_typeid][name] = (void *)value;
        }

        inline _jxx::_string _get(_jxx::_string name) const
        {
            return (_jxx::_string)m_attributes.at(m_typeid).at(name);
        }

        inline long _getint(_jxx::_string name) const
        {
            return (_jxx::_qword)m_attributes.at(m_typeid).at(name);
        }

        static inline _jxx::_string _gettypename(typeid_t type)
        {
            return g_typenames_mapping.at(type);
        }

        static inline typeid_t _gettypeid(_jxx::_string name)
        {
            return g_typenames_mapping_reverse.at(name);
        }

        static size_t _sizeof(typeid_t id)
        {
            size_t size = 0;
            for (auto &field_type : g_reflective_entries.at(id))
            {
                if (g_builtin_sizes.contains(field_type.type))
                {
                    size += g_builtin_sizes.at(field_type.type) * field_type.count;
                }
                else
                {
                    size += _sizeof(_gettypeid(field_type.type)) * field_type.count;
                }
            }

            return size;
        }

        inline size_t _sizeof() const
        {
            return _sizeof(m_typeid);
        }
    };

    template <typeid_t T>
    std::map<typeid_t, std::map<_jxx::_string, const void *>> StructGeneric<T>::m_attributes;
    template <typeid_t T>
    const typeid_t StructGeneric<T>::m_typeid;
    /* End Generic Structure Base Class */)";

bool jcc::CompilationUnit::join_to_output_cxx(const std::vector<std::pair<std::string, std::string>> &sources, const std::string &output_cxx)
{
    std::ofstream output_cxx_stream(output_cxx, std::ios::binary);

    if (!output_cxx_stream.is_open())
    {
        return false;
    }

    jcc::crypto::SHA256_CTX sha256_ctx;
    jcc::crypto::sha256_init(sha256_ctx);

    time_t current_time = std::time(nullptr);

    std::string sources_formatted = "// Sources: [";
    size_t line_width = 10;
    for (const auto &source : sources)
    {
        if (line_width + source.first.size() + 4 > 64)
        {
            for (size_t i = 0; i < 64 - line_width; i++)
            {
                sources_formatted += " ";
            }
            sources_formatted += " //\n// ";
            line_width = 0;
        }

        sources_formatted += "\"" + source.first;
        if (source != sources.back())
        {
            sources_formatted += "\", ";
            line_width += source.first.size() + 4;
        }
        else
        {
            sources_formatted += "\"";
            line_width += source.first.size() + 3;
        }
    }
    sources_formatted += "]";
    for (size_t i = 0; i < 64 - line_width; i++)
    {
        sources_formatted += " ";
    }
    sources_formatted += " //";

    // print header
    output_cxx_stream << "//==================================================================//\n"
                      << "// Info: J++ Transpiled Code                                        //\n"
                      << "// Type: C++-20                                                     //\n"
                      << "// Version: J++-dev                                                 //\n"
                      << sources_formatted << "\n"
                      << "// Platform: "
                      << "independent"
                      << "                                            //\n"
                      << "// Date: " << std::put_time(std::gmtime(&current_time), "%Y-%m-%dT%H:%M:%S %z %Z") << "                              //\n"
                      << "// Copyright (C) 2023 Wesley C. Jones. All rights reserved.         //\n"
                      << "//==================================================================//\n"
                      << "\n";

    output_cxx_stream << typedef_commons << "\n\n";
    // replace '!!!/* JCC_TYPENAMES_MAPPING */!!!' with the actual mapping
    // only replace first occurrence
    if (!g_typenames_mapping.empty())
    {

        std::string tmp = structure_generic_baseclass;
        std::string new_value;
        std::string new_value_reverse;
        for (auto it = g_typenames_mapping.begin(); it != g_typenames_mapping.end(); ++it)
        {
            new_value += "{" + std::to_string(it->first) + ", \"" + it->second + "\"}";
            new_value_reverse += "{\"" + it->second + "\", " + std::to_string(it->first) + "}";

            if (std::next(it) != g_typenames_mapping.end())
            {
                new_value += ", ";
                new_value_reverse += ", ";
            }
        }
        size_t pos = tmp.find("!!!/* JCC_TYPENAMES_MAPPING */!!!");
        if (pos != std::string::npos)
        {
            tmp.replace(pos, 33, new_value);
        }
        size_t pos_reverse = tmp.find("!!!/* JCC_TYPENAMES_MAPPING_REVERSE */!!!", pos);

        if (pos_reverse != std::string::npos)
        {
            tmp.replace(pos_reverse, 41, new_value_reverse);
        }

        std::string reflective_entries;

        for (auto it = g_reflective_entries.begin(); it != g_reflective_entries.end(); ++it)
        {
            reflective_entries += "{" + std::to_string(it->first) + ", {";

            for (size_t i = 0; i < it->second.size(); i++)
            {
                reflective_entries += "{\"" + it->second[i].field_name + "\", \"" + it->second[i].type + "\", " + std::to_string(it->second[i].count) + "}";

                if (i != it->second.size() - 1)
                {
                    reflective_entries += ", ";
                }
            }

            reflective_entries += "}}";

            if (std::next(it) != g_reflective_entries.end())
            {
                reflective_entries += ", ";
            }
        }

        pos = tmp.find("!!!/* JCC_REFLECTIVE_ENTRIES */!!!");
        if (pos != std::string::npos)
        {
            tmp.replace(pos, 34, reflective_entries);
        }

        output_cxx_stream << "namespace _j {\n";
        output_cxx_stream << tmp << "\n";
        output_cxx_stream << "};\n\n";
    }

    for (const auto &source : sources)
    {
        std::ifstream input_cxx_stream(source.second, std::ios::binary);

        if (!input_cxx_stream.is_open())
        {
            output_cxx_stream.close();
            return false;
        }

        std::string fname_padded = "\"" + source.first + "\"";
        if (fname_padded.size() > 58)
        {
            fname_padded = "..." + fname_padded.substr(fname_padded.size() - 55);
        }
        else
        {
            fname_padded.resize(58, ' ');
        }

        output_cxx_stream << "//==================================================================//\n"
                          << "// File: \"" << source.first << "\" //\n"
                          << "//==================================================================//\n"
                          << "\n";

        std::stringstream input_cxx_stream_copy;
        input_cxx_stream_copy << input_cxx_stream.rdbuf();
        std::string file_contents = input_cxx_stream_copy.str();

        // update hash
        jcc::crypto::sha256_update(sha256_ctx, (uint8_t *)file_contents.c_str(), file_contents.size());

        // write to output
        output_cxx_stream << file_contents;

        output_cxx_stream << "//==================================================================//\n"
                          << "// EOF: " << fname_padded << "  //\n"
                          << "//==================================================================//\n";

        if (source != sources.back())
        {
            output_cxx_stream << "\n";
        }

        input_cxx_stream.close();
    }

    g_has_main_mutex.lock();
    if (g_has_main)
    {
        output_cxx_stream << "\nint main(int argc, char **argv)\n{\n    return _jxx::_main();\n}\n";
    }
    g_has_main_mutex.unlock();

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