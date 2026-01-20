#pragma once

#include <sstream>

#include <SDL3/SDL_log.h>
#include <SDLWrapper/FileWorker.hpp>

#include <App/Statistic/AllGameStatistic.hpp>
#include <Core/Time.hpp>

#include "FullFileWorker.hpp"

namespace IO
{

inline bool readAllGameStatistic(statistic::AllGameStatistic &stat, const std::filesystem::path &path)
{
    stat.gameStatistic.clear();

    pugi::xml_document doc;
    if (auto res = doc.load_string(IO::readAllFile(path).c_str()); !res)
    {
        SDL_Log("GameStatisticReader: xml parse error at offset %u", (unsigned)res.offset);
        return false;
    }

    const pugi::xml_node root = doc.child("root");
    if (!root)
        return false;

    for (const pugi::xml_node gameNode : root.children("game"))
    {
        const pugi::xml_node statNode = gameNode.child("statistic");
        if (!statNode)
            continue;

        statistic::GameStatistic gs;

        gs.stringID = gameNode.attribute("id").as_string();
        gs.name = gameNode.attribute("name").as_string();
        gs.record = statNode.attribute("record").as_uint();
        gs.gameCount = statNode.attribute("countGames").as_uint();
        gs.time = core::Time::fromString(statNode.attribute("bestTime").as_string());

        stat.gameStatistic.push_back(std::move(gs));
    }

    return !stat.gameStatistic.empty();
}

inline bool writeAllGameStatistic(const statistic::AllGameStatistic &stat, const std::filesystem::path path)
{
    pugi::xml_document doc;
    pugi::xml_node root = doc.append_child("root");

    for (const auto &gs : stat.gameStatistic)
    {
        pugi::xml_node gameNode = root.append_child("game");
        pugi::xml_node statNode = gameNode.append_child("statistic");

        gameNode.append_attribute("id").set_value(gs.stringID.c_str());
        gameNode.append_attribute("name").set_value(gs.name.c_str());
        statNode.append_attribute("bestTime").set_value(core::Time::to6String(gs.time).data());
        statNode.append_attribute("record").set_value(gs.record);
        statNode.append_attribute("countGames").set_value(gs.gameCount);
    }

    std::stringstream outStream;
    doc.save(outStream);

    return IO::writeAllFile(path, outStream.str());
}

} // namespace IO
