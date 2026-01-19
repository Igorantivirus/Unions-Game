#pragma once

#include <cstdint>
#include <string>

namespace statistic
{

struct Time
{
    std::uint8_t minuts{};
    std::uint8_t seconds{};

    static Time fromSeconds(const float seconds)
    {
        unsigned int s = static_cast<unsigned int>(seconds);
        Time res;
        res.minuts = s / 60 / 10;
        res.minuts = s / 60 % 10;
        res.seconds = s % 60 / 10;
        res.seconds = s % 60 % 10;
        return res;
    }

    static std::string toString(const Time& time)
    {
        std::string res;
        res[0] = time.minuts / 10 + '0';
        res[1] = time.minuts % 10 + '0';
        res[2] = ':';
        res[3] = time.seconds / 10 + '0';
        res[4] = time.seconds % 10 + '0';
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

} // namespace statistic