#include "preprocessor.hpp"

std::string jcc::Preprocessor::preprocess(const std::string &source)
{
    /// TODO: Implement this function.
    std::string result;

    // wrap the entire source in a namespace for sanity
    result += "namespace jxx {" + source + "\n}\n";

    return result;
}