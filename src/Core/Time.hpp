#pragma once

#include <SDL3/SDL_log.h>
#include <array>
#include <cstdint>
#include <string>

namespace core
{

struct Time
{
    std::uint8_t minuts{};
    std::uint8_t seconds{};

    bool isValid() const
    {
        return minuts < 60 && seconds < 60;
    }

    static Time fromSeconds(const float seconds)
    {
        unsigned int s = static_cast<unsigned int>(seconds);
        Time res;
        res.minuts = s / 60;
        res.seconds = s % 60;
        return res;
    }
    static Time fromString(const std::string &string)
    {
        Time res;
        if (string.size() != 5)
            return res;

        res.minuts = (string[0] - '0') * 10;
        res.minuts += string[1] - '0';
        res.seconds = (string[3] - '0') * 10;
        res.seconds += string[4] - '0';

        return res;
    }

    static std::string toString(const Time &time)
    {
        std::string res;
        res.resize(5, 0);
        res[0] = time.minuts / 10 + '0';
        res[1] = time.minuts % 10 + '0';
        res[2] = ':';
        res[3] = time.seconds / 10 + '0';
        res[4] = time.seconds % 10 + '0';
        return res;
    }
    static std::array<char, 6> to6String(const Time &time)
    {
        std::array<char, 6> res;
        res[0] = time.minuts / 10 + '0';
        res[1] = time.minuts % 10 + '0';
        res[2] = ':';
        res[3] = time.seconds / 10 + '0';
        res[4] = time.seconds % 10 + '0';
        res[5] = '\0';
        return res;
    }

    bool operator==(const Time &other)
    {
        return minuts == other.minuts && seconds == other.seconds;
    }
    bool operator!=(const Time &other)
    {
        return !this->operator==(other);
    }

    bool operator<(const Time &other)
    {
        if (minuts != other.minuts)
            return minuts < other.minuts;
        return seconds > other.seconds;
    }
    bool operator>(const Time &other)
    {
        if (minuts != other.minuts)
            return minuts > other.minuts;
        return seconds > other.seconds;
    }

    bool operator<=(const Time &other)
    {
        return !this->operator>(other);
    }
    bool operator>=(const Time &other)
    {
        return !this->operator<(other);
    }
};

} // namespace core