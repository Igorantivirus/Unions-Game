#pragma once

#include <string_view>

namespace names
{
    constexpr const std::string_view mainIco = "ico.png";
    constexpr const std::string_view statisticFile = "stat.xml";
    constexpr const std::string_view windowName = "Объединялы";

}

namespace assets::fonts
{
    constexpr const std::string_view fontPath = "fonts/fonts.txt";

}

namespace assets::ui
{
    constexpr const std::string_view mainMenuRmlFile = "ui/MainMenu.html";
    constexpr const std::string_view gameMenuRmlFile = "ui/GameMenu.html";
    constexpr const std::string_view setsMenuRmlFile = "ui/SetsMenu.html";
}

namespace assets::mainMenu
{
    constexpr const std::string_view exitB = "exit-b";
    constexpr const std::string_view startB = "start-b";
    constexpr const std::string_view settsB = "setts-b";
}

namespace assets::gameMenu
{
    constexpr const std::string_view backB = "back-b";
}

namespace assets::gameMenu
{
    constexpr const std::string_view saveExitB = "save-exit-b";
    constexpr const std::string_view saveThrowB = "throw-exit-b";
}