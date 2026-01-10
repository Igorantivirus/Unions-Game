#pragma once

// sdl3::Vector2f rotateVector(const sdl3::Vector2f &v, float angle_rad)
// {
//     // Сохраняем исходные координаты
//     float original_x = v.x;
//     float original_y = v.y;

//     sdl3::Vector2f vector = v;

//     // Формулы поворота:
//     // x' = x*cos(angle) - y*sin(angle)
//     // y' = x*sin(angle) + y*cos(angle)
//     vector.x = original_x * std::cos(angle_rad) - original_y * std::sin(angle_rad);
//     vector.y = original_x * std::sin(angle_rad) + original_y * std::cos(angle_rad);
//     return vector;
// }

// static sdl3::Matrix3x3<float> inverse(const sdl3::Matrix3x3<float> &m)
// {
//     // Матрица:
//     // [ a  c  tx ]
//     // [ b  d  ty ]
//     // [ 0  0   1 ]

//     const float det = m.a * m.d - m.b * m.c;

//     // Если det == 0, матрица не обратима (например, scale = 0 по одной оси).
//     // Тут можно assert, или вернуть identity, или как тебе удобнее.
//     // Я сделаю безопасный вариант:
//     const float eps = 1e-8f;

//     if (std::fabs(det) < eps)
//     {
//         sdl3::Matrix3x3<float> res;
//         // T a, b, c, d, tx, ty;
//         res.a = 1.f;
//         res.b = 0.f;
//         res.c = 0.f;
//         res.d = 1.f;
//         res.tx = 0.f;
//         res.ty = 0.f;
//         return res;
//     }

//     const float invDet = 1.f / det;

//     sdl3::Matrix3x3<float> inv;

//     // Обратная к 2x2 (поворот/масштаб/сдвиг без tx/ty):
//     inv.a = m.d * invDet;
//     inv.b = -m.b * invDet;
//     inv.c = -m.c * invDet;
//     inv.d = m.a * invDet;

//     // Обратный перенос: -R^{-1} * t
//     inv.tx = -(inv.a * m.tx + inv.c * m.ty);
//     inv.ty = -(inv.b * m.tx + inv.d * m.ty);

//     return inv;
// }