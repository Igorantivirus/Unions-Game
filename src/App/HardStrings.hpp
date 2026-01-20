#pragma once

#include <string_view>

namespace names
{
constexpr const std::string_view mainIco = "ico.png";
constexpr const std::string_view statisticFile = "stat.xml";
constexpr const std::string_view windowName = "Объединялы";

} // namespace names

namespace assets
{
constexpr const std::string_view packages = "objects";
constexpr const std::string_view fontPath = "fonts/fonts.txt";

} // namespace assets

namespace ui::mainMenu
{
constexpr const std::string_view file = "ui/MainMenu.html";
constexpr const std::string_view exitB = "exit-b";
constexpr const std::string_view startB = "start-b";
constexpr const std::string_view settsB = "setts-b";
} // namespace ui::mainMenu

namespace ui::gameMenu
{
constexpr const std::string_view file = "ui/GameMenu.html";
constexpr const std::string_view backB = "back-b";
} // namespace ui::gameMenu

namespace ui::setsMenu
{
constexpr const std::string_view file = "ui/SetsMenu.html";
constexpr const std::string_view saveExitB = "save-exit-b";
constexpr const std::string_view saveThrowB = "throw-exit-b";
} // namespace ui::gameMenu