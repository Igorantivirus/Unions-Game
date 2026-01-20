#pragma once

namespace Config
{
inline constexpr const float PPM = 50.f; // Пикселей в метре
inline constexpr const float MPP = 1.f / PPM;

inline constexpr const float defaultDensity = 1.f;

inline constexpr const float defaultFrictionRect = 0.3f;
inline constexpr const float defaultFrictionEllipse = 0.5f;
inline constexpr const float defaultFrictionCircle = 0.5f;
} // namespace Config