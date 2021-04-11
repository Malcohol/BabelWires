#pragma once

#include <string>
#include <string_view>

namespace babelwires
{

// Assume the XML API copies string contents if necessary.
struct toCStr
{
    toCStr(std::string_view view) : m_str(view) {}
    operator const char*() { return m_str.c_str(); }
    std::string m_str;
};

}
