#include "compile.hpp"
#include "preprocessor.hpp"
#include <stdexcept>
#include <algorithm>
#include <filesystem>
#include <thread>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <atomic>
#include <iomanip>
#include <cstring>

#define _JCC_BACKEND_
#include "sha256.hpp"

#if defined(__linux__)
#include <execinfo.h>
#include <gmpxx.h>
#else
#error "Cross-platform support is not implemented yet"
#endif

///=============================================================================
/// jcc::CompilerMessage class implementation
///=============================================================================

jcc::CompilerMessage::CompilerMessage()
{
    m_message = "";
    m_file = "";
    m_line = 0;
    m_column = 0;
    m_type = CompilerMessageType::Debug;
    m_hash = 0;
    m_longhash = "";

    this->generate_hash();
}

jcc::CompilerMessage::CompilerMessage(const std::string &message, const std::string &file, int line, int column, jcc::CompilerMessageType type)
{
    m_message = message;
    m_file = file;
    m_line = line;
    m_column = column;
    m_type = type;
    m_hash = 0;
    m_longhash = "";

    this->generate_hash();
}

const std::string &jcc::CompilerMessage::message_raw() const
{
    return m_message;
}

const std::string &jcc::CompilerMessage::file() const
{
    return m_file;
}

size_t jcc::CompilerMessage::line() const
{
    return m_line;
}

size_t jcc::CompilerMessage::column() const
{
    return m_column;
}

jcc::CompilerMessageType jcc::CompilerMessage::type() const
{
    return m_type;
}

uint64_t jcc::CompilerMessage::hash() const
{
    return m_hash;
}

std::string jcc::CompilerMessage::longhash() const
{
    return m_longhash;
}

std::string jcc::CompilerMessage::message() const
{
    std::string part1, part2, part3, part4;

    if (!this->m_file.empty())
    {
        part1 = this->m_file + ":";
    }

    if (this->m_line != 0 && m_line != 0)
    {
        part2 = std::to_string(this->m_line) + ":" + std::to_string(this->m_column) + ": ";
    }
    else
    {
        part1 += " ";
    }

    switch (this->m_type)
    {
    case CompilerMessageType::Debug:
        part3 = "Debug: ";
        break;
    case CompilerMessageType::Error:
        part3 = "Error: ";
        break;
    case CompilerMessageType::Warning:
        part3 = "Warning: ";
        break;
    case CompilerMessageType::Info:
        part3 = "Info: ";
        break;
    default:
        panic("CompilerMessage::message(): Unknown message type", {});
    }

    part4 = this->m_message;

    return part1 + part2 + part3 + part4;
}

std::string jcc::CompilerMessage::ansi_message() const
{
    std::string part1, part2, part3, part4;

    if (!this->m_file.empty())
    {
        part1 = "\x1b[37;49;1m" + this->m_file + ":";
    }

    if (this->m_line != 0 && m_line != 0)
    {
        part2 = std::to_string(this->m_line) + ":" + std::to_string(this->m_column) + ":\x1b[0m ";
    }
    else
    {
        part1 += "\x1b[0m ";
    }

    switch (this->m_type)
    {
    case CompilerMessageType::Debug:
        part3 = "\x1b[36;49;1mDebug:\x1b[0m ";
        break;
    case CompilerMessageType::Error:
        part3 = "\x1b[31;49;1mError:\x1b[0m ";
        break;
    case CompilerMessageType::Warning:
        part3 = "\x1b[35;49;1mWarning:\x1b[0m ";
        break;
    case CompilerMessageType::Info:
        part3 = "\x1b[36;49;1mInfo:\x1b[0m ";
        break;
    default:
        panic("CompilerMessage::ansi_message(): Unknown message type", {});
    }

    part4 = this->m_message;

    return part1 + part2 + part3 + part4;
}

#if defined(__linux__)
static std::string base58_encode(const std::string &input)
{
    static const char *ALPHABET = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
    static const size_t ALPHABET_SIZE = 58;

    std::string output;
    output.reserve(input.size() * 138 / 100 + 1);

    mpz_class z;
    mpz_import(z.get_mpz_t(), input.size(), 1, 1, 0, 0, input.data());

    while (z > 0)
    {
        mpz_class r;
        mpz_class q;
        mpz_class b(ALPHABET_SIZE);

        mpz_fdiv_qr(q.get_mpz_t(), r.get_mpz_t(), z.get_mpz_t(), b.get_mpz_t());
        z = q;

        output += ALPHABET[r.get_ui()];
    }

    for (auto it = input.begin(); it != input.end() && *it == 0; ++it)
    {
        output += ALPHABET[0];
    }

    std::reverse(output.begin(), output.end());
    return output;
}
#else

#error "Cross-platform support is not implemented yet"

static std::string base58_encode(const std::string &input)
{
    return "";
}

#endif

static uint32_t unix_timestamp()
{
    return (uint32_t)((uint64_t)std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() & 0xFFFFFFFF);
}

void jcc::CompilerMessage::generate_hash()
{
    std::string message;
    std::string hash;
    char binary_portion[12 + 4 + 2 + 4];
    uint32_t unix_timestamp;
    uint64_t small_hash = 0;

    message = this->m_message + "::" + this->m_file + "::" + std::to_string(this->m_line) + "::" + std::to_string(this->m_column) + "::";

    hash = jcc::crypto::sha256(message);

    for (int i = 0; i < 8; i++)
    {
        small_hash |= (uint64_t)hash[i] << (i * 8);
    }

    this->m_hash = small_hash;

    unix_timestamp = ::unix_timestamp();

    memcpy(binary_portion, hash.c_str(), 12);
    binary_portion[12] = unix_timestamp & 0xFF;
    binary_portion[13] = (unix_timestamp >> 8) & 0xFF;
    binary_portion[14] = (unix_timestamp >> 16) & 0xFF;
    binary_portion[15] = (unix_timestamp >> 24) & 0xFF;
    binary_portion[16] = this->m_line & 0xFF;
    binary_portion[17] = (this->m_line >> 8) & 0xFF;
    binary_portion[18] = this->m_column & 0xFF;
    binary_portion[19] = (this->m_column >> 8) & 0xFF;
    binary_portion[20] = (this->m_column >> 16) & 0xFF;
    binary_portion[21] = (this->m_column >> 24) & 0xFF;

    this->m_longhash = "JC0" + base58_encode(std::string(binary_portion, 22));
}

///=============================================================================
/// jcc::CompilationUnit class implementation
///=============================================================================

jcc::CompilationUnit::CompilationUnit()
{
    m_files = {};
    m_current_file = 0;
    m_flags = {};
    m_output_file = "a.out";
    m_cxx_temp_files = {};
    m_obj_temp_files = {};
    m_messages = {};
    m_success = false;
}

jcc::CompilationUnit::~CompilationUnit()
{
    for (const auto &file : this->m_cxx_temp_files)
    {
        std::remove(file.second.c_str());
    }

    for (const auto &file : this->m_obj_temp_files)
    {
        std::remove(file.second.c_str());
    }
}

bool jcc::CompilationUnit::add_file(const std::string &file)
{
    if (!std::filesystem::exists(file))
    {
        return false;
    }

    m_files.push_back(file);
    return true;
}

void jcc::CompilationUnit::add_flag(CompileFlag flag)
{
    m_flags.insert(flag);
}

void jcc::CompilationUnit::set_output_file(const std::string &file)
{
    m_output_file = file;
}

const std::vector<std::string> &jcc::CompilationUnit::files() const
{
    return m_files;
}

const std::set<jcc::CompileFlag> &jcc::CompilationUnit::flags() const
{
    return m_flags;
}

const std::string &jcc::CompilationUnit::output_file() const
{
    return m_output_file;
}

const std::vector<std::shared_ptr<jcc::CompilerMessage>> &jcc::CompilationUnit::messages() const
{
    return m_messages;
}

std::vector<jcc::CompilerWarning> jcc::CompilationUnit::warnings() const
{
    std::vector<jcc::CompilerWarning> warnings;

    for (const auto &message : this->m_messages)
    {
        if (message->type() == CompilerMessageType::Warning)
        {
            warnings.push_back(reinterpret_cast<const jcc::CompilerWarning &>(*message));
        }
    }

    return warnings;
}

std::vector<jcc::CompilerError> jcc::CompilationUnit::errors() const
{
    std::vector<jcc::CompilerError> errors;

    for (const auto &message : this->m_messages)
    {
        if (message->type() == CompilerMessageType::Error)
        {
            errors.push_back(reinterpret_cast<const jcc::CompilerError &>(*message));
        }
    }

    return errors;
}

std::vector<jcc::CompilerInfo> jcc::CompilationUnit::infos() const
{
    std::vector<jcc::CompilerInfo> infos;

    for (const auto &message : this->m_messages)
    {
        if (message->type() == CompilerMessageType::Info)
        {
            infos.push_back(reinterpret_cast<const jcc::CompilerInfo &>(*message));
        }
    }

    return infos;
}

static std::string to_objname(const std::string &input_cxx)
{
    static uint64_t counter = 0;
    static std::mutex counter_mutex;
    static const char *hex = "0123456789abcdef";

    std::lock_guard<std::mutex> lock(counter_mutex);

    std::string result = "/tmp/jcc-";

    // convert 64-bit counter to hex
    for (int i = 0; i < 16; i++)
    {
        result += hex[(counter >> (i * 4)) & 0xF];
    }

    result += ".obj";

    counter++;

    return result;
}

bool jcc::CompilationUnit::invoke_jcc_helper_cxx2obj(const std::string &input_cxx, std::string &output_obj, const std::vector<std::string> &flags, const std::string &program)
{
    /// TODO: Implement

    // generate output filename
    output_obj = to_objname(input_cxx);

    std::string cmd = program + " " + input_cxx + " -o " + output_obj;

    for (const auto &flag : flags)
    {
        cmd += " " + flag;
    }

    cmd += " -c"; // compile only

    push_message(CompilerMessageType::Debug, "BUILD CMD: " + cmd);

    return system(cmd.c_str()) == 0;
}

bool jcc::CompilationUnit::invoke_jcc_helper_ld(const std::vector<std::string> &input_objs, const std::string &outputname, const std::vector<std::string> &flags, const std::string &program)
{
    /// TODO: Implement

    std::string ld_cmd = program + " ";

    for (const auto &obj : input_objs)
    {
        ld_cmd += obj + " ";
    }

    ld_cmd += "-o " + outputname;

    for (const auto &flag : flags)
    {
        ld_cmd += " " + flag;
    }

    push_message(CompilerMessageType::Debug, "BUILD CMD: " + ld_cmd);

    return system(ld_cmd.c_str()) == 0;
}

void jcc::CompilationUnit::reset_instance()
{
    this->m_files.clear();
    this->m_messages.clear();
    this->m_success = false;
    this->m_current_file = 0;
    this->m_cxx_temp_files.clear();
    this->m_obj_temp_files.clear();
}

static std::vector<std::string> parse_env_list(const char *val)
{
    std::vector<std::string> list;

    while (*val != '\0')
    {
        std::string flag;

        while (*val != '\0' && *val != ' ')
        {
            flag += *val;
            val++;
        }

        if (!flag.empty())
        {
            list.push_back(flag);
        }

        if (*val == '\0')
        {
            break;
        }

        val++;
    }

    return list;
}

bool jcc::CompilationUnit::build()
{
    const char *env_jcc_helper, *env_jcc_helper_flags, *env_jcc_ld, *env_jcc_ldflags;
    std::vector<std::string> cxx_flags, ld_flags;

    this->m_success = false;

    if ((env_jcc_helper = std::getenv("JCC_CXX")) == NULL)
    {
        this->push_message(CompilerMessageType::Error, "JCC_CXX environment variable not set. Unable to compile generated C++ code. Set the variable to the path of a compatible C++ compiler.");
        return false;
    }

    if ((env_jcc_helper_flags = std::getenv("JCC_CXX_FLAGS")) == NULL)
    {
        this->push_message(CompilerMessageType::Info, "JCC_CXX_FLAGS environment variable not set. Using default flags.");
        env_jcc_helper_flags = "";
    }

    if ((env_jcc_ld = std::getenv("JCC_LD")) == NULL)
    {
        this->push_message(CompilerMessageType::Error, "JCC_LD environment variable not set. Unable to link generated object files. Set the variable to the path of a compatible linker.");
        return false;
    }

    if ((env_jcc_ldflags = std::getenv("JCC_LDFLAGS")) == NULL)
    {
        this->push_message(CompilerMessageType::Info, "JCC_LDFLAGS environment variable not set. Using default flags.");
        env_jcc_ldflags = "";
    }

    cxx_flags = parse_env_list(env_jcc_helper_flags);
    ld_flags = parse_env_list(env_jcc_ldflags);

    for (const auto &file : this->m_files)
    {
        if (!compile_file(file))
        {
            return false;
        }
    }

    /// TODO: Debug code, remove later
    if (this->m_cxx_temp_files.size() != m_files.size())
    {
        m_success = true;
        return true;
    }

    // foreach in m_temp_files map
    for (const auto &file : this->m_cxx_temp_files)
    {
        std::string objpath;
        if (!invoke_jcc_helper_cxx2obj(file.second, objpath, cxx_flags, env_jcc_helper))
        {
            this->push_message(CompilerMessageType::Error, "Downstream c++ compilation failed. Failed to build generated C++ code.");
            return false;
        }

        // add to m_obj_temp_files map
        m_obj_temp_files[file.second] = objpath;
    }

    std::vector<std::string> obj_files;
    for (const auto &file : this->m_obj_temp_files)
    {
        obj_files.push_back(file.second);
    }

    if (!invoke_jcc_helper_ld(obj_files, this->m_output_file, ld_flags, env_jcc_ld))
    {
        this->push_message(CompilerMessageType::Error, "Downstream linking failed. Failed to link generated object files.");
        return false;
    }

    this->m_success = true;

    return true;
}

// https://stackoverflow.com/questions/28270310/how-to-easily-detect-utf8-encoding-in-the-string
static bool is_valid_utf8(const std::string &input)
{
    if (input.empty())
        return true;

    const unsigned char *bytes = (const unsigned char *)input.c_str();
    unsigned int cp;
    int num;

    while (*bytes != 0x00)
    {
        if ((*bytes & 0x80) == 0x00)
        {
            // U+0000 to U+007F
            cp = (*bytes & 0x7F);
            num = 1;
        }
        else if ((*bytes & 0xE0) == 0xC0)
        {
            // U+0080 to U+07FF
            cp = (*bytes & 0x1F);
            num = 2;
        }
        else if ((*bytes & 0xF0) == 0xE0)
        {
            // U+0800 to U+FFFF
            cp = (*bytes & 0x0F);
            num = 3;
        }
        else if ((*bytes & 0xF8) == 0xF0)
        {
            // U+10000 to U+10FFFF
            cp = (*bytes & 0x07);
            num = 4;
        }
        else
            return false;

        bytes += 1;
        for (int i = 1; i < num; ++i)
        {
            if ((*bytes & 0xC0) != 0x80)
                return false;
            cp = (cp << 6) | (*bytes & 0x3F);
            bytes += 1;
        }

        if ((cp > 0x10FFFF) ||
            ((cp >= 0xD800) && (cp <= 0xDFFF)) ||
            ((cp <= 0x007F) && (num != 1)) ||
            ((cp >= 0x0080) && (cp <= 0x07FF) && (num != 2)) ||
            ((cp >= 0x0800) && (cp <= 0xFFFF) && (num != 3)) ||
            ((cp >= 0x10000) && (cp <= 0x1FFFFF) && (num != 4)))
            return false;
    }

    return true;
}

bool jcc::CompilationUnit::read_source_code(const std::string &filepath, std::string &source_code)
{
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);

    if (!file.is_open())
    {
        this->push_message(CompilerMessageType::Error, "Unable to open file '" + filepath + "' for reading");
        return false;
    }

    std::streampos sizepos = file.tellg();

    if (file.fail())
    {
        this->push_message(CompilerMessageType::Error, "Unable to get size of file '" + filepath + "'");
        file.close();
        return false;
    }

    if (sizepos == 0)
    {
        this->push_message(CompilerMessageType::Warning, "File '" + filepath + "' is empty");
        file.close();
        return true;
    }

    if (sizepos == INT64_MAX)
    {
        this->push_message(CompilerMessageType::Error, "Unable to get size of file '" + filepath + "'");
        file.close();
        return false;
    }

    file.seekg(0, std::ios::beg);

    source_code.resize(sizepos);

    if (!file.read(source_code.data(), sizepos))
    {
        this->push_message(CompilerMessageType::Error, "Unable to read file '" + filepath + "'");
        file.close();
        return false;
    }

    file.close();

    // prelininary check for UTF-8
    if (!is_valid_utf8(source_code))
    {
        this->push_message(CompilerMessageType::Error, "File '" + filepath + "' is not a valid J++ source file (invalid UTF-8)");
        return false;
    }

    bool found_non_ascii = false;
    for (const auto &c : source_code)
    {
        if (c & 0x80)
        {
            found_non_ascii = true;
            break;
        }
    }
    if (found_non_ascii)
    {
        this->push_message(CompilerMessageType::Info, "File contains non-ASCII characters. This is fine, just a heads up.");
    }

    return true;
}

bool jcc::CompilationUnit::compile_file(const std::string &file)
{
    std::string source_code, preprocessed_code;
    TokenList tokens;

    if (!read_source_code(file, source_code))
    {
        this->push_message(CompilerMessageType::Info, "Disregarding file '" + file + "' due to previous errors");
        return false;
    }
    try
    {
        preprocessed_code = Preprocessor::preprocess(source_code);
    }
    catch (const PreprocessorImportNotFoundException &e)
    {
        this->push_message(CompilerMessageType::Error, "Import not found: " + std::string(e.what()));
        return false;
    }
    catch (const PreprocessorImportCyclicException &e)
    {
        this->push_message(CompilerMessageType::Error, "Cyclic import: " + std::string(e.what()));
        return false;
    }
    catch (const PreprocessorTokenException &e)
    {
        this->push_message(CompilerMessageType::Error, "Invalid token: " + std::string(e.what()));
        return false;
    }
    catch (const PreprocessorException &e)
    {
        this->push_message(CompilerMessageType::Error, "Preprocessor error: " + std::string(e.what()));
        return false;
    }
    catch (const std::exception &e)
    {
        this->push_message(CompilerMessageType::Error, "Internal compiler error: Preprocessor::preprocess(" + std::string(e.what()) + ")");
        panic("Caught unexpected exception in Preprocessor::preprocess()", this->m_messages);
        return false;
    }

    if (preprocessed_code.empty())
    {
        this->push_message(CompilerMessageType::Info, "Disregarding empty file '" + file + "'");
        return true;
    }

    try
    {
        tokens = lex(preprocessed_code);
    }
    catch (const LexerException &e)
    {
        this->push_message(CompilerMessageType::Error, "Lexer::lex(" + std::string(e.what()) + ")");
        return false;
    }
    catch (const std::exception &e)
    {
        this->push_message(CompilerMessageType::Error, "Internal compiler error: Lexer::lex(" + std::string(e.what()) + ")");
        panic("Caught unexpected exception in Lexer::lex()", this->m_messages);
        return false;
    }

    std::shared_ptr<AbstractSyntaxTree> ast;

    try
    {
        ast = parse(tokens);
    }
    catch (const SyntaxError &e)
    {
        this->push_message(CompilerMessageType::Error, "Parser error: " + std::string(e.what()));
        return false;
    }
    catch (const SemanticError &e)
    {
        this->push_message(CompilerMessageType::Error, "Semantic error: " + std::string(e.what()));
        return false;
    }
    catch (const UnexpectedTokenError &e)
    {
        this->push_message(CompilerMessageType::Error, "Unexpected token: " + std::string(e.what()));
        return false;
    }
    catch (const ParserException &e)
    {
        this->push_message(CompilerMessageType::Error, "Parser error: " + std::string(e.what()));
        return false;
    }
    catch (const std::exception &e)
    {
        this->push_message(CompilerMessageType::Error, "Internal compiler error: Parser::parse(" + std::string(e.what()) + ")");
        panic("Caught unexpected exception in Parser::parse()", this->m_messages);
        return false;
    }

    if (ast == nullptr)
    {
        this->push_message(CompilerMessageType::Error, "Failed to parse source");
        return false;
    }

    /// TODO: Implement semantic analyzer

    /// TODO: Implement AST optimizer

    std::string cxx_code;

    try
    {
        cxx_code = generate(ast);
    }
    catch (const std::exception &e)
    {
        this->push_message(CompilerMessageType::Error, "Internal compiler error: Generator::generate(" + std::string(e.what()) + ")");
        panic("Caught unexpected exception in Generator::generate()", this->m_messages);
        return false;
    }

    std::string temp_file = std::tmpnam(nullptr) + std::string(".cpp");

    std::ofstream temp_file_stream(temp_file, std::ios::binary);

    if (!temp_file_stream.is_open())
    {
        this->push_message(CompilerMessageType::Error, "Unable to open temporary file '" + temp_file + "' for writing");
        return false;
    }

    temp_file_stream << cxx_code;

    temp_file_stream.close();

    this->m_cxx_temp_files[file] = temp_file;

    return true;
}

bool jcc::CompilationUnit::success() const
{
    return this->m_success;
}

void jcc::CompilationUnit::push_message(jcc::CompilerMessageType type, const std::string &message, const std::string &file, int line, int column)
{
    std::string fname;

    if (file.empty())
    {
        fname = this->m_files[this->m_current_file];
    }
    else
    {
        fname = file;
    }

    this->m_messages.push_back(std::make_shared<jcc::CompilerMessage>(message, fname, line, column, type));
}

///=============================================================================
/// jcc::CompilationJob class implementation
///=============================================================================

jcc::CompilationJob::CompilationJob()
{
    m_units = {};
    m_output_file = "";
    m_progress = 0;
}

void jcc::CompilationJob::add_unit(const std::string &unit_name, std::unique_ptr<jcc::CompilationUnit> unit)
{
    if (m_units.find(unit_name) != m_units.end())
    {
        throw std::runtime_error("jcc::CompilationJob::add_unit(): Unit with name '" + unit_name + "' already exists");
    }

    m_units[unit_name] = std::move(unit);
}

void jcc::CompilationJob::set_output_file(const std::string &output_file)
{
    m_output_file = output_file;
}

const std::shared_ptr<jcc::CompilationUnit> jcc::CompilationJob::get_unit(const std::string &unit_name)
{
    if (m_units.find(unit_name) == m_units.end())
    {
        throw std::out_of_range("jcc::CompilationJob::get_unit(): Unit with name '" + unit_name + "' does not exist");
    }

    return m_units.at(unit_name);
}

const std::map<std::string, std::shared_ptr<jcc::CompilationUnit>> &jcc::CompilationJob::units() const
{
    return m_units;
}

bool jcc::CompilationJob::run_job(bool detatch)
{
    if (detatch)
    {
        std::thread job_thread(&jcc::CompilationJob::run_job_internal, this);
        job_thread.detach();
        return true;
    }

    return this->run_job_internal();
}

std::vector<std::shared_ptr<jcc::CompilerMessage>> jcc::CompilationJob::messages() const
{
    std::vector<std::shared_ptr<jcc::CompilerMessage>> messages_union;

    for (const auto &unit : this->m_units)
    {
        for (const auto &message : unit.second->messages())
        {
            messages_union.push_back(message);
        }
    }

    return messages_union;
}

std::vector<jcc::CompilerWarning> jcc::CompilationJob::warnings() const
{
    std::vector<jcc::CompilerWarning> warnings_union;

    for (const auto &unit : this->m_units)
    {
        for (const auto &message : unit.second->warnings())
        {
            warnings_union.push_back(message);
        }
    }

    return warnings_union;
}

std::vector<jcc::CompilerError> jcc::CompilationJob::errors() const
{
    std::vector<jcc::CompilerError> errors_union;

    for (const auto &unit : this->m_units)
    {
        for (const auto &message : unit.second->errors())
        {
            errors_union.push_back(message);
        }
    }

    return errors_union;
}

std::vector<jcc::CompilerInfo> jcc::CompilationJob::infos() const
{
    std::vector<jcc::CompilerInfo> infos_union;

    for (const auto &unit : this->m_units)
    {
        for (const auto &message : unit.second->infos())
        {
            infos_union.push_back(message);
        }
    }

    return infos_union;
}

uint8_t jcc::CompilationJob::progress() const
{
    return m_progress;
}

bool jcc::CompilationJob::run_job_internal()
{
    if (m_units.empty())
    {
        return false; // nothing to do
    }

    if (m_units.size() > 1)
    {
        // create a thread for each unit
        // wait for all threads to finish

        std::vector<std::thread> threads;

        for (auto unit : m_units)
        {
            threads.emplace_back([unit]()
                                 { unit.second->build(); });
        }

        for (auto &thread : threads)
        {
            thread.join();
        }
    }
    else
    {
        // just build the unit
        m_units.begin()->second->build();
    }

    for (const auto &unit : m_units)
    {
        if (!unit.second->success())
        {
            return false;
        }
    }

    return false;
}

bool jcc::CompilationJob::success() const
{
    for (const auto &unit : this->m_units)
    {
        if (!unit.second->success())
        {
            return false;
        }
    }

    return true;
}

///=============================================================================
/// jcc standalone functions
///=============================================================================

#if defined(__linux__)
static void print_stacktrace()
{
    std::cerr << "Stacktrace:" << std::endl;

    void *array[128];
    size_t size;

    size = backtrace(array, 128);

    char **strings = backtrace_symbols(array, size);

    for (size_t i = 0; i < size; i++)
    {
        std::cerr << "   \x1b[37;49m" << strings[i] << "\x1b[0m" << std::endl;
    }

    std::cerr << std::endl;

    free(strings);
}
#else
static void print_stacktrace()
{
    std::cerr << "Stacktrace: Not implemented for this platform.\n"
              << std::endl;
}
#endif

void jcc::panic(const std::string &message, std::vector<std::shared_ptr<jcc::CompilerMessage>> messages)
{
    static std::atomic<bool> panic_called = false;

    if (panic_called)
    {
        // prevent infinite recursion

        // what is this called? A function sink?
        while (1)
        {
            std::terminate();
        }
    }

    panic_called = true;

    std::stringstream autoreport_id;

    if (messages.size() > 5)
    {
        std::cerr << "\r\n\x1b[31;49;1m[\x1b[0m \x1b[31;49;1;4mINTERNAL COMPILER ERROR\x1b[0m \x1b[31;49;1m]\x1b[0m: \x1b[36;49;1m" << message << "\x1b[0m\n"
                  << std::endl;
    }
    else
    {
        std::cerr << "\r\n";
    }

    std::cerr << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
              << std::endl;

    for (const auto &message : messages)
    {
        std::cerr << message->ansi_message() << std::endl;

        autoreport_id << message->message_raw() << "::";
    }

    std::cerr << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
              << std::endl;

    std::cerr << "\x1b[31;49;1m[\x1b[0m \x1b[31;49;1;4mINTERNAL COMPILER ERROR\x1b[0m \x1b[31;49;1m]\x1b[0m: \x1b[36;49;1m" << message << "\x1b[0m\n"
              << std::endl;

    print_stacktrace();

    std::cerr
        << "\x1b[32;49mPlease report this error to the JCC developers\x1b[0m" << std::endl;

    // calculate autoreport id
    std::cerr << "\x1b[32;49mAutoreport ID:\x1b[0m \x1b[36;49;4m"
              << "JCR0-" + base58_encode(jcc::crypto::sha256(autoreport_id.str())) << "\x1b[0m\n"
              << std::endl;

    _exit(0);
}