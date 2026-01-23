#pragma once

#include <Core/Time.hpp>

namespace statistic
{

struct GameStatistic
{
    std::string stringID;
    std::string name;

    core::Time time{};
    unsigned int record{};
    unsigned int gameCount{};

    void reset()
    {
        time = core::Time{};
        record = 0;
        gameCount = 0;
    }
};

} // namespace statistic
