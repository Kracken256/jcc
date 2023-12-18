#include "parser.hpp"
#include <stack>
#include <string>
#include <vector>
#include <iostream>
#include <memory>

std::string std::to_string(const std::shared_ptr<jcc::AST::GenericNode> value)
{
    if (value == nullptr)
        return "{}";

    return value->to_json();
}

/// TODO: Implement this
jcc::AbstractSyntaxTree jcc::Parser::parse(const jcc::TokenList &tokens, std::vector<std::shared_ptr<jcc::CompilerMessage>> &messages)
{
    (void)messages;
    (void)tokens;
    using namespace AST;

    AbstractSyntaxTree ast;

    auto item = std::make_shared<ClassDeclaration>();

    item->name() = "AuthClient";
    item->members() = {
        std::make_shared<ClassMemberDeclaration>("user_status", "jcc::boolean", "false", VisiblityModifier::Public),
        std::make_shared<ClassMemberDeclaration>("user_id", "jcc::i64", "0", VisiblityModifier::Public),
        std::make_shared<ClassMemberDeclaration>("user_name", "jcc::string", "\"\"", VisiblityModifier::Public),
        std::make_shared<ClassMemberDeclaration>("user_email", "jcc::string", "\"\"", VisiblityModifier::Public),
    };
    item->methods() = {
        std::make_shared<ClassMethodDeclaration>("hi", "int", std::vector<std::shared_ptr<FunctionParameter>>{
                                                              },
                                                 VisiblityModifier::Protected),
    };

    ast = std::static_pointer_cast<GenericNode>(item);

    return ast;
}