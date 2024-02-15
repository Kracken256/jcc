#ifndef _JCC_PREPROCESSOR_HPP_
#define _JCC_PREPROCESSOR_HPP_

#include <string>
#include <stdexcept>

namespace jcc
{
    class PreprocessorException : public std::runtime_error
    {
    public:
        PreprocessorException(const std::string &message) : std::runtime_error(message) {}
    };

    class PreprocessorImportException : public PreprocessorException
    {
    public:
        PreprocessorImportException(const std::string &message) : PreprocessorException(message) {}
    };

    class PreprocessorTokenException : public PreprocessorException
    {
    public:
        PreprocessorTokenException(const std::string &message) : PreprocessorException(message) {}
    };

    class PreprocessorImportNotFoundException : public PreprocessorImportException
    {
    public:
        PreprocessorImportNotFoundException(const std::string &message) : PreprocessorImportException(message) {}
    };

    class PreprocessorImportCyclicException : public PreprocessorImportException
    {
    public:
        PreprocessorImportCyclicException(const std::string &message) : PreprocessorImportException(message) {}
    };

    class Preprocessor
    {
    public:
        /// @brief Preprocess the source code
        /// @param source The source code
        /// @return std::string
        /// @throw PreprocessorException
        static std::string preprocess(const std::string &source);

    private:
        Preprocessor() = delete;
        ~Preprocessor() = delete;
    };
}

#endif // !_JCC_PREPROCESSOR_HPP_