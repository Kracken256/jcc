#include "preprocessor.hpp"

std::string jcc::Preprocessor::preprocess(const std::string &source)
{
    /// TODO: Implement this function.
    // wrap the entire source in a namespace for sanity
    return "namespace jxx {" + source + "\n}\n";
}