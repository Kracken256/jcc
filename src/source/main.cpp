#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include <memory>
#include "lexer.hpp"
#include "compile.hpp"
#include <algorithm>

using namespace jcc;

typedef std::vector<std::string> StringVector;

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

bool parse_arguments(StringVector args, JccMode &mode)
{
    if (args.size() < 1)
    {
        std::cerr << "jcc: argument error: no arguments specified" << std::endl;
        return false;
    }

    args = StringVector(args.begin() + 1, args.end());

    for (auto it = args.begin(); it != args.end(); ++it)
    {
        if (*it == "-o")
        {
            if (it + 1 == args.end())
            {
                std::cerr << "jcc: error: no output file specified" << std::endl;
                return true;
            }

            if (mode.output_file != "")
            {
                std::cerr << "jcc: error: multiple output files specified" << std::endl;
                return false;
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
            if (!std::filesystem::exists(*it))
            {
                std::cerr << "jcc: error: file '" << *it << "' does not exist" << std::endl;
                return false;
            }
            mode.input_files.push_back(*it);
        }
    }

    if (mode.input_files.size() == 0)
    {
        std::cerr << "jcc: error: no input files specified" << std::endl;
        return false;
    }

    if (mode.output_file == "")
    {
        bool found = false;
        for (auto flag : mode.flags)
        {
            if (flag == JccModeFlags::Object)
            {
                found = true;
                break;
            }
        }
        if (found)
        {
            mode.output_file = "a.out";
        }
        else
        {
            mode.output_file = "a.cpp";
        }
    }

    // check if duplicate flags
    std::sort(mode.flags.begin(), mode.flags.end());

    const auto duplicate = std::adjacent_find(mode.flags.begin(), mode.flags.end());
    if (duplicate != mode.flags.end())
    {
        std::cerr << "jcc: error: duplicate flag '" << flag_names[*duplicate] << "'" << std::endl;
        return false;
    }

    std::vector<std::string> input_files_copy = mode.input_files;
    std::sort(input_files_copy.begin(), input_files_copy.end());
    const auto duplicate_file = std::adjacent_find(input_files_copy.begin(), input_files_copy.end());
    if (duplicate_file != input_files_copy.end())
    {
        std::cerr << "jcc: error: duplicate input file '" << *duplicate_file << "'" << std::endl;
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

    CompilationJob job;

    for (auto file : mode.input_files)
    {
        auto unit = std::make_shared<CompilationUnit>();
        unit->add_file(file);
        job.add_unit(file, unit);
    }

    job.run_job();

    // print all messages
    for (auto message : job.messages())
    {
        std::cerr << message->ansi_message() << std::endl;
    }

    if (!job.success())
    {
        std::cerr << "jcc: error: compilation failed" << std::endl;
        return 1;
    }

    return 0;
}