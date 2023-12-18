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
    namespace AST
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

            ///===================
            /// Declarations
            ///===================

            TypeDeclaration,
            StructField,
            StructDeclaration,
            UnionDeclaration,
            UnionField,
            EnumItem,
            EnumDeclaration,
            FunctionParameter,
            FunctionDeclaration,
            ClassMemberDeclaration,
            ClassMethodDeclaration,
            ClassDeclaration,
            ExternalDeclaration,

            ///===================
            /// Expressions
            ///===================
            BinaryExpression,
            UnaryExpression,
            CastExpression,
            CallExpression,
            LambdaExpression,

            ///===================
            /// Statements
            ///===================

            ///===================
            /// Definitions
            ///===================
            BlockDefinition,
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

            virtual std::string generate() const { return "/* GenericNode */"; }

        protected:
            NodeType m_type;

            std::string json_escape(const std::string &str) const
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
        };

        class Expression : public GenericNode
        {
        public:
            Expression(NodeType type = NodeType::Expression) : GenericNode(type) {}
            virtual ~Expression() {}

            std::string to_string() const override { return "Expression()"; }
            std::string to_json() const override { return "{\"type\":\"expression\"}"; }

            std::string generate() const override { return "/* Expression */"; }
        };

        class Statement : public GenericNode
        {
        public:
            Statement(NodeType type = NodeType::Statement) : GenericNode(type) {}
            virtual ~Statement() {}

            std::string to_string() const override { return "Statement()"; }
            std::string to_json() const override { return "{\"type\":\"statement\"}"; }

            std::string generate() const override { return "/* Statement */"; }
        };

        class Declaration : public GenericNode
        {
        public:
            Declaration(NodeType type = NodeType::Declaration) : GenericNode(type) {}
            virtual ~Declaration() {}

            std::string to_string() const override { return "Declaration()"; }
            std::string to_json() const override { return "{\"type\":\"declaration\"}"; }

            std::string generate() const override { return "/* Declaration */"; }
        };

        class Definition : public GenericNode
        {
        public:
            Definition(NodeType type = NodeType::Definition) : GenericNode(type) {}
            virtual ~Definition() {}

            std::string to_string() const override { return "Definition()"; }
            std::string to_json() const override { return "{\"type\":\"definition\"}"; }

            std::string generate() const override { return "/* Definition */"; }
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

            std::string generate() const override { return "typedef " + m_typename + " " + m_alias + ";"; }

        protected:
            std::string m_alias;
            std::string m_typename;
        };

        class StructField : public Declaration
        {
        public:
            StructField(NodeType type = NodeType::StructField) : Declaration(type) {}
            StructField(const std::string &name, const std::string &type) : Declaration(NodeType::StructField), m_name(name), m_type(type) {}
            virtual ~StructField() {}

            const std::string &name() const { return m_name; }
            std::string &name() { return m_name; }

            const std::string &type() const { return m_type; }
            std::string &type() { return m_type; }

            std::string to_string() const override { return "StructField(" + m_name + ", " + m_type + ")"; }
            std::string to_json() const override { return "{\"type\":\"struct_field\",\"name\":\"" + json_escape(m_name) + "\",\"type\":\"" + json_escape(m_type) + "\"}"; }

            std::string generate() const override { return m_type + " " + m_name + ";"; }

        protected:
            std::string m_name;
            std::string m_type;
        };

        class StructDeclaration : public TypeDeclaration
        {
        public:
            StructDeclaration(NodeType type = NodeType::StructDeclaration) : TypeDeclaration(type) {}
            StructDeclaration(const std::string &name, std::vector<std::shared_ptr<StructField>> members) : TypeDeclaration(name, "", NodeType::StructDeclaration), m_members(members) {}
            virtual ~StructDeclaration() {}

            const std::string &name() const { return m_name; }
            std::string &name() { return m_name; }

            const std::vector<std::shared_ptr<StructField>> &members() const { return m_members; }
            std::vector<std::shared_ptr<StructField>> &members() { return m_members; }

            std::string to_string() const override
            {
                std::string str = "StructDeclaration(" + m_name + ", [";
                for (auto &member : m_members)
                {
                    str += member->to_string() + ", ";
                }

                if (m_members.size() > 0)
                {
                    str.pop_back();
                    str.pop_back();
                }

                str += "])";
                return str;
            }

            std::string to_json() const override
            {
                std::string str = "{\"type\":\"struct_declaration\",\"name\":\"" + json_escape(m_name) + "\",\"members\":[";
                for (auto &member : m_members)
                {
                    str += member->to_json() + ",";
                }
                if (m_members.size() > 0)
                {
                    str.pop_back();
                }
                str += "]}";
                return str;
            }

            std::string generate() const override
            {
                std::string str = "typedef struct " + m_name + " {\n";
                for (auto &member : m_members)
                {
                    str += member->generate() + "\n";
                }
                str += "} " + m_name + ";";
                return str;
            }

        protected:
            std::string m_name;
            std::vector<std::shared_ptr<StructField>> m_members;
        };

        class UnionField : public Declaration
        {
        public:
            UnionField(NodeType type = NodeType::UnionField) : Declaration(type) {}
            UnionField(const std::string &name, const std::string &type) : Declaration(NodeType::UnionField), m_name(name), m_type(type) {}
            virtual ~UnionField() {}

            const std::string &name() const { return m_name; }
            std::string &name() { return m_name; }

            const std::string &type() const { return m_type; }
            std::string &type() { return m_type; }

            std::string to_string() const override { return "UnionField(" + m_name + ", " + m_type + ")"; }
            std::string to_json() const override { return "{\"type\":\"union_field\",\"name\":\"" + json_escape(m_name) + "\",\"type\":\"" + json_escape(m_type) + "\"}"; }

            std::string generate() const override { return m_type + " " + m_name + ";"; }

        protected:
            std::string m_name;
            std::string m_type;
        };

        class UnionDeclaration : public TypeDeclaration
        {
        public:
            UnionDeclaration(NodeType type = NodeType::UnionDeclaration) : TypeDeclaration(type) {}
            UnionDeclaration(const std::string &name, std::vector<std::shared_ptr<UnionField>> members) : TypeDeclaration(name, "", NodeType::UnionDeclaration), m_members(members) {}
            virtual ~UnionDeclaration() {}

            const std::string &name() const { return m_name; }
            std::string &name() { return m_name; }

            const std::vector<std::shared_ptr<UnionField>> &members() const { return m_members; }
            std::vector<std::shared_ptr<UnionField>> &members() { return m_members; }

            std::string to_string() const override
            {
                std::string str = "UnionDeclaration(" + m_name + ", [";
                for (auto &member : m_members)
                {
                    str += member->to_string() + ", ";
                }

                if (m_members.size() > 0)
                {
                    str.pop_back();
                    str.pop_back();
                }

                str += "])";
                return str;
            }

            std::string to_json() const override
            {
                std::string str = "{\"type\":\"union_declaration\",\"name\":\"" + json_escape(m_name) + "\",\"members\":[";
                for (auto &member : m_members)
                {
                    str += member->to_json() + ",";
                }
                if (m_members.size() > 0)
                {
                    str.pop_back();
                }
                str += "]}";
                return str;
            }

            std::string generate() const override
            {
                std::string str = "typedef union " + m_name + " {\n";
                for (auto &member : m_members)
                {
                    str += member->generate() + "\n";
                }
                str += "} " + m_name + ";";
                return str;
            }

        protected:
            std::string m_name;
            std::vector<std::shared_ptr<UnionField>> m_members;
        };

        class EnumItem : public Declaration
        {
        public:
            EnumItem(NodeType type = NodeType::EnumItem) : Declaration(type) {}
            EnumItem(const std::string &name, const std::string &value) : Declaration(NodeType::EnumItem), m_name(name), m_value(value) {}
            virtual ~EnumItem() {}

            const std::string &name() const { return m_name; }
            std::string &name() { return m_name; }

            const std::string &value() const { return m_value; }
            std::string &value() { return m_value; }

            std::string to_string() const override { return "EnumItem(" + m_name + ", " + m_value + ")"; }
            std::string to_json() const override { return "{\"type\":\"enum_item\",\"name\":\"" + json_escape(m_name) + "\",\"value\":\"" + json_escape(m_value) + "\"}"; }

            std::string generate() const override
            {
                if (m_value.empty())
                    return m_name + ",";
                else
                    return m_name + " = " + m_value + ",";
            }

        protected:
            std::string m_name;
            std::string m_value;
        };

        class EnumDeclaration : public TypeDeclaration
        {
        public:
            EnumDeclaration(NodeType type = NodeType::EnumDeclaration) : TypeDeclaration(type) {}
            EnumDeclaration(const std::string &name, std::vector<std::shared_ptr<EnumItem>> members) : TypeDeclaration(name, "", NodeType::EnumDeclaration), m_members(members) {}
            virtual ~EnumDeclaration() {}

            const std::string &name() const { return m_name; }
            std::string &name() { return m_name; }

            const std::vector<std::shared_ptr<EnumItem>> &members() const { return m_members; }
            std::vector<std::shared_ptr<EnumItem>> &members() { return m_members; }

            std::string to_string() const override
            {
                std::string str = "EnumDeclaration(" + m_name + ", [";
                for (auto &member : m_members)
                {
                    str += member->to_string() + ", ";
                }

                if (m_members.size() > 0)
                {
                    str.pop_back();
                    str.pop_back();
                }

                str += "])";
                return str;
            }

            std::string to_json() const override
            {
                std::string str = "{\"type\":\"enum_declaration\",\"name\":\"" + json_escape(m_name) + "\",\"members\":[";
                for (auto &member : m_members)
                {
                    str += member->to_json() + ",";
                }
                if (m_members.size() > 0)
                {
                    str.pop_back();
                }
                str += "]}";
                return str;
            }

            std::string generate() const override
            {
                std::string str = "typedef enum " + m_name + " {\n";
                for (auto &member : m_members)
                {
                    str += member->generate() + "\n";
                }
                str += "} " + m_name + ";";
                return str;
            }

        protected:
            std::string m_name;
            std::vector<std::shared_ptr<EnumItem>> m_members;
        };

        class FunctionParameter : public Declaration
        {
        public:
            FunctionParameter(NodeType type = NodeType::FunctionParameter) : Declaration(type) {}
            FunctionParameter(const std::string &name, const std::string &type, const std::string &default_value) : Declaration(NodeType::FunctionParameter), m_name(name), m_type(type), m_default_value(default_value) {}

            const std::string &name() const { return m_name; }
            std::string &name() { return m_name; }

            const std::string &type() const { return m_type; }
            std::string &type() { return m_type; }

            const std::string &default_value() const { return m_default_value; }
            std::string &default_value() { return m_default_value; }

            std::string to_string() const override { return "FunctionParameter(" + m_name + ", " + m_type + ", " + m_default_value + ")"; }
            std::string to_json() const override { return "{\"type\":\"function_parameter\",\"name\":\"" + json_escape(m_name) + "\",\"type\":\"" + json_escape(m_type) + "\",\"default_value\":\"" + json_escape(m_default_value) + "\"}"; }

            std::string generate() const override
            {
                if (m_default_value.empty())
                    return m_type + " " + m_name;
                else
                    return m_type + " " + m_name + " = " + m_default_value;
            }

        protected:
            std::string m_name;
            std::string m_type;
            std::string m_default_value;
        };

        class FunctionDeclaration : public Declaration
        {
        public:
            FunctionDeclaration(NodeType type = NodeType::FunctionDeclaration) : Declaration(type) {}
            FunctionDeclaration(const std::string &name, const std::string &return_type, std::vector<std::shared_ptr<FunctionParameter>> parameters) : Declaration(NodeType::FunctionDeclaration), m_return_type(return_type), m_parameters(parameters), m_name(name) {}
            virtual ~FunctionDeclaration() {}

            const std::string &return_type() const { return m_return_type; }
            std::string &return_type() { return m_return_type; }

            const std::vector<std::shared_ptr<FunctionParameter>> &parameters() const { return m_parameters; }
            std::vector<std::shared_ptr<FunctionParameter>> &parameters() { return m_parameters; }

            const std::string &name() const { return m_name; }
            std::string &name() { return m_name; }

            std::string to_string() const override
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

            std::string to_json() const override
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

            std::string generate() const override
            {
                std::string str = m_return_type + " " + m_name + "(";
                for (auto &parameter : m_parameters)
                {
                    str += parameter->generate() + ", ";
                }
                if (m_parameters.size() > 0)
                {
                    str.pop_back();
                    str.pop_back();
                }
                str += ");";
                return str;
            }

        protected:
            std::string m_return_type;
            std::vector<std::shared_ptr<FunctionParameter>> m_parameters;
            std::string m_name;
        };

        class ClassMemberDeclaration : public Declaration
        {

        public:
            ClassMemberDeclaration(NodeType type = NodeType::ClassMemberDeclaration) : Declaration(type) {}
            ClassMemberDeclaration(const std::string &name, const std::string &type, const std::string &default_value, VisiblityModifier visiblity_modifier) : Declaration(NodeType::ClassMemberDeclaration), m_name(name), m_type(type), m_default_value(default_value), m_visiblity_modifier(visiblity_modifier) {}

            const std::string &name() const { return m_name; }
            std::string &name() { return m_name; }

            const std::string &type() const { return m_type; }
            std::string &type() { return m_type; }

            const std::string &default_value() const { return m_default_value; }
            std::string &default_value() { return m_default_value; }

            VisiblityModifier visiblity_modifier() const { return m_visiblity_modifier; }
            VisiblityModifier &visiblity_modifier() { return m_visiblity_modifier; }

            std::string to_string() const override { return "ClassMemberDeclaration(" + m_name + ", " + m_type + ", " + m_default_value + ")"; }
            std::string to_json() const override { return "{\"type\":\"class_member_declaration\",\"name\":\"" + json_escape(m_name) + "\",\"type\":\"" + json_escape(m_type) + "\",\"default_value\":\"" + json_escape(m_default_value) + "\",\"visiblity_modifier\":\"" + std::to_string(static_cast<int>(m_visiblity_modifier)) + "\"}"; }

            std::string generate() const override
            {

                if (m_default_value.empty())
                    return visiblity_modifier_to_string.at(m_visiblity_modifier) + m_type + " " + m_name + ";";
                else
                    return visiblity_modifier_to_string.at(m_visiblity_modifier) + m_type + " " + m_name + " = " + m_default_value + ";";
            }

        protected:
            std::string m_name;
            std::string m_type;
            std::string m_default_value;
            VisiblityModifier m_visiblity_modifier;

            std::map<VisiblityModifier, std::string> visiblity_modifier_to_string = {
                {VisiblityModifier::Public, "public: "},
                {VisiblityModifier::Protected, "protected: "},
                {VisiblityModifier::Private, "private: "},
            };
        };

        class ClassMethodDeclaration : public FunctionDeclaration
        {
        public:
            ClassMethodDeclaration(NodeType type = NodeType::ClassMethodDeclaration) : FunctionDeclaration(type) {}
            ClassMethodDeclaration(const std::string &name, const std::string &return_type, std::vector<std::shared_ptr<FunctionParameter>> parameters, VisiblityModifier visiblity_modifier) : FunctionDeclaration(name, return_type, parameters), m_visiblity_modifier(visiblity_modifier) {} 
            virtual ~ClassMethodDeclaration() {}

            VisiblityModifier visiblity_modifier() const { return m_visiblity_modifier; }
            VisiblityModifier &visiblity_modifier() { return m_visiblity_modifier; }

            std::string to_string() const override
            {
                std::string str = "ClassMethodDeclaration(" + m_name + ", " + m_return_type + ", [";
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

            std::string to_json() const override
            {
                std::string str = "{\"type\":\"class_method_declaration\",\"name\":\"" + json_escape(m_name) + "\",\"return_type\":\"" + json_escape(m_return_type) + "\",\"parameters\":[";
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

            std::string generate() const override
            {
                std::string str = visiblity_modifier_to_string.at(m_visiblity_modifier) + m_return_type + " " + m_name + "(";
                for (auto &parameter : m_parameters)
                {
                    str += parameter->generate() + ", ";
                }
                if (m_parameters.size() > 0)
                {
                    str.pop_back();
                    str.pop_back();
                }
                str += ");";
                return str;
            }

        protected:
            VisiblityModifier m_visiblity_modifier;

            std::map<VisiblityModifier, std::string> visiblity_modifier_to_string = {
                {VisiblityModifier::Public, "public: "},
                {VisiblityModifier::Protected, "protected: "},
                {VisiblityModifier::Private, "private: "},
            };
        };

        class ClassDeclaration : public TypeDeclaration
        {
        public:
            ClassDeclaration(NodeType type = NodeType::ClassDeclaration) : TypeDeclaration(type) {}
            ClassDeclaration(const std::string &name, std::vector<std::shared_ptr<ClassMemberDeclaration>> members, std::vector<std::shared_ptr<ClassMethodDeclaration>> methods) : TypeDeclaration(name, "", NodeType::ClassDeclaration), m_name(name), m_members(members), m_methods(methods) {}
            virtual ~ClassDeclaration() {}

            const std::string &name() const { return m_name; }
            std::string &name() { return m_name; }

            const std::vector<std::shared_ptr<ClassMemberDeclaration>> &members() const { return m_members; }
            std::vector<std::shared_ptr<ClassMemberDeclaration>> &members() { return m_members; }

            const std::vector<std::shared_ptr<ClassMethodDeclaration>> &methods() const { return m_methods; }
            std::vector<std::shared_ptr<ClassMethodDeclaration>> &methods() { return m_methods; }

            std::string to_string() const override
            {
                std::string str = "ClassDeclaration(" + m_name + ", [";
                for (auto &member : m_members)
                {
                    str += member->to_string() + ", ";
                }

                if (m_members.size() > 0)
                {
                    str.pop_back();
                    str.pop_back();
                }

                str += "])";
                return str;
            }

            std::string to_json()
            {
                std::map<VisiblityModifier, std::vector<std::shared_ptr<Declaration>>> member_bins = {
                    {VisiblityModifier::Public, {}},
                    {VisiblityModifier::Protected, {}},
                    {VisiblityModifier::Private, {}},
                };

                std::map<VisiblityModifier, std::vector<std::shared_ptr<Declaration>>> method_bins = {
                    {VisiblityModifier::Public, {}},
                    {VisiblityModifier::Protected, {}},
                    {VisiblityModifier::Private, {}},
                };

                for (const auto &member : m_members)
                {
                    member_bins[member->visiblity_modifier()].push_back(member);
                }

                for (const auto &method : m_methods)
                {
                    method_bins[method->visiblity_modifier()].push_back(method);
                }

                std::string str = "{\"type\":\"class_declaration\",\"name\":\"" + json_escape(m_name) + "\",\"members\":{";

                str += "\"public\":[";

                for (const auto &pubmem : member_bins[VisiblityModifier::Public])
                {
                    str += pubmem->to_json() + ",";
                }

                if (member_bins[VisiblityModifier::Public].size() > 0)
                {
                    str.pop_back();
                }

                str += "],\"protected\":[";
                for (const auto &protmem : member_bins[VisiblityModifier::Protected])
                {
                    str += protmem->to_json() + ",";
                }

                if (member_bins[VisiblityModifier::Protected].size() > 0)
                {
                    str.pop_back();
                }

                str += "],\"private\":[";
                for (const auto &privmem : member_bins[VisiblityModifier::Private])
                {
                    str += privmem->to_json() + ",";
                }

                if (member_bins[VisiblityModifier::Private].size() > 0)
                {
                    str.pop_back();
                }

                str += "],\"methods\":{";

                str += "\"public\":[";
                for (const auto &pubmeth : method_bins[VisiblityModifier::Public])
                {
                    str += pubmeth->to_json() + ",";
                }

                if (method_bins[VisiblityModifier::Public].size() > 0)
                {
                    str.pop_back();
                }

                str += "],\"protected\":[";
                for (const auto &protmeth : method_bins[VisiblityModifier::Protected])
                {
                    str += protmeth->to_json() + ",";
                }

                if (method_bins[VisiblityModifier::Protected].size() > 0)
                {
                    str.pop_back();
                }

                str += "],\"private\":[";
                for (const auto &privmeth : method_bins[VisiblityModifier::Private])
                {
                    str += privmeth->to_json() + ",";
                }

                if (method_bins[VisiblityModifier::Private].size() > 0)
                {
                    str.pop_back();
                }

                str += "]}}";

                return str;
            }

            std::string generate() const override
            {
                std::string str = "class " + m_name + " {\n";
                for (auto &member : m_members)
                {
                    str += member->generate() + "\n";
                }

                for (auto &method : m_methods)
                {
                    str += method->generate() + "\n";
                }

                str += "};";
                return str;
            }

        protected:
            std::string m_name;
            std::vector<std::shared_ptr<ClassMemberDeclaration>> m_members;
            std::vector<std::shared_ptr<ClassMethodDeclaration>> m_methods;
        };
    };
}

namespace std
{
    std::string to_string(const std::shared_ptr<jcc::AST::GenericNode> value);
}

#include "node.hpp"

namespace jcc
{
    typedef Node<std::shared_ptr<AST::GenericNode>> ASTNode;
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
