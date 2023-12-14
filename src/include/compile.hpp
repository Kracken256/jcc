#ifndef _JCC_COMPILE_HPP_
#define _JCC_COMPILE_HPP_

#include <vector>
#include <string>
#include <map>
#include <set>
#include <cstdint>
#include <atomic>
#include <memory>

namespace jcc
{
    enum class CompileFlag
    {
        Verbose,
        Debug,
        OptimizeNone,
        OptimizeLight,
        OptimizeSpeed,
        OptimizeAggressive,
        OptimizeSize,
        Object,
        TranslateOnly,
    };

    enum class CompilerMessageType
    {
        Common,
        Error,
        Warning,
        Info,
    };

    class CompilerMessage
    {
    public:
        CompilerMessage();
        CompilerMessage(const std::string &message, const std::string &file, int line, int column, CompilerMessageType type = CompilerMessageType::Common);

        /// @brief Get plain message
        /// @return std::string
        const std::string &message_raw() const;

        /// @brief Get the filepath that the message originated from
        /// @return std::string
        const std::string &file() const;

        /// @brief Get the line number that the message originated from
        /// @return size_t
        size_t line() const;

        /// @brief Get the column number that the message originated from
        /// @return size_t
        size_t column() const;

        /// @brief Get the type of the message
        /// @return CompilerMessageType
        CompilerMessageType type() const;

        /// @brief Get the hash of the message
        /// @return uint64_t
        uint64_t hash() const;

        /// @brief Get the longhash of the message info for bug tracking/reporting
        /// @return std::string
        std::string longhash() const;

        /// @brief Get the message as a string
        std::string message() const;

        /// @brief Get the message as an ANSI formatted string
        /// @return std::string
        std::string ansi_message() const;

    protected:
        std::string m_message;
        std::string m_file;
        size_t m_line;
        size_t m_column;
        uint64_t m_hash;
        std::string m_longhash;
        CompilerMessageType m_type;

        void generate_hash();
    };

    class CompilerError : public CompilerMessage
    {
    public:
        CompilerError() : CompilerMessage("", "", 0, 0, CompilerMessageType::Error) {}
        CompilerError(const std::string &message, const std::string &file, int line, int column) : CompilerMessage(message, file, line, column, CompilerMessageType::Error) {}
    };

    class CompilerWarning : public CompilerMessage
    {
    public:
        CompilerWarning() : CompilerMessage("", "", 0, 0, CompilerMessageType::Warning) {}
        CompilerWarning(const std::string &message, const std::string &file, int line, int column) : CompilerMessage(message, file, line, column, CompilerMessageType::Warning) {}
    };

    class CompilerInfo : public CompilerMessage
    {
    public:
        CompilerInfo() : CompilerMessage("", "", 0, 0, CompilerMessageType::Info) {}
        CompilerInfo(const std::string &message, const std::string &file, int line, int column) : CompilerMessage(message, file, line, column, CompilerMessageType::Info) {}
    };

    class CompilationUnit
    {
    public:
        /// @brief Construct a new CompilationUnit object
        /// @note The temporary output file is generated automatically by default
        CompilationUnit();
        ~CompilationUnit() = default;

        /// @brief Add a file to the compilation unit
        /// @param file The file to add
        /// @return True if successful, false otherwise
        /// @note Returns false if file does not exist
        bool add_file(const std::string &file);

        /// @brief Add a flag to the compilation unit
        void add_flag(CompileFlag flag);

        /// @brief Change the output file of the compilation unit
        void set_output_file(const std::string &file);

        /// @brief Get the files in the compilation unit
        /// @return std::vector<std::string>
        const std::vector<std::string> &files() const;

        /// @brief Get the flags in the compilation unit
        /// @return std::vector<CompileFlag>
        const std::set<CompileFlag> &flags() const;

        /// @brief Get the output file of the compilation unit
        const std::string &output_file() const;

        /// @brief Get the messages from the compilation unit
        const std::vector<std::shared_ptr<CompilerMessage>> &messages() const;

        /// @brief Get the warnings from the compilation unit
        std::vector<CompilerWarning> warnings() const;

        /// @brief Get the errors from the compilation unit
        std::vector<CompilerError> errors() const;

        /// @brief Get the infos from the compilation unit
        std::vector<CompilerInfo> infos() const;

        /// @brief Build the compilation unit
        /// @return True if successful, false otherwise
        /// @note This function will populate the messages vector
        bool build();

        /// @brief Check if the compilation unit successfully built
        /// @return True if successful, false otherwise
        bool success() const;

    private:
        std::vector<std::string> m_files;
        size_t m_current_file;
        std::set<CompileFlag> m_flags;
        std::string m_output_file;
        std::vector<std::shared_ptr<CompilerMessage>> m_messages;
        bool m_success;

        /// @brief Push a message to the compilation unit
        /// @param type The type of the message
        /// @param message The message
        /// @param file The file that the message originated from
        /// @param line Line number that the message originated from
        /// @param column Column number that the message originated from
        void push_message(CompilerMessageType type, const std::string &message, const std::string &file = "", int line = 0, int column = 0);

        /// @brief Compile a file
        /// @param file The file to compile
        /// @return True if successful, false otherwise
        bool compile_file(const std::string &file);

        /// @brief Read the source code from a file
        /// @param filepath The path to the file
        /// @param source_code The source code
        /// @return True if successful, false otherwise
        bool read_source_code(const std::string &filepath, std::string &source_code);
    };

    class CompilationJob
    {
    public:
        CompilationJob();
        ~CompilationJob() = default;

        /// @brief Add a compilation unit to the job
        /// @param unit_name A unique name for the unit
        /// @param unit The compilation unit to add
        void add_unit(const std::string &unit_name, std::shared_ptr<CompilationUnit> unit);

        void set_output_file(const std::string &file);

        /// @brief Get a compilation unit from the job
        /// @param unit_name The name of the unit to get
        /// @return CompilationUnit
        const std::shared_ptr<CompilationUnit> get_unit(const std::string &unit_name);

        /// @brief Get all compilation units from the job
        /// @return std::map<std::string, CompilationUnit>
        const std::map<std::string, std::shared_ptr<CompilationUnit>> &units() const;

        /// @brief Build the compilation job
        /// @param detatch Detatch and run the job in a seperate thread. Defaults is blocking
        /// @return True if successful, false otherwise. If detatch is true, this will always return true
        bool run_job(bool detatch = false);

        /// @brief Get all messages from the compilation job
        /// @return std::vector<std::shared_ptr<CompilerMessage>>
        std::vector<std::shared_ptr<CompilerMessage>> messages() const;

        /// @brief Get all warnings from the compilation job
        /// @return std::vector<CompilerWarning>
        std::vector<CompilerWarning> warnings() const;

        /// @brief Get all errors from the compilation job
        /// @return std::vector<CompilerError>
        std::vector<CompilerError> errors() const;

        /// @brief Get all infos from the compilation job
        /// @return std::vector<CompilerInfo>
        std::vector<CompilerInfo> infos() const;

        /// @brief Get the progress of the compilation job
        /// @return 0-100 progress
        uint8_t progress() const;

        bool success() const;

    private:
        std::map<std::string, std::shared_ptr<CompilationUnit>> m_units;
        std::atomic<uint8_t> m_progress;
        std::string m_output_file;
        bool run_job_internal();
    };
}

#endif // _JCC_COMPILE_HPP_