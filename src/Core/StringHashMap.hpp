#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

namespace core
{

// Определяем кастомный хешер, работающий с string и string_view
struct string_hash
{
    using is_transparent = void; // Ключевая метка!

    size_t operator()(std::string_view str) const
    {
        return std::hash<std::string_view>{}(str);
    }

    size_t operator()(const std::string &str) const
    {
        return std::hash<std::string>{}(str);
    }
};

// Определяем кастомный компаратор
struct string_equal
{
    using is_transparent = void;

    bool operator()(std::string_view a, std::string_view b) const
    {
        return a == b;
    }
};

template <typename T>
using StringHashMap = std::unordered_map<std::string, T, string_hash, string_equal>;

} // namespace core