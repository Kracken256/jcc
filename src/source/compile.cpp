#include "compile.hpp"
#include <stdexcept>
#include <openssl/evp.h>
#include <gmpxx.h>
#include <algorithm>
#include <filesystem>
#include <thread>
#include <iostream>

///=============================================================================
/// jcc::CompilerMessage class implementation
///=============================================================================

jcc::CompilerMessage::CompilerMessage()
{
    m_message = "";
    m_file = "";
    m_line = 0;
    m_column = 0;
    m_type = CompilerMessageType::Common;
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
    switch (this->m_type)
    {
    case CompilerMessageType::Common:
        return std::string(this->m_file + ":" + std::to_string(this->m_line) + ":" + std::to_string(this->m_column) + ": Generic: " + this->m_message);
    case CompilerMessageType::Error:
        return std::string(this->m_file + ":" + std::to_string(this->m_line) + ":" + std::to_string(this->m_column) + ": Error: " + this->m_message);
    case CompilerMessageType::Warning:
        return std::string(this->m_file + ":" + std::to_string(this->m_line) + ":" + std::to_string(this->m_column) + ": Warning: " + this->m_message);
        break;
    case CompilerMessageType::Info:
        return std::string(this->m_file + ":" + std::to_string(this->m_line) + ":" + std::to_string(this->m_column) + ": Info: " + this->m_message);
    default:
        throw std::runtime_error("CompilerMessage::message(): Unknown message type");
    }

    return "";
}

std::string jcc::CompilerMessage::ansi_message() const
{
    switch (this->m_type)
    {
    case CompilerMessageType::Common:
        return std::string("\x1b[37;49;1m" + this->m_file + ":" + std::to_string(this->m_line) + ":" + std::to_string(this->m_column) + ":\x1b[0m Generic:\x1b[0m " + this->m_message);
    case CompilerMessageType::Error:
        return std::string("\x1b[37;49;1m" + this->m_file + ":" + std::to_string(this->m_line) + ":" + std::to_string(this->m_column) + ": \x1b[31;49;1mError:\x1b[0m " + this->m_message);
    case CompilerMessageType::Warning:
        return std::string("\x1b[37;49;1m" + this->m_file + ":" + std::to_string(this->m_line) + ":" + std::to_string(this->m_column) + ": \x1b[35;49;1mWarning:\x1b[0m " + this->m_message);
        break;
    case CompilerMessageType::Info:
        return std::string("\x1b[37;49;1m" + this->m_file + ":" + std::to_string(this->m_line) + ":" + std::to_string(this->m_column) + ": \x1b[36;49;1mInfo:\x1b[0m " + this->m_message);
    default:
        throw std::runtime_error("CompilerMessage::ansi_message(): Unknown message type");
    }

    return "";
}

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

uint32_t unix_timestamp()
{
    return (uint32_t)time(NULL);
}

void jcc::CompilerMessage::generate_hash()
{
    std::string message;
    unsigned char hash[EVP_MAX_MD_SIZE];
    char binary_portion[12 + 4 + 2 + 4];
    uint32_t unix_timestamp;
    uint64_t small_hash = 0;
    EVP_MD_CTX *mdctx;
    const EVP_MD *md;
    unsigned int md_len;

    message = this->m_message + "::" + this->m_file + "::" + std::to_string(this->m_line) + "::" + std::to_string(this->m_column) + "::";

    md = EVP_get_digestbyname("sha3-256");

    if (!md)
    {
        throw std::runtime_error("jcc::CompilerMessage::generate_hash(): Unable to get digest");
    }

    mdctx = EVP_MD_CTX_new();
    if (!mdctx)
    {
        throw std::runtime_error("jcc::CompilerMessage::generate_hash(): Unable to create digest context");
    }

    if (1 != EVP_DigestInit_ex(mdctx, md, NULL))
    {
        throw std::runtime_error("jcc::CompilerMessage::generate_hash(): Unable to initialize digest");
    }

    if (1 != EVP_DigestUpdate(mdctx, message.c_str(), message.length()))
    {
        throw std::runtime_error("jcc::CompilerMessage::generate_hash(): Unable to update digest");
    }

    if (1 != EVP_DigestFinal_ex(mdctx, hash, &md_len))
    {
        throw std::runtime_error("jcc::CompilerMessage::generate_hash(): Unable to finalize digest");
    }

    EVP_MD_CTX_free(mdctx);

    for (int i = 0; i < 8; i++)
    {
        small_hash |= (uint64_t)hash[i] << (i * 8);
    }

    this->m_hash = small_hash;

    unix_timestamp = ::unix_timestamp();

    memcpy(binary_portion, hash, 12);
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
    m_flags = {};
    m_output_file = "";
    m_messages = {};
    m_success = false;
    m_current_file = 0;
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

bool jcc::CompilationUnit::build()
{
    /// TODO: implement
    // this->m_success = true;

    this->push_message(CompilerMessageType::Error, "Compiler not implemented yet");

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

void jcc::CompilationJob::add_unit(const std::string &unit_name, std::shared_ptr<jcc::CompilationUnit> unit)
{
    if (m_units.find(unit_name) != m_units.end())
    {
        throw std::runtime_error("jcc::CompilationJob::add_unit(): Unit with name '" + unit_name + "' already exists");
    }

    m_units[unit_name] = unit;
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

static void build_unit(std::shared_ptr<jcc::CompilationUnit> unit)
{
    unit->build();
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

        for (const auto &unit : m_units)
        {
            std::thread thread = std::thread(build_unit, unit.second);
            threads.push_back(std::move(thread));
            thread.detach();
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