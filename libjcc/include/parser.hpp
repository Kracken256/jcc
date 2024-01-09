#ifndef _JCC_PARSER_HPP_
#define _JCC_PARSER_HPP_

#include <string>
#include <vector>
#include <exception>
#include <stdexcept>
#include <variant>
#include <memory>
#include "lexer.hpp"

namespace jcc
{
    enum class NodeType
    {
        Invalid,

        ///===================
        /// Common base types
        ///===================
        Expression,
        Statement,
        Declaration,
        Definition,
        Block,

        ///===================
        /// Other
        ///===================
        RawNode,

        ///===================
        /// Declarations
        ///===================

        TypeDeclaration,
        StructDeclaration,
        UnionDeclaration,
        EnumDeclaration,
        FunctionParameter,
        FunctionDeclaration,
        ClassDeclaration,
        ExternalDeclaration,
        SubsystemDeclaration,

        ///===================
        /// Definitions
        ///===================

        SubsystemDefinition,
        StructField,
        StructMethod,
        StructAttribute,
        StructDefinition,
        FunctionDefinition,

        ///===================
        /// Expressions
        ///===================
        BinaryExpression,
        UnaryExpression,
        CastExpression,
        NullExpression,
        LiteralExpression,
        CallExpression,
        StringLiteralExpression,
        CharLiteralExpression,
        IntegerLiteralExpression,
        FloatingPointLiteralExpression,
        BooleanLiteralExpression,

        ///===================
        /// Statements
        ///===================
        ReturnStatement,
    };

    enum class VisiblityModifier
    {
        Public,
        Protected,
        Private,
    };

    class GenericNode
    {
    public:
        GenericNode(NodeType type = NodeType::Invalid) : m_type(type) {}
        virtual ~GenericNode() {}

        NodeType type() const { return m_type; }

        virtual std::string to_string() const { return "GenericNode()"; }
        virtual std::string to_json() const { return "{\"type\":\"generic_node\"}"; }

    protected:
        NodeType m_type;

        std::string json_escape(const std::string &str) const;
    };

    class RawNode : public GenericNode
    {
    public:
        RawNode(NodeType type = NodeType::RawNode) : GenericNode(type) {}
        RawNode(const std::string &value, NodeType type = NodeType::RawNode) : GenericNode(type), m_value(value) {}
        virtual ~RawNode() {}

        const std::string &value() const { return m_value; }
        std::string &value() { return m_value; }

        std::string to_string() const override { return "RawNode(" + m_value + ")"; }
        std::string to_json() const override { return "{\"type\":\"raw_node\",\"value\":\"" + json_escape(m_value) + "\"}"; }

    protected:
        std::string m_value;
    };

    class Expression : public GenericNode
    {
    public:
        Expression(NodeType type = NodeType::Expression) : GenericNode(type) {}
        virtual ~Expression() {}

        std::string to_string() const override { return "Expression()"; }
        std::string to_json() const override { return "{\"type\":\"expression\"}"; }
    };

    class Statement : public GenericNode
    {
    public:
        Statement(NodeType type = NodeType::Statement) : GenericNode(type) {}
        virtual ~Statement() {}

        std::string to_string() const override { return "Statement()"; }
        std::string to_json() const override { return "{\"type\":\"statement\"}"; }
    };

    class Declaration : public GenericNode
    {
    public:
        Declaration(NodeType type = NodeType::Declaration) : GenericNode(type) {}
        virtual ~Declaration() {}

        std::string to_string() const override { return "Declaration()"; }
        std::string to_json() const override { return "{\"type\":\"declaration\"}"; }
    };

    class Definition : public GenericNode
    {
    public:
        Definition(NodeType type = NodeType::Definition) : GenericNode(type) {}
        virtual ~Definition() {}

        std::string to_string() const override { return "Definition()"; }
        std::string to_json() const override { return "{\"type\":\"definition\"}"; }
    };

    class Block : public GenericNode
    {
    public:
        Block(NodeType type = NodeType::Block) : GenericNode(type), m_render_braces(true) {}
        Block(const std::vector<std::shared_ptr<GenericNode>> &children, bool render_braces = true) : GenericNode(NodeType::Block), m_children(children), m_render_braces(render_braces) {}
        virtual ~Block() {}

        const std::vector<std::shared_ptr<GenericNode>> &children() const { return m_children; }
        std::vector<std::shared_ptr<GenericNode>> &children() { return m_children; }
        const bool &render_braces() const { return m_render_braces; }
        bool &render_braces() { return m_render_braces; }

        void push(std::shared_ptr<GenericNode> node) { m_children.push_back(node); }

        std::string to_string() const override;
        std::string to_json() const override;

    protected:
        std::vector<std::shared_ptr<GenericNode>> m_children;
        bool m_render_braces;
    };

    ///=================================================================================================
    /// Type
    ///=================================================================================================

    class TypeDeclaration : public Declaration
    {
    public:
        TypeDeclaration(NodeType type = NodeType::TypeDeclaration) : Declaration(type) {}
        TypeDeclaration(const std::string &alias, const std::string &_typename, NodeType type = NodeType::TypeDeclaration) : Declaration(type), m_alias(alias), m_typename(_typename) {}
        virtual ~TypeDeclaration() {}

        /// @brief Get the name of the typedef
        /// @return Name of the type
        const std::string &alias() const { return m_alias; }

        /// @brief Get the name of the typedef
        /// @return Name of the type
        std::string &alias() { return m_alias; }

        /// @brief Get the name of the type (real name)
        /// @return Name of the type
        const std::string &type_name() const { return m_typename; }

        /// @brief Get the name of the type (real name)
        /// @return Name of the type
        std::string &type_name() { return m_typename; }

        std::string to_string() const override { return "TypeDeclaration(" + m_alias + ", " + m_typename + ")"; }
        std::string to_json() const override { return "{\"type\":\"type_declaration\",\"alias\":\"" + json_escape(m_alias) + "\",\"typename\":\"" + json_escape(m_typename) + "\"}"; }

    protected:
        std::string m_alias;
        std::string m_typename;
    };

    class StructDeclaration : public TypeDeclaration
    {
    public:
        StructDeclaration(NodeType type = NodeType::StructDeclaration) : TypeDeclaration(type) {}
        StructDeclaration(const std::string &name) : TypeDeclaration(name, "", NodeType::StructDeclaration), m_name(name) {}
        virtual ~StructDeclaration() {}

        const std::string &name() const { return m_name; }
        std::string &name() { return m_name; }

        std::string to_string() const override;
        std::string to_json() const override;

    protected:
        std::string m_name;
    };

    class UnionDeclaration : public TypeDeclaration
    {
    public:
        UnionDeclaration(NodeType type = NodeType::UnionDeclaration) : TypeDeclaration(type) {}
        UnionDeclaration(const std::string &name) : TypeDeclaration(name, "", NodeType::UnionDeclaration) {}
        virtual ~UnionDeclaration() {}

        const std::string &name() const { return m_name; }
        std::string &name() { return m_name; }

        std::string to_string() const override;

        std::string to_json() const override;

    protected:
        std::string m_name;
    };

    class EnumDeclaration : public TypeDeclaration
    {
    public:
        EnumDeclaration(NodeType type = NodeType::EnumDeclaration) : TypeDeclaration(type) {}
        EnumDeclaration(const std::string &name) : TypeDeclaration(name, "", NodeType::EnumDeclaration) {}
        virtual ~EnumDeclaration() {}

        const std::string &name() const { return m_name; }
        std::string &name() { return m_name; }

        std::string to_string() const override;

        std::string to_json() const override;

    protected:
        std::string m_name;
    };

    class FunctionParameter : public GenericNode
    {
    public:
        FunctionParameter(NodeType type = NodeType::FunctionParameter) : GenericNode(type), m_arr_size(0), m_is_const(false), m_is_reference(false) {}
        FunctionParameter(const std::string &name, const std::string &type, const std::shared_ptr<Expression> &default_value, uint64_t arr_size = 0, bool is_const = false, bool is_reference = false) : GenericNode(NodeType::FunctionParameter), m_name(name), m_type(type), m_arr_size(arr_size), m_default_value(default_value), m_is_const(is_const), m_is_reference(is_reference) {}
        virtual ~FunctionParameter() {}

        const std::string &name() const { return m_name; }
        std::string &name() { return m_name; }

        const std::string &type() const { return m_type; }
        std::string &type() { return m_type; }

        const std::shared_ptr<Expression> &default_value() const { return m_default_value; }
        std::shared_ptr<Expression> &default_value() { return m_default_value; }

        const uint64_t &arr_size() const { return m_arr_size; }
        uint64_t &arr_size() { return m_arr_size; }

        std::string to_string() const override;
        std::string to_json() const override;

        const bool &is_const() const { return m_is_const; }
        bool &is_const() { return m_is_const; }

        const bool &is_reference() const { return m_is_reference; }
        bool &is_reference() { return m_is_reference; }

    protected:
        std::string m_name;
        std::string m_type;
        uint64_t m_arr_size;
        std::shared_ptr<Expression> m_default_value;
        bool m_is_const;
        bool m_is_reference;
    };

    class FunctionDeclaration : public Declaration
    {
    public:
        FunctionDeclaration(NodeType type = NodeType::FunctionDeclaration) : Declaration(type), m_return_arr_size(0) {}
        FunctionDeclaration(const std::string &name, const std::string &return_type, std::vector<std::shared_ptr<FunctionParameter>> parameters, uint64_t return_arr_size = 0) : Declaration(NodeType::FunctionDeclaration), m_return_type(return_type), m_parameters(parameters), m_name(name), m_return_arr_size(return_arr_size) {}
        virtual ~FunctionDeclaration() {}

        const std::string &return_type() const { return m_return_type; }
        std::string &return_type() { return m_return_type; }

        const uint64_t &return_arr_size() const { return m_return_arr_size; }
        uint64_t &return_arr_size() { return m_return_arr_size; }

        const std::vector<std::shared_ptr<FunctionParameter>> &parameters() const { return m_parameters; }
        std::vector<std::shared_ptr<FunctionParameter>> &parameters() { return m_parameters; }

        const std::string &name() const { return m_name; }
        std::string &name() { return m_name; }

        std::string to_string() const override;
        std::string to_json() const override;

    protected:
        std::string m_return_type;
        std::vector<std::shared_ptr<FunctionParameter>> m_parameters;
        std::string m_name;
        uint64_t m_return_arr_size;
    };

    class ClassDeclaration : public TypeDeclaration
    {
    public:
        ClassDeclaration(NodeType type = NodeType::ClassDeclaration) : TypeDeclaration(type) {}
        ClassDeclaration(const std::string &name) : TypeDeclaration(name, "", NodeType::ClassDeclaration), m_name(name) {}
        virtual ~ClassDeclaration() {}

        const std::string &name() const { return m_name; }
        std::string &name() { return m_name; }

        std::string to_string() const override;
        std::string to_json() const override;

    protected:
        std::string m_name;
    };

    class ExternalDeclaration : public Declaration
    {
    public:
        ExternalDeclaration(NodeType type = NodeType::ExternalDeclaration) : Declaration(type) {}
        ExternalDeclaration(std::shared_ptr<Declaration> declaration) : Declaration(NodeType::ExternalDeclaration), m_declaration(declaration) {}
        virtual ~ExternalDeclaration() {}

        const std::shared_ptr<Declaration> &declaration() const { return m_declaration; }
        std::shared_ptr<Declaration> &declaration() { return m_declaration; }

        std::string to_string() const override { return "ExternalDeclaration(" + m_declaration->to_string() + ")"; }

        std::string to_json() const override { return "{\"type\":\"external_declaration\",\"declaration\":" + m_declaration->to_json() + "}"; }

    protected:
        std::shared_ptr<Declaration> m_declaration;
    };

    class SubsystemDeclaration : public Declaration
    {
    public:
        SubsystemDeclaration(NodeType type = NodeType::SubsystemDeclaration) : Declaration(type) {}
        SubsystemDeclaration(const std::string &name, const std::vector<std::string> &dependencies = {}) : Declaration(NodeType::SubsystemDeclaration), m_name(name), m_dependencies(dependencies) {}
        virtual ~SubsystemDeclaration() {}

        const std::string &name() const { return m_name; }
        std::string &name() { return m_name; }

        const std::vector<std::string> &dependencies() const { return m_dependencies; }
        std::vector<std::string> &dependencies() { return m_dependencies; }

        std::string to_string() const override { return "SubsystemDeclaration(" + m_name + ")"; }
        std::string to_json() const override { return "{\"type\":\"subsystem_declaration\",\"name\":\"" + json_escape(m_name) + "\"}"; }

    protected:
        std::string m_name;
        std::vector<std::string> m_dependencies;
    };

    ///=================================================================================================
    /// Definitions types
    ///=================================================================================================

    class SubsystemDefinition : public Definition
    {
    public:
        SubsystemDefinition(NodeType type = NodeType::SubsystemDefinition) : Definition(type) {}
        SubsystemDefinition(const std::string &name, std::shared_ptr<Block> block, const std::vector<std::string> &dependencies = {}) : Definition(NodeType::SubsystemDefinition), m_name(name), m_block(block), m_dependencies(dependencies) {}
        virtual ~SubsystemDefinition() {}

        const std::string &name() const { return m_name; }
        std::string &name() { return m_name; }

        const std::vector<std::string> &dependencies() const { return m_dependencies; }
        std::vector<std::string> &dependencies() { return m_dependencies; }

        const std::shared_ptr<Block> &block() const { return m_block; }
        std::shared_ptr<Block> &block() { return m_block; }

        std::string to_string() const override;
        std::string to_json() const override;

    protected:
        std::string m_name;
        std::shared_ptr<Block> m_block;
        std::vector<std::string> m_dependencies;
    };

    class StructAttribute : public GenericNode
    {
    public:
        StructAttribute(NodeType type = NodeType::StructAttribute) : GenericNode(type) {}
        StructAttribute(const std::string &name, const std::string &value) : GenericNode(NodeType::StructAttribute), m_name(name), m_value(value) {}
        virtual ~StructAttribute() {}

        const std::string &name() const { return m_name; }
        std::string &name() { return m_name; }

        const std::string &value() const { return m_value; }
        std::string &value() { return m_value; }

        std::string to_string() const override { return "StructAttribute(" + m_name + ", " + m_value + ")"; }
        std::string to_json() const override { return "{\"type\":\"struct_attribute\",\"name\":\"" + json_escape(m_name) + "\",\"value\":\"" + json_escape(m_value) + "\"}"; }

    protected:
        std::string m_name;
        std::string m_value;
    };

    class StructField : public GenericNode
    {
    public:
        StructField(NodeType type = NodeType::StructField) : GenericNode(type), m_bitfield(0), m_arr_size(0) {}
        StructField(const std::string &name, const std::string &type, uint64_t bitfield, const std::string &default_value, uint64_t arr_size = 0, const std::vector<std::shared_ptr<StructAttribute>> &attributes = {}) : GenericNode(NodeType::StructField), m_name(name), m_type(type), m_bitfield(bitfield), m_default_value(default_value), m_arr_size(arr_size), m_attributes(attributes) {}
        virtual ~StructField() {}

        const std::string &name() const { return m_name; }
        std::string &name() { return m_name; }

        const std::string &type() const { return m_type; }
        std::string &type() { return m_type; }

        const std::string &default_value() const { return m_default_value; }
        std::string &default_value() { return m_default_value; }

        const uint64_t &bitfield() const { return m_bitfield; }
        uint64_t &bitfield() { return m_bitfield; }

        const uint64_t &arr_size() const { return m_arr_size; }
        uint64_t &arr_size() { return m_arr_size; }

        const std::vector<std::shared_ptr<StructAttribute>> &attributes() const { return m_attributes; }
        std::vector<std::shared_ptr<StructAttribute>> &attributes() { return m_attributes; }

        std::string to_string() const override { return "StructField(" + m_name + ", " + m_type + ")"; }
        std::string to_json() const override;

    protected:
        std::string m_name;
        std::string m_type;
        uint64_t m_bitfield;
        std::string m_default_value;
        uint64_t m_arr_size;
        std::vector<std::shared_ptr<StructAttribute>> m_attributes;
    };

    class StructMethod : public GenericNode
    {
    public:
        StructMethod(NodeType type = NodeType::StructMethod) : GenericNode(type) {}
        StructMethod(const std::string &name, const std::string &type, const std::vector<std::shared_ptr<FunctionParameter>> &parameters, std::shared_ptr<Block> block) : GenericNode(NodeType::StructMethod), m_name(name), m_type(type), m_parameters(parameters), m_block(block) {}
        virtual ~StructMethod() {}

        const std::string &name() const { return m_name; }
        std::string &name() { return m_name; }

        const std::string &type() const { return m_type; }
        std::string &type() { return m_type; }

        const std::vector<std::shared_ptr<FunctionParameter>> &parameters() const { return m_parameters; }
        std::vector<std::shared_ptr<FunctionParameter>> &parameters() { return m_parameters; }

        const std::shared_ptr<Block> &block() const { return m_block; }
        std::shared_ptr<Block> &block() { return m_block; }

        std::string to_string() const override { return "StructMethod(" + m_name + ", " + m_type + ")"; }
        std::string to_json() const override;

    protected:
        std::string m_name;
        std::string m_type;
        std::vector<std::shared_ptr<FunctionParameter>> m_parameters;
        std::shared_ptr<Block> m_block;
    };

    class StructDefinition : public Definition
    {
    public:
        StructDefinition(NodeType type = NodeType::StructDefinition) : Definition(type) {}
        StructDefinition(const std::string &name, std::vector<std::shared_ptr<StructField>> fields, const std::vector<std::shared_ptr<StructMethod>> &methods, bool packed = false) : Definition(NodeType::StructDefinition), m_name(name), m_fields(fields), m_methods(methods), m_packed(packed) {}
        virtual ~StructDefinition() {}

        const std::string &name() const { return m_name; }
        std::string &name() { return m_name; }

        const std::vector<std::shared_ptr<StructField>> &fields() const { return m_fields; }
        std::vector<std::shared_ptr<StructField>> &fields() { return m_fields; }

        const std::vector<std::shared_ptr<StructMethod>> &methods() const { return m_methods; }
        std::vector<std::shared_ptr<StructMethod>> &methods() { return m_methods; }

        const bool &packed() const { return m_packed; }
        bool &packed() { return m_packed; }

        std::string to_string() const override;
        std::string to_json() const override;

    protected:
        std::string m_name;
        std::vector<std::shared_ptr<StructField>> m_fields;
        std::vector<std::shared_ptr<StructMethod>> m_methods;
        bool m_packed;
    };

    class FunctionDefinition : public Definition
    {
    public:
        FunctionDefinition(NodeType type = NodeType::FunctionDefinition) : Definition(type), m_return_arr_size(0) {}
        FunctionDefinition(const std::string &name, const std::string &return_type, std::vector<std::shared_ptr<FunctionParameter>> parameters, std::shared_ptr<Block> block, uint64_t return_arr_size = 0) : Definition(NodeType::FunctionDefinition), m_return_type(return_type), m_parameters(parameters), m_name(name), m_block(block), m_return_arr_size(return_arr_size) {}
        virtual ~FunctionDefinition() {}

        const std::string &return_type() const { return m_return_type; }
        std::string &return_type() { return m_return_type; }

        const uint64_t &return_arr_size() const { return m_return_arr_size; }
        uint64_t &return_arr_size() { return m_return_arr_size; }

        const std::vector<std::shared_ptr<FunctionParameter>> &parameters() const { return m_parameters; }
        std::vector<std::shared_ptr<FunctionParameter>> &parameters() { return m_parameters; }

        const std::string &name() const { return m_name; }
        std::string &name() { return m_name; }

        const std::shared_ptr<Block> &block() const { return m_block; }
        std::shared_ptr<Block> &block() { return m_block; }

        std::string to_string() const override;
        std::string to_json() const override;

    protected:
        std::string m_return_type;
        std::vector<std::shared_ptr<FunctionParameter>> m_parameters;
        std::string m_name;
        std::shared_ptr<Block> m_block;
        uint64_t m_return_arr_size;
    };

    ///=================================================================================================
    /// Expressions types
    ///=================================================================================================

    class BinaryExpression : public Expression
    {
    public:
        BinaryExpression(NodeType type = NodeType::BinaryExpression) : Expression(type) {}
        BinaryExpression(const std::string &op, std::shared_ptr<Expression> left, std::shared_ptr<Expression> right) : Expression(NodeType::BinaryExpression), m_op(op), m_left(left), m_right(right) {}
        virtual ~BinaryExpression() {}

        const std::string &op() const { return m_op; }
        std::string &op() { return m_op; }

        const std::shared_ptr<Expression> &left() const { return m_left; }
        std::shared_ptr<Expression> &left() { return m_left; }

        const std::shared_ptr<Expression> &right() const { return m_right; }
        std::shared_ptr<Expression> &right() { return m_right; }

        std::string to_string() const override { return "BinaryExpression(" + m_op + ", " + m_left->to_string() + ", " + m_right->to_string() + ")"; }
        std::string to_json() const override { return "{\"type\":\"binary_expression\",\"op\":\"" + json_escape(m_op) + "\",\"left\":" + m_left->to_json() + ",\"right\":" + m_right->to_json() + "}"; }

    protected:
        std::string m_op;
        std::shared_ptr<Expression> m_left;
        std::shared_ptr<Expression> m_right;
    };

    class UnaryExpression : public Expression
    {
    public:
        UnaryExpression(NodeType type = NodeType::UnaryExpression) : Expression(type) {}
        UnaryExpression(const std::string &op, std::shared_ptr<Expression> expression) : Expression(NodeType::UnaryExpression), m_op(op), m_expression(expression) {}
        virtual ~UnaryExpression() {}

        const std::string &op() const { return m_op; }
        std::string &op() { return m_op; }

        const std::shared_ptr<Expression> &expression() const { return m_expression; }
        std::shared_ptr<Expression> &expression() { return m_expression; }

        std::string to_string() const override { return "UnaryExpression(" + m_op + ", " + m_expression->to_string() + ")"; }
        std::string to_json() const override { return "{\"type\":\"unary_expression\",\"op\":\"" + json_escape(m_op) + "\",\"expression\":" + m_expression->to_json() + "}"; }

    protected:
        std::string m_op;
        std::shared_ptr<Expression> m_expression;
    };

    class CastExpression : public Expression
    {
    public:
        CastExpression(NodeType type = NodeType::CastExpression) : Expression(type) {}
        CastExpression(const std::string &type, std::shared_ptr<Expression> expression) : Expression(NodeType::CastExpression), m_type(type), m_expression(expression) {}
        virtual ~CastExpression() {}

        const std::string &type() const { return m_type; }
        std::string &type() { return m_type; }

        const std::shared_ptr<Expression> &expression() const { return m_expression; }
        std::shared_ptr<Expression> &expression() { return m_expression; }

        std::string to_string() const override { return "CastExpression(" + m_type + ", " + m_expression->to_string() + ")"; }
        std::string to_json() const override { return "{\"type\":\"cast_expression\",\"type\":\"" + json_escape(m_type) + "\",\"expression\":" + m_expression->to_json() + "}"; }

    protected:
        std::string m_type;
        std::shared_ptr<Expression> m_expression;
    };

    class NullExpression : public Expression
    {
    public:
        NullExpression(NodeType type = NodeType::NullExpression) : Expression(type) {}
        NullExpression(const std::string &type) : Expression(NodeType::NullExpression), m_type(type) {}
        virtual ~NullExpression() {}

        const std::string &type() const { return m_type; }
        std::string &type() { return m_type; }

        std::string to_string() const override { return "NullExpression(" + m_type + ")"; }
        std::string to_json() const override { return "{\"type\":\"null_expression\",\"type\":\"" + json_escape(m_type) + "\"}"; }

    protected:
        std::string m_type;
    };

    class CallExpression : public Expression
    {
    public:
        CallExpression(NodeType type = NodeType::CallExpression) : Expression(type) {}
        CallExpression(const std::string &name, std::vector<std::shared_ptr<Expression>> arguments) : Expression(NodeType::CallExpression), m_name(name), m_arguments(arguments) {}
        virtual ~CallExpression() {}

        const std::string &name() const { return m_name; }
        std::string &name() { return m_name; }

        const std::vector<std::shared_ptr<Expression>> &arguments() const { return m_arguments; }
        std::vector<std::shared_ptr<Expression>> &arguments() { return m_arguments; }

        std::string to_string() const override;
        std::string to_json() const override;

    protected:
        std::string m_name;
        std::vector<std::shared_ptr<Expression>> m_arguments;
    };

    class LiteralExpression : public Expression
    {
    public:
        LiteralExpression(NodeType type = NodeType::LiteralExpression) : Expression(type) {}
        LiteralExpression(const std::string &value, NodeType type = NodeType::LiteralExpression) : Expression(type), m_value(value) {}
        virtual ~LiteralExpression() {}

        const std::string &value() const { return m_value; }
        std::string &value() { return m_value; }

        std::string to_string() const override { return "LiteralExpression(" + m_value + ")"; }
        std::string to_json() const override { return "{\"type\":\"literal_expression\",\"value\":\"" + json_escape(m_value) + "\"}"; }

    protected:
        std::string m_value;
    };

    class StringLiteralExpression : public LiteralExpression
    {
    public:
        StringLiteralExpression(NodeType type = NodeType::StringLiteralExpression) : LiteralExpression(type) {}
        StringLiteralExpression(const std::string &value) : LiteralExpression(value, NodeType::StringLiteralExpression) {}
        virtual ~StringLiteralExpression() {}

        std::string to_string() const override { return "StringLiteralExpression(" + m_value + ")"; }
        std::string to_json() const override { return "{\"type\":\"string_literal_expression\",\"value\":\"" + json_escape(m_value) + "\"}"; }
    };

    class CharLiteralExpression : public LiteralExpression
    {
    public:
        CharLiteralExpression(NodeType type = NodeType::CharLiteralExpression) : LiteralExpression(type) {}
        CharLiteralExpression(const std::string &value) : LiteralExpression(value, NodeType::CharLiteralExpression) {}
        virtual ~CharLiteralExpression() {}

        std::string to_string() const override { return "CharLiteralExpression(" + m_value + ")"; }
        std::string to_json() const override { return "{\"type\":\"char_literal_expression\",\"value\":\"" + json_escape(m_value) + "\"}"; }
    };

    class IntegerLiteralExpression : public LiteralExpression
    {
    public:
        IntegerLiteralExpression(NodeType type = NodeType::IntegerLiteralExpression) : LiteralExpression(type) {}
        IntegerLiteralExpression(const std::string &value) : LiteralExpression(value, NodeType::IntegerLiteralExpression) {}
        virtual ~IntegerLiteralExpression() {}

        std::string to_string() const override { return "IntegerLiteralExpression(" + m_value + ")"; }
        std::string to_json() const override { return "{\"type\":\"integer_literal_expression\",\"value\":\"" + json_escape(m_value) + "\"}"; }
    };

    class FloatingPointLiteralExpression : public LiteralExpression
    {
    public:
        FloatingPointLiteralExpression(NodeType type = NodeType::FloatingPointLiteralExpression) : LiteralExpression(type) {}
        FloatingPointLiteralExpression(const std::string &value) : LiteralExpression(value, NodeType::FloatingPointLiteralExpression) {}
        virtual ~FloatingPointLiteralExpression() {}

        std::string to_string() const override { return "FloatingPointLiteralExpression(" + m_value + ")"; }
        std::string to_json() const override { return "{\"type\":\"float_literal_expression\",\"value\":\"" + json_escape(m_value) + "\"}"; }
    };

    class BooleanLiteralExpression : public LiteralExpression
    {
    public:
        BooleanLiteralExpression(NodeType type = NodeType::BooleanLiteralExpression) : LiteralExpression(type) {}
        BooleanLiteralExpression(const std::string &value) : LiteralExpression(value, NodeType::BooleanLiteralExpression) {}
        virtual ~BooleanLiteralExpression() {}

        std::string to_string() const override { return "BooleanLiteralExpression(" + m_value + ")"; }
        std::string to_json() const override { return "{\"type\":\"boolean_literal_expression\",\"value\":\"" + json_escape(m_value) + "\"}"; }
    };

    ///=================================================================================================
    /// Statements types
    ///=================================================================================================

    class ReturnStatement : public Statement
    {
    public:
        ReturnStatement(NodeType type = NodeType::ReturnStatement) : Statement(type) {}
        ReturnStatement(std::shared_ptr<Expression> expression) : Statement(NodeType::ReturnStatement), m_expression(expression) {}
        virtual ~ReturnStatement() {}

        const std::shared_ptr<Expression> &expression() const { return m_expression; }
        std::shared_ptr<Expression> &expression() { return m_expression; }

        std::string to_string() const override { return "ReturnStatement()"; }
        std::string to_json() const override { return "{\"type\":\"return_statement\"}"; }

    protected:
        std::shared_ptr<Expression> m_expression;
    };
}

namespace std
{
    std::string to_string(const std::shared_ptr<jcc::GenericNode> value);
}

#include "node.hpp"

namespace jcc
{
    typedef GenericNode ASTNode;

    class AbstractSyntaxTree
    {
    public:
        AbstractSyntaxTree() {}
        AbstractSyntaxTree(const std::shared_ptr<ASTNode> &root) : m_root(root) {}
        virtual ~AbstractSyntaxTree() {}

        const std::shared_ptr<ASTNode> &root() const { return m_root; }
        std::shared_ptr<ASTNode> &root() { return m_root; }

        std::string to_string() const { return m_root->to_string(); }
        std::string to_json() const { return m_root->to_json(); }

    protected:
        std::shared_ptr<ASTNode> m_root;
    };

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
};

#endif // _JCC_PARSER_HPP_
