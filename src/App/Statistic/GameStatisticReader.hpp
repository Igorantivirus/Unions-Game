#pragma once

#include "GameStatistic.hpp"

#include <SDLWrapper/FileWorker.hpp>
#include <SDL3/SDL_log.h>

#include <sstream>
#include <string_view>

namespace statistic::reader
{

    inline bool readAllGameStatistic(AllGameStatistic &stat, const std::string_view path)
    {
        stat.gameStatistic.clear();

        sdl3::FileWorker read(path, sdl3::FileWorkerMode::read | sdl3::FileWorkerMode::binary);
        if (!read.isOpen())
        {
            SDL_Log("GameStatisticReader: couldn't open %s", std::string(path).c_str());
            return false;
        }

        const std::string file = read.readAll();
        if (file.empty())
            return false;

        pugi::xml_document doc;
        const pugi::xml_parse_result res = doc.load_string(file.c_str());
        if (!res)
        {
            SDL_Log("GameStatisticReader: xml parse error at offset %u", (unsigned)res.offset);
            return false;
        }

        const pugi::xml_node root = doc.child("root");
        if (!root)
            return false;

        for (const pugi::xml_node gameNode : root.children("game"))
        {
            GameStatistic gs;
            gs.stringID = gameNode.attribute("id").as_string();
            gs.name = gameNode.attribute("name").as_string();

            const pugi::xml_node statNode = gameNode.child("statistic");
            if (!gs.loadFromStatistic(statNode))
                continue;

            stat.gameStatistic.push_back(std::move(gs));
        }

        return !stat.gameStatistic.empty();
    }

    inline bool writeAllGameStatistic(const AllGameStatistic &stat, const std::string_view path)
    {
        pugi::xml_document doc;
        pugi::xml_node root = doc.append_child("root");

        for (const auto &gs : stat.gameStatistic)
        {
            pugi::xml_node gameNode = root.append_child("game");
            gameNode.append_attribute("id").set_value(gs.stringID.c_str());
            gameNode.append_attribute("name").set_value(gs.name.c_str());

            pugi::xml_node statNode = gameNode.append_child("statistic");

            char bestTime[6] = {};
            bestTime[0] = static_cast<char>('0' + (gs.time.minuts / 10));
            bestTime[1] = static_cast<char>('0' + (gs.time.minuts % 10));
            bestTime[2] = ':';
            bestTime[3] = static_cast<char>('0' + (gs.time.seconds / 10));
            bestTime[4] = static_cast<char>('0' + (gs.time.seconds % 10));
            bestTime[5] = '\0';

            statNode.append_attribute("bestTime").set_value(bestTime);
            statNode.append_attribute("record").set_value(gs.record);
            statNode.append_attribute("countGames").set_value(gs.gameCount);
        }

        std::stringstream outStream;

        std::string out;
        out.reserve(1024);
        
        doc.save(outStream);

        out = outStream.str();

        sdl3::FileWorker write(path, sdl3::FileWorkerMode::write | sdl3::FileWorkerMode::binary);
        if (!write.isOpen())
        {
            SDL_Log("GameStatisticReader: couldn't write %s", std::string(path).c_str());
            return false;
        }
        return write.write(out);
    }

}
