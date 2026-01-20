#pragma once

#include <string>
#include <string_view>

namespace core
{

    static const std::string_view viewSubstr(const std::string& str, std::size_t off, std::size_t count = std::string::npos)
    {
        const std::string_view view = str;
        return view.substr(off, count);
    }
    static std::string_view viewSubstr(std::string& str, std::size_t off, std::size_t count = std::string::npos)
    {
        const std::string_view view = str;
        return view.substr(off, count);
    }

}