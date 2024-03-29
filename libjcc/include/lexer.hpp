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
        Raw,
        Whitespace,
    };

    enum class Keyword
    {
        // Namespaces
        Subsystem,

        // Symbols import/export
        Import,
        Export,
        Extern,

        Infer,
        Let,
        Var,

        // Composites
        Struct,
        Region,
        Union,
        Packet,

        // Functions
        Func,

        // Modifiers
        Typedef,
        Const,
        Ref,
        Static,
        Volatile,

        // Object-oriented
        Class,
        Public,
        Private,
        Protected,
        Override,
        Claim,
        Virtual,
        Abstract,
        Friend,
        Interface,

        Meta,

        // Control flow
        If,
        Else,
        For,
        While,
        Do,
        Switch,
        Return,
        Retif,
        Retz,
        Retnz,
        Abortif,
        Case,
        Break,
        Default,
        Abort,
        Throw,
        Continue,

        // Enumeration
        Enum,

        // Types
        Bool,
        Char,
        Byte,
        Short,
        Word,
        Int,
        Dword,
        Long,
        Qword,
        Float,
        Double,
        Intn,
        Uintn,
        Address,
        Routine,
        Bigfloat,
        Bigint,
        Biguint,
        Arbint,
        Arbuint,
        Real,
        Complex,
        String,
        Map,
        Tensor,
        Void,
        Null,
    };

    const std::map<const char *, jcc::Keyword> lexKeywordMap = {
        {"subsystem", jcc::Keyword::Subsystem},

        {"import", jcc::Keyword::Import},
        {"export", jcc::Keyword::Export},
        {"extern", jcc::Keyword::Extern},

        {"infer", jcc::Keyword::Infer},
        {"let", jcc::Keyword::Let},
        {"var", jcc::Keyword::Var},

        {"struct", jcc::Keyword::Struct},
        {"region", jcc::Keyword::Region},
        {"union", jcc::Keyword::Union},
        {"packet", jcc::Keyword::Packet},

        {"func", jcc::Keyword::Func},

        {"typedef", jcc::Keyword::Typedef},
        {"const", jcc::Keyword::Const},
        {"ref", jcc::Keyword::Ref},
        {"static", jcc::Keyword::Static},
        {"volatile", jcc::Keyword::Volatile},

        {"class", jcc::Keyword::Class},
        {"public", jcc::Keyword::Public},
        {"private", jcc::Keyword::Private},
        {"protected", jcc::Keyword::Protected},
        {"override", jcc::Keyword::Override},
        {"claim", jcc::Keyword::Claim},
        {"virtual", jcc::Keyword::Virtual},
        {"abstract", jcc::Keyword::Abstract},
        {"friend", jcc::Keyword::Friend},
        {"interface", jcc::Keyword::Interface},

        {"meta", jcc::Keyword::Meta},

        {"if", jcc::Keyword::If},
        {"else", jcc::Keyword::Else},
        {"for", jcc::Keyword::For},
        {"while", jcc::Keyword::While},
        {"do", jcc::Keyword::Do},
        {"switch", jcc::Keyword::Switch},
        {"return", jcc::Keyword::Return},
        {"case", jcc::Keyword::Case},
        {"break", jcc::Keyword::Break},
        {"default", jcc::Keyword::Default},
        {"abort", jcc::Keyword::Abort},
        {"throw", jcc::Keyword::Throw},
        {"continue", jcc::Keyword::Continue},

        {"enum", jcc::Keyword::Enum},

        {"bool", jcc::Keyword::Bool},
        {"char", jcc::Keyword::Char},
        {"byte", jcc::Keyword::Byte},
        {"short", jcc::Keyword::Short},
        {"word", jcc::Keyword::Word},
        {"int", jcc::Keyword::Int},
        {"dword", jcc::Keyword::Dword},
        {"long", jcc::Keyword::Long},
        {"qword", jcc::Keyword::Qword},
        {"float", jcc::Keyword::Float},
        {"double", jcc::Keyword::Double},
        {"intn", jcc::Keyword::Intn},
        {"uintn", jcc::Keyword::Uintn},
        {"address", jcc::Keyword::Address},
        {"routine", jcc::Keyword::Routine},
        {"bigfloat", jcc::Keyword::Bigfloat},
        {"bigint", jcc::Keyword::Bigint},
        {"biguint", jcc::Keyword::Biguint},
        {"arbint", jcc::Keyword::Arbint},
        {"arbuint", jcc::Keyword::Arbuint},
        {"real", jcc::Keyword::Real},
        {"complex", jcc::Keyword::Complex},
        {"string", jcc::Keyword::String},
        {"map", jcc::Keyword::Map},
        {"tensor", jcc::Keyword::Tensor},
        {"void", jcc::Keyword::Void},
        {"null", jcc::Keyword::Null}};

    const std::map<jcc::Keyword, const char *> lexKeywordMapReverse = {
        {jcc::Keyword::Subsystem, "subsystem"},

        {jcc::Keyword::Import, "import"},
        {jcc::Keyword::Export, "export"},
        {jcc::Keyword::Extern, "extern"},

        {jcc::Keyword::Infer, "infer"},
        {jcc::Keyword::Let, "let"},
        {jcc::Keyword::Var, "var"},

        {jcc::Keyword::Struct, "struct"},
        {jcc::Keyword::Region, "region"},
        {jcc::Keyword::Union, "union"},
        {jcc::Keyword::Packet, "packet"},

        {jcc::Keyword::Func, "func"},

        {jcc::Keyword::Typedef, "typedef"},
        {jcc::Keyword::Const, "const"},
        {jcc::Keyword::Ref, "ref"},
        {jcc::Keyword::Static, "static"},
        {jcc::Keyword::Volatile, "volatile"},

        {jcc::Keyword::Class, "class"},
        {jcc::Keyword::Public, "public"},
        {jcc::Keyword::Private, "private"},
        {jcc::Keyword::Protected, "protected"},
        {jcc::Keyword::Override, "override"},
        {jcc::Keyword::Claim, "claim"},
        {jcc::Keyword::Virtual, "virtual"},
        {jcc::Keyword::Abstract, "abstract"},
        {jcc::Keyword::Friend, "friend"},
        {jcc::Keyword::Interface, "interface"},

        {jcc::Keyword::Meta, "meta"},

        {jcc::Keyword::If, "if"},
        {jcc::Keyword::Else, "else"},
        {jcc::Keyword::For, "for"},
        {jcc::Keyword::While, "while"},
        {jcc::Keyword::Do, "do"},
        {jcc::Keyword::Switch, "switch"},
        {jcc::Keyword::Return, "return"},
        {jcc::Keyword::Case, "case"},
        {jcc::Keyword::Break, "break"},
        {jcc::Keyword::Default, "default"},
        {jcc::Keyword::Abort, "abort"},
        {jcc::Keyword::Throw, "throw"},
        {jcc::Keyword::Continue, "continue"},

        {jcc::Keyword::Enum, "enum"},

        {jcc::Keyword::Bool, "bool"},
        {jcc::Keyword::Char, "char"},
        {jcc::Keyword::Byte, "byte"},
        {jcc::Keyword::Short, "short"},
        {jcc::Keyword::Word, "word"},
        {jcc::Keyword::Int, "int"},
        {jcc::Keyword::Dword, "dword"},
        {jcc::Keyword::Long, "long"},
        {jcc::Keyword::Qword, "qword"},
        {jcc::Keyword::Float, "float"},
        {jcc::Keyword::Double, "double"},
        {jcc::Keyword::Intn, "intn"},
        {jcc::Keyword::Uintn, "uintn"},
        {jcc::Keyword::Address, "address"},
        {jcc::Keyword::Routine, "routine"},
        {jcc::Keyword::Bigfloat, "bigfloat"},
        {jcc::Keyword::Bigint, "bigint"},
        {jcc::Keyword::Biguint, "biguint"},
        {jcc::Keyword::Arbint, "arbint"},
        {jcc::Keyword::Arbuint, "arbuint"},
        {jcc::Keyword::Real, "real"},
        {jcc::Keyword::Complex, "complex"},
        {jcc::Keyword::String, "string"},
        {jcc::Keyword::Map, "map"},
        {jcc::Keyword::Tensor, "tensor"},
        {jcc::Keyword::Void, "void"},
        {jcc::Keyword::Null, "null"}};

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
        {".", jcc::Punctuator::Period}};

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
        {jcc::Punctuator::Period, "."},};

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

        {"?", jcc::Operator::Ternary}};

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
        {jcc::Operator::Ternary, "?"}};

    typedef std::variant<std::string, Operator, Keyword, Punctuator>
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

        TokenType m_type;
        TokenValueType m_value;

        bool operator==(const Token &other) const;
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

        /// @brief Finish building the list
        void done();

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

        std::vector<Token> m_tokens;
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