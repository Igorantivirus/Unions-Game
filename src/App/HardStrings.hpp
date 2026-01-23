#pragma once

#include <string>
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
constexpr const std::string_view packagConf = "config.xml";
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

inline const std::string gameStats = "game-status";
inline const std::string timeLabel = "time";
inline const std::string pointsLabel = "points";
inline const std::string recordLabel = "record";

inline const std::string pauseB = "pause-b";
inline const std::string pauseOverlayId = "pause-overlay";
inline const std::string gameOverOverlayId = "gameover-overlay";

inline const std::string openClass = "open";
inline const std::string restartClass = "restart";
inline const std::string exitClass = "exit";
inline const std::string resumeClass = "resume";

inline const std::string deathLabel = "death";
inline const std::string maxDeathLabel = "maxdeath";

} // namespace ui::gameMenu

namespace ui::setsMenu
{
constexpr const std::string_view file = "ui/SetsMenu.html";
constexpr const std::string_view saveExitB = "save-exit-b";
constexpr const std::string_view saveThrowB = "throw-exit-b";
} // namespace ui::setsMenu
