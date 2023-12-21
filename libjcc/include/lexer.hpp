#ifndef _JCC_LEXER_HPP_
#define _JCC_LEXER_HPP_

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <exception>
#include <stdexcept>

namespace jcc
{
    enum class TokenType
    {
        Unknown = -1,
        Identifier,
        Keyword,
        NumberLiteral,
        FloatingPointLiteral,
        StringLiteral,
        Operator,
        Punctuator,
        MultiLineComment,
        SingleLineComment,
        Whitespace,
    };

    enum class Keyword
    {
        Namemap,
        Namespace,
        Using,
        Export,
        Global,
        Infer,
        Seal,
        Unseal,
        Class,
        Struct,
        Union,
        Typedef,
        Public,
        Private,
        Protected,
        Claim,
        Virtual,
        Abstract,
        Volatile,
        Const,
        Enum,
        Static_map,
        Explicit,
        Extern,
        Friend,
        Operator,
        This,
        Constructor,
        Destructor,
        Metaclass,
        Metatype,
        Metafunction,
        Meta,
        Static,
        Sizeof,
        If,
        Else,
        For,
        While,
        Do,
        Switch,
        Return,
        Fault,
        Case,
        Break,
        Default,
        Abort,
        Throw,
        Continue,

        Bit,
        Byte,
        Char,
        Word,
        Short,
        Dword,
        Int,
        Qword,
        Long,
        Float,
        Double,
        Nint,
        Nuint,
        Intn,
        Uintn,
        Address,
        Bigfloat,
        Bigint,
        Biguint,
        Arbint,
        Arbuint,
        Real,
        Complex,
        String,
        Buffer,
        Secbuffer,
        Secstring,
        Map,
        Tensor,
        Routine,
        Void
    };

    const std::map<const char *, jcc::Keyword> lexKeywordMap = {
        {"namemap", jcc::Keyword::Namemap},
        {"namespace", jcc::Keyword::Namespace},
        {"using", jcc::Keyword::Using},
        {"export", jcc::Keyword::Export},
        {"global", jcc::Keyword::Global},
        {"infer", jcc::Keyword::Infer},
        {"seal", jcc::Keyword::Seal},
        {"unseal", jcc::Keyword::Unseal},
        {"class", jcc::Keyword::Class},
        {"struct", jcc::Keyword::Struct},
        {"union", jcc::Keyword::Union},
        {"typedef", jcc::Keyword::Typedef},
        {"public", jcc::Keyword::Public},
        {"private", jcc::Keyword::Private},
        {"protected", jcc::Keyword::Protected},
        {"claim", jcc::Keyword::Claim},
        {"virtual", jcc::Keyword::Virtual},
        {"abstract", jcc::Keyword::Abstract},
        {"volatile", jcc::Keyword::Volatile},
        {"const", jcc::Keyword::Const},
        {"enum", jcc::Keyword::Enum},
        {"static_map", jcc::Keyword::Static_map},
        {"explicit", jcc::Keyword::Explicit},
        {"extern", jcc::Keyword::Extern},
        {"friend", jcc::Keyword::Friend},
        {"operator", jcc::Keyword::Operator},
        {"this", jcc::Keyword::This},
        {"constructor", jcc::Keyword::Constructor},
        {"destructor", jcc::Keyword::Destructor},
        {"metaclass", jcc::Keyword::Metaclass},
        {"metatype", jcc::Keyword::Metatype},
        {"metafunction", jcc::Keyword::Metafunction},
        {"meta", jcc::Keyword::Meta},
        {"static", jcc::Keyword::Static},
        {"sizeof", jcc::Keyword::Sizeof},
        {"if", jcc::Keyword::If},
        {"else", jcc::Keyword::Else},
        {"for", jcc::Keyword::For},
        {"while", jcc::Keyword::While},
        {"do", jcc::Keyword::Do},
        {"switch", jcc::Keyword::Switch},
        {"return", jcc::Keyword::Return},
        {"fault", jcc::Keyword::Fault},
        {"case", jcc::Keyword::Case},
        {"break", jcc::Keyword::Break},
        {"default", jcc::Keyword::Default},
        {"abort", jcc::Keyword::Abort},
        {"throw", jcc::Keyword::Throw},
        {"continue", jcc::Keyword::Continue},

        {"bit", jcc::Keyword::Bit},
        {"byte", jcc::Keyword::Byte},
        {"char", jcc::Keyword::Char},
        {"word", jcc::Keyword::Word},
        {"short", jcc::Keyword::Short},
        {"dword", jcc::Keyword::Dword},
        {"int", jcc::Keyword::Int},
        {"qword", jcc::Keyword::Qword},
        {"long", jcc::Keyword::Long},
        {"float", jcc::Keyword::Float},
        {"double", jcc::Keyword::Double},
        {"nint", jcc::Keyword::Nint},
        {"nuint", jcc::Keyword::Nuint},
        {"intn", jcc::Keyword::Intn},
        {"uintn", jcc::Keyword::Uintn},
        {"address", jcc::Keyword::Address},
        {"bigfloat", jcc::Keyword::Bigfloat},
        {"bigint", jcc::Keyword::Bigint},
        {"biguint", jcc::Keyword::Biguint},
        {"arbint", jcc::Keyword::Arbint},
        {"arbuint", jcc::Keyword::Arbuint},
        {"real", jcc::Keyword::Real},
        {"complex", jcc::Keyword::Complex},
        {"string", jcc::Keyword::String},
        {"buffer", jcc::Keyword::Buffer},
        {"secbuffer", jcc::Keyword::Secbuffer},
        {"secstring", jcc::Keyword::Secstring},
        {"map", jcc::Keyword::Map},
        {"tensor", jcc::Keyword::Tensor},
        {"routine", jcc::Keyword::Routine},
        {"void", jcc::Keyword::Void}};

    const std::map<jcc::Keyword, const char *> lexKeywordMapReverse = {
        {jcc::Keyword::Namemap, "namemap"},
        {jcc::Keyword::Namespace, "namespace"},
        {jcc::Keyword::Using, "using"},
        {jcc::Keyword::Export, "export"},
        {jcc::Keyword::Global, "global"},
        {jcc::Keyword::Infer, "infer"},
        {jcc::Keyword::Seal, "seal"},
        {jcc::Keyword::Unseal, "unseal"},
        {jcc::Keyword::Class, "class"},
        {jcc::Keyword::Struct, "struct"},
        {jcc::Keyword::Union, "union"},
        {jcc::Keyword::Typedef, "typedef"},
        {jcc::Keyword::Public, "public"},
        {jcc::Keyword::Private, "private"},
        {jcc::Keyword::Protected, "protected"},
        {jcc::Keyword::Claim, "claim"},
        {jcc::Keyword::Virtual, "virtual"},
        {jcc::Keyword::Abstract, "abstract"},
        {jcc::Keyword::Volatile, "volatile"},
        {jcc::Keyword::Const, "const"},
        {jcc::Keyword::Enum, "enum"},
        {jcc::Keyword::Static_map, "static_map"},
        {jcc::Keyword::Explicit, "explicit"},
        {jcc::Keyword::Extern, "extern"},
        {jcc::Keyword::Friend, "friend"},
        {jcc::Keyword::Operator, "operator"},
        {jcc::Keyword::This, "this"},
        {jcc::Keyword::Constructor, "constructor"},
        {jcc::Keyword::Destructor, "destructor"},
        {jcc::Keyword::Metaclass, "metaclass"},
        {jcc::Keyword::Metatype, "metatype"},
        {jcc::Keyword::Metafunction, "metafunction"},
        {jcc::Keyword::Meta, "meta"},
        {jcc::Keyword::Static, "static"},
        {jcc::Keyword::Sizeof, "sizeof"},
        {jcc::Keyword::If, "if"},
        {jcc::Keyword::Else, "else"},
        {jcc::Keyword::For, "for"},
        {jcc::Keyword::While, "while"},
        {jcc::Keyword::Do, "do"},
        {jcc::Keyword::Switch, "switch"},
        {jcc::Keyword::Return, "return"},
        {jcc::Keyword::Fault, "fault"},
        {jcc::Keyword::Case, "case"},
        {jcc::Keyword::Break, "break"},
        {jcc::Keyword::Default, "default"},
        {jcc::Keyword::Abort, "abort"},
        {jcc::Keyword::Throw, "throw"},
        {jcc::Keyword::Continue, "continue"},
        
        {jcc::Keyword::Bit, "bit"},
        {jcc::Keyword::Byte, "byte"},
        {jcc::Keyword::Char, "char"},
        {jcc::Keyword::Word, "word"},
        {jcc::Keyword::Short, "short"},
        {jcc::Keyword::Dword, "dword"},
        {jcc::Keyword::Int, "int"},
        {jcc::Keyword::Qword, "qword"},
        {jcc::Keyword::Long, "long"},
        {jcc::Keyword::Float, "float"},
        {jcc::Keyword::Double, "double"},
        {jcc::Keyword::Nint, "nint"},
        {jcc::Keyword::Nuint, "nuint"},
        {jcc::Keyword::Intn, "intn"},
        {jcc::Keyword::Uintn, "uintn"},
        {jcc::Keyword::Address, "address"},
        {jcc::Keyword::Bigfloat, "bigfloat"},
        {jcc::Keyword::Bigint, "bigint"},
        {jcc::Keyword::Biguint, "biguint"},
        {jcc::Keyword::Arbint, "arbint"},
        {jcc::Keyword::Arbuint, "arbuint"},
        {jcc::Keyword::Real, "real"},
        {jcc::Keyword::Complex, "complex"},
        {jcc::Keyword::String, "string"},
        {jcc::Keyword::Buffer, "buffer"},
        {jcc::Keyword::Secbuffer, "secbuffer"},
        {jcc::Keyword::Secstring, "secstring"},
        {jcc::Keyword::Map, "map"},
        {jcc::Keyword::Tensor, "tensor"},
        {jcc::Keyword::Routine, "routine"},
        {jcc::Keyword::Void, "void"}};

    enum class Punctuator
    {
        OpenParen,
        CloseParen,
        OpenBrace,
        CloseBrace,
        OpenBracket,
        CloseBracket,
        Semicolon,
        Colon,
        Comma,
        Period,
        ScopeResolution,
    };

    const std::map<const char *, jcc::Punctuator> lexPunctuatorMap = {
        {"(", jcc::Punctuator::OpenParen},
        {")", jcc::Punctuator::CloseParen},
        {"{", jcc::Punctuator::OpenBrace},
        {"}", jcc::Punctuator::CloseBrace},
        {"[", jcc::Punctuator::OpenBracket},
        {"]", jcc::Punctuator::CloseBracket},
        {";", jcc::Punctuator::Semicolon},
        {":", jcc::Punctuator::Colon},
        {",", jcc::Punctuator::Comma},
        {".", jcc::Punctuator::Period},
        {"::", jcc::Punctuator::ScopeResolution}};

    const std::map<jcc::Punctuator, const char *> lexPunctuatorMapReverse = {
        {jcc::Punctuator::OpenParen, "("},
        {jcc::Punctuator::CloseParen, ")"},
        {jcc::Punctuator::OpenBrace, "{"},
        {jcc::Punctuator::CloseBrace, "}"},
        {jcc::Punctuator::OpenBracket, "["},
        {jcc::Punctuator::CloseBracket, "]"},
        {jcc::Punctuator::Semicolon, ";"},
        {jcc::Punctuator::Colon, ":"},
        {jcc::Punctuator::Comma, ","},
        {jcc::Punctuator::Period, "."},
        {jcc::Punctuator::ScopeResolution, "::"}};

    enum class Operator
    {
        PlusEquals,
        MinusEquals,
        TimesEquals,
        FloatingDivideEquals,
        ModulusEquals,

        XorEquals,
        OrEquals,
        AndEquals,
        LeftShiftEquals,
        ArithmeticRightShiftEquals,
        UnsignedRightShiftEquals,
        BitwiseOrEquals,
        BitwiseAndEquals,
        BitwiseXorEquals,

        LeftShift,
        RightShift,
        ArithmeticRightShift,
        Equals,
        NotEquals,
        And,
        Or,
        Xor,
        LessThanOrEqual,
        GreaterThanOrEqual,
        LessThan,
        GreaterThan,

        Assign,
        NullCoalesce,
        At,

        FloorDivide,
        Increment,
        Decrement,
        Plus,
        Minus,
        Times,
        FloatingDivide,
        Modulus,
        BitwiseAnd,
        BitwiseOr,
        BitwiseXor,
        BitwiseNot,
        Not,

        Ternary,
        Preprocessor,
        MemberAccess,
        Comma,
        New,
        Delete,
    };

    const std::map<const char *, jcc::Operator> lexOperatorMap = {
        {"+=", jcc::Operator::PlusEquals},
        {"-=", jcc::Operator::MinusEquals},
        {"*=", jcc::Operator::TimesEquals},
        {"/=", jcc::Operator::FloatingDivideEquals},
        {"%=", jcc::Operator::ModulusEquals},

        {"^^=", jcc::Operator::XorEquals},
        {"||=", jcc::Operator::OrEquals},
        {"&&=", jcc::Operator::AndEquals},
        {"<<=", jcc::Operator::LeftShiftEquals},
        {">>=", jcc::Operator::ArithmeticRightShiftEquals},
        {">>>=", jcc::Operator::UnsignedRightShiftEquals},
        {"|=", jcc::Operator::BitwiseOrEquals},
        {"&=", jcc::Operator::BitwiseAndEquals},
        {"^=", jcc::Operator::BitwiseXorEquals},

        {"<<", jcc::Operator::LeftShift},
        {">>", jcc::Operator::RightShift},
        {"==", jcc::Operator::Equals},
        {"!=", jcc::Operator::NotEquals},
        {"&&", jcc::Operator::And},
        {"||", jcc::Operator::Or},
        {"^^", jcc::Operator::Xor},
        {"<=", jcc::Operator::LessThanOrEqual},
        {">=", jcc::Operator::GreaterThanOrEqual},
        {"<", jcc::Operator::LessThan},
        {">", jcc::Operator::GreaterThan},

        {"=", jcc::Operator::Assign},
        {"??", jcc::Operator::NullCoalesce},
        {"@", jcc::Operator::At},

        {"//", jcc::Operator::FloorDivide},
        {"++", jcc::Operator::Increment},
        {"--", jcc::Operator::Decrement},
        {"+", jcc::Operator::Plus},
        {"-", jcc::Operator::Minus},
        {"*", jcc::Operator::Times},
        {"/", jcc::Operator::FloatingDivide},
        {"%", jcc::Operator::Modulus},
        {"&", jcc::Operator::BitwiseAnd},
        {"|", jcc::Operator::BitwiseOr},
        {"^", jcc::Operator::BitwiseXor},
        {"~", jcc::Operator::BitwiseNot},
        {"!", jcc::Operator::Not},

        {"?", jcc::Operator::Ternary},
        {"#", jcc::Operator::Preprocessor},
        {".", jcc::Operator::MemberAccess},
        {",", jcc::Operator::Comma},
        {"new", jcc::Operator::New},
        {"delete", jcc::Operator::Delete}};

    const std::map<jcc::Operator, const char *> lexOperatorMapReverse = {
        {jcc::Operator::PlusEquals, "+="},
        {jcc::Operator::MinusEquals, "-="},
        {jcc::Operator::TimesEquals, "*="},
        {jcc::Operator::FloatingDivideEquals, "/="},
        {jcc::Operator::ModulusEquals, "%="},

        {jcc::Operator::XorEquals, "^^="},
        {jcc::Operator::OrEquals, "||="},
        {jcc::Operator::AndEquals, "&&="},
        {jcc::Operator::LeftShiftEquals, "<<="},
        {jcc::Operator::ArithmeticRightShiftEquals, ">>="},
        {jcc::Operator::UnsignedRightShiftEquals, ">>>="},
        {jcc::Operator::BitwiseOrEquals, "|="},
        {jcc::Operator::BitwiseAndEquals, "&="},
        {jcc::Operator::BitwiseXorEquals, "^="},

        {jcc::Operator::LeftShift, "<<"},
        {jcc::Operator::RightShift, ">>"},
        {jcc::Operator::ArithmeticRightShift, ">>"},
        {jcc::Operator::Equals, "=="},
        {jcc::Operator::NotEquals, "!="},
        {jcc::Operator::And, "&&"},
        {jcc::Operator::Or, "||"},
        {jcc::Operator::Xor, "^^"},
        {jcc::Operator::LessThanOrEqual, "<="},
        {jcc::Operator::GreaterThanOrEqual, ">="},
        {jcc::Operator::LessThan, "<"},
        {jcc::Operator::GreaterThan, ">"},

        {jcc::Operator::Assign, "="},
        {jcc::Operator::NullCoalesce, "??"},
        {jcc::Operator::At, "@"},

        {jcc::Operator::FloorDivide, "//"},
        {jcc::Operator::Increment, "++"},
        {jcc::Operator::Decrement, "--"},
        {jcc::Operator::Plus, "+"},
        {jcc::Operator::Minus, "-"},
        {jcc::Operator::Times, "*"},
        {jcc::Operator::FloatingDivide, "/"},
        {jcc::Operator::Modulus, "%"},
        {jcc::Operator::BitwiseAnd, "&"},
        {jcc::Operator::BitwiseOr, "|"},
        {jcc::Operator::BitwiseXor, "^"},
        {jcc::Operator::BitwiseNot, "~"},
        {jcc::Operator::Not, "!"},
        {jcc::Operator::Ternary, "?"},
        {jcc::Operator::Preprocessor, "#"},
        {jcc::Operator::MemberAccess, "."},
        {jcc::Operator::Comma, ","},
        {jcc::Operator::New, "new"},
        {jcc::Operator::Delete, "delete"}};

    typedef std::variant<std::string, uint64_t, double, Operator, Keyword, Punctuator>
        TokenValueType;

    class Token
    {
    public:
        Token();

        ~Token() = default;

        /// @brief Construct a new Token object
        /// @param type The type of the token
        /// @param value The value of the token
        Token(TokenType type, TokenValueType value);

        /// @brief Get the type of the token
        /// @return TokenType
        TokenType type() const;

        /// @brief Get the value of the token
        /// @return std::string
        const TokenValueType &value() const;

        /// @brief Convert the token to a string
        /// @return std::string
        std::string to_string() const;

    private:
        TokenType m_type;
        TokenValueType m_value;
    };

    class TokenList
    {
    public:
        /// @brief Construct a new TokenList object
        TokenList();

        /// @brief Construct a new TokenList object
        TokenList(const std::vector<Token> &tokens);

        ~TokenList() = default;

        /// @brief Push a token to the back of the list
        /// @param token The token to push
        void push_back(const Token &token);

        /// @brief Push a vector of tokens to the back of the list
        /// @param tokens The vector of tokens to push
        void push_back(const std::vector<Token> &tokens);

        /// @brief Lock the list
        /// @details Once the list is locked, no more tokens can be pushed.
        void lock();

        /// @brief Check if the list is locked
        /// @return true if the list is locked
        bool is_locked() const;

        /// @brief Convert TokenList to a human-readable string
        /// @return std::string
        std::string to_string() const;

        /// @brief Convert TokenList to an ANSI string
        /// @return std::string
        std::string to_json() const;

        /// @brief Get the token at the specified index
        /// @param index The index of the token
        /// @return Token
        const Token &operator[](size_t index) const;

        /// @brief Pop the first token off the list
        void pop(size_t count = 1);

        /// @brief Peek at a token
        /// @param index The index of the token to peek at
        /// @return Token
        const Token &peek(size_t index = 0) const;

        /// @brief Check if the list is empty
        /// @return true if the list is empty
        bool eof() const;

        /// @brief Get the size of the list
        /// @return size_t
        size_t size() const;

        /// @brief Get the tokens. Throws exception if the list is locked.
        /// @return std::vector<Token>
        std::vector<Token> &data();

        std::vector<Token> m_tokens;

    private:
        bool m_locked;
    };

    class LexerException : public std::runtime_error
    {
    public:
        LexerException(const std::string &message)
            : std::runtime_error(message) {}
    };

    class LexerExceptionInvalid : public LexerException
    {
    public:
        LexerExceptionInvalid(const std::string &message)
            : LexerException(message)
        {
        }
    };

    class LexerExceptionUnexpected : public LexerException
    {
    public:
        LexerExceptionUnexpected(const std::string &message)
            : LexerException(message)
        {
        }
    };

    class LexerExceptionInvalidLiteral : public LexerExceptionInvalid
    {
    public:
        LexerExceptionInvalidLiteral(const std::string &message)
            : LexerExceptionInvalid(message)
        {
        }
    };

    class LexerExceptionInvalidIdentifier : public LexerExceptionInvalid
    {
    public:
        LexerExceptionInvalidIdentifier(const std::string &message)
            : LexerExceptionInvalid(message)
        {
        }
    };

    class LexerExceptionInvalidOperator : public LexerExceptionInvalid
    {
    public:
        LexerExceptionInvalidOperator(const std::string &message)
            : LexerExceptionInvalid(message)
        {
        }
    };

    class LexerExceptionInvalidPunctuator : public LexerExceptionInvalid
    {
    public:
        LexerExceptionInvalidPunctuator(const std::string &message)
            : LexerExceptionInvalid(message)
        {
        }
    };

    class LexerExceptionUnexpectedEOF : public LexerExceptionUnexpected
    {
    public:
        LexerExceptionUnexpectedEOF(const std::string &message)
            : LexerExceptionUnexpected(message)
        {
        }
    };
}

#endif // _JCC_LEXER_HPP_