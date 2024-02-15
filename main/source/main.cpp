#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include <memory>
#include <thread>
#include <openssl/crypto.h>
#include <mutex>
#include <functional>
#include "compile.hpp"
#include <algorithm>

using namespace jcc;

typedef std::vector<std::string> StringVector;

std::vector<std::shared_ptr<std::mutex>> mutexes;

// OpenSSL locking function callback
void openssl_locking_function(int mode, int n, const char *file, int line)
{
    (void)(file);
    (void)(line);

    if (mode & CRYPTO_LOCK)
    {
        mutexes[n]->lock();
    }
    else
    {
        mutexes[n]->unlock();
    }
}

// OpenSSL thread ID callback
unsigned long openssl_thread_id_function()
{
    return std::hash<std::thread::id>()(std::this_thread::get_id());
}

enum JccModeFlags
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

std::map<JccModeFlags, std::string> flag_names = {
    {JccModeFlags::Verbose, "-v"},
    {JccModeFlags::Debug, "-g"},
    {JccModeFlags::OptimizeNone, "-O0"},
    {JccModeFlags::OptimizeLight, "-O1"},
    {JccModeFlags::OptimizeSpeed, "-O2"},
    {JccModeFlags::OptimizeAggressive, "-O3"},
    {JccModeFlags::OptimizeSize, "-Os"},
    {JccModeFlags::Object, "-c"},
    {JccModeFlags::TranslateOnly, "-S"},
};

struct JccMode
{
    std::vector<std::string> input_files;
    std::string output_file;
    std::vector<JccModeFlags> flags;
};

static void print_error(const std::string &message)
{
    std::cerr << "\x1b[37;49;1mjcc:\x1b[0m \x1b[31;49;1mError:\x1b[0m " << message << std::endl;
}

static void print_help()
{
    std::cout << "Usage: jcc [options] file.j [file.j ...]\n"
                 "Options:\n"
                 "  -o <file>  Place the output into <file>\n"
                 "  -v         Verbose output\n"
                 "  -g         Generate debug information\n"
                 "  -O0        Do not optimize\n"
                 "  -O1        Do basic optimizations\n"
                 "  -O2        Optimize (default)\n"
                 "  -O3        Optimize aggressively\n"
                 "  -Os        Optimize for size\n"
                 "  -c         Compile only, do not link\n"
                 "  -S         Translate only, do not compile or link\n\n";

    std::cout << "Copyright (C) 2024 Wesley C. Jones. All rights "
                 "reserved.\n";
}

bool parse_arguments(StringVector args, JccMode &mode)
{
    if (args.size() < 1)
    {
        // std::cerr << "jcc: argument error: no arguments specified" << std::endl;
        print_error("no arguments specified");
        return false;
    }

    args = StringVector(args.begin() + 1, args.end());

    for (auto it = args.begin(); it != args.end(); ++it)
    {
        if (*it == "-h" || *it == "--help")
        {
            print_help();
            exit(0);
        }
        else if (*it == "-o" && mode.output_file != "")
        {
            print_error("multiple output files specified");
            return false;
        }
        else if (*it == "-o")
        {
            if (it + 1 == args.end())
            {
                print_error("no output file specified");
                return true;
            }

            mode.output_file = *(++it);
        }
        else if (*it == "-v")
        {
            mode.flags.push_back(JccModeFlags::Verbose);
        }
        else if (*it == "-g")
        {
            mode.flags.push_back(JccModeFlags::Debug);
        }
        else if (*it == "-O0")
        {
            mode.flags.push_back(JccModeFlags::OptimizeNone);
        }
        else if (*it == "-O1")
        {
            mode.flags.push_back(JccModeFlags::OptimizeLight);
        }
        else if (*it == "-O2") // Default
        {
            mode.flags.push_back(JccModeFlags::OptimizeSpeed);
        }
        else if (*it == "-O3")
        {
            mode.flags.push_back(JccModeFlags::OptimizeAggressive);
        }
        else if (*it == "-Os")
        {
            mode.flags.push_back(JccModeFlags::OptimizeSize);
        }
        else if (*it == "-c")
        {
            mode.flags.push_back(JccModeFlags::Object);
        }
        else if (*it == "-S")
        {
            mode.flags.push_back(JccModeFlags::TranslateOnly);
        }
        else
        {
            if (!it->ends_with(".j"))
            {
                print_error("file '" + *it + "' is not a j source file. j source files must end with '.j'");
                return false;
            }

            if (!std::filesystem::exists(*it))
            {
                print_error("file '" + *it + "' does not exist");
                return false;
            }

            if (std::filesystem::is_directory(*it))
            {
                print_error("file '" + *it + "' is a directory. can not compile directories");
                return false;
            }

            if (!std::filesystem::is_regular_file(*it))
            {
                print_error("file '" + *it + "' is not a regular file");
                return false;
            }

            mode.input_files.push_back(*it);
        }
    }

    if (mode.input_files.size() == 0)
    {
        print_error("no input files specified");
        return false;
    }

    if (mode.output_file == "")
    {
        bool found = false;
        for (auto flag : mode.flags)
        {
            if (flag == JccModeFlags::TranslateOnly)
            {
                found = true;
                break;
            }
        }
        if (found)
        {
            mode.output_file = "out.cpp";
        }
        else
        {
            mode.output_file = "a.out";
        }
    }

    // check if duplicate flags
    std::sort(mode.flags.begin(), mode.flags.end());

    const auto duplicate = std::adjacent_find(mode.flags.begin(), mode.flags.end());
    if (duplicate != mode.flags.end())
    {
        print_error("duplicate flag '" + flag_names[*duplicate] + "'");
        return false;
    }

    std::vector<std::string> input_files_copy = mode.input_files;
    std::sort(input_files_copy.begin(), input_files_copy.end());
    const auto duplicate_file = std::adjacent_find(input_files_copy.begin(), input_files_copy.end());
    if (duplicate_file != input_files_copy.end())
    {
        print_error("duplicate input file '" + *duplicate_file + "'");
        return false;
    }

    return true;
}

int main(int argc, char **argv)
{
    StringVector args(argv, argv + argc);
    JccMode mode;

    if (!parse_arguments(args, mode))
    {
        return 1;
    }

    // Initialize mutexes
    int num_locks = CRYPTO_num_locks();
    mutexes.resize(num_locks);

    // Set up OpenSSL thread callbacks
    CRYPTO_set_locking_callback(openssl_locking_function);
    CRYPTO_set_id_callback(openssl_thread_id_function);

    CompilationJob job;

    auto unit = std::make_unique<CompilationUnit>();
    unit->set_output_file(mode.output_file);

    for (auto file : mode.input_files)
    {
        unit->add_file(file);
    }
    for (auto flag : mode.flags)
    {
        switch (flag)
        {
        case JccModeFlags::Verbose:
            unit->add_flag(CompileFlag::Verbose);
            break;
        case JccModeFlags::Debug:
            unit->add_flag(CompileFlag::Debug);
            break;
        case JccModeFlags::OptimizeNone:
            unit->add_flag(CompileFlag::OptimizeNone);
            break;
        case JccModeFlags::OptimizeLight:
            unit->add_flag(CompileFlag::OptimizeLight);
            break;
        case JccModeFlags::OptimizeSpeed:
            unit->add_flag(CompileFlag::OptimizeSpeed);
            break;
        case JccModeFlags::OptimizeAggressive:
            unit->add_flag(CompileFlag::OptimizeAggressive);
            break;
        case JccModeFlags::OptimizeSize:
            unit->add_flag(CompileFlag::OptimizeSize);
            break;
        case JccModeFlags::Object:
            unit->add_flag(CompileFlag::Object);
            break;
        case JccModeFlags::TranslateOnly:
            unit->add_flag(CompileFlag::TranslateOnly);
            break;

        default:
            break;
        }
    }

    job.set_output_file(mode.output_file);
    job.add_unit(mode.output_file, std::move(unit));

    job.run_job();

    // print all messages
    for (auto message : job.messages())
    {
        std::cerr << message->ansi_message() << std::endl;
    }

    if (!job.success())
    {
        print_error("compilation failed");
        return 1;
    }

    return 0;
}