#pragma once

#include <SDLWrapper/Math/VectorMath.hpp>
#include <memory>
#include <vector>

#include "PhysicObjects/Object.hpp"
#include "PhysicObjects/PhysicsShapes.hpp"

template <typename T>
using Simplex = std::vector<T>;

using V2Simplex = Simplex<sdl3::Vector2f>;

class CollisionMeneger
{
public:
    static bool GJK(const std::unique_ptr<IPhysicsShape> &A, const std::unique_ptr<IPhysicsShape> &B, V2Simplex &simplex)
    {
        // Начальное направление: от центра B к центру A
        sdl3::Vector2f d = A->getCenterPosition() - B->getCenterPosition();

        // Если центры совпали — берём любое направление
        if (sdl3::dot(d, d) < 1e-12f)
            d = {1.f, 0.f};

        simplex.reserve(3);

        // 1) Берём первую точку на разности Минковского
        sdl3::Vector2f a = supportMinkowski(A, B, d);
        simplex.push_back(a);

        // 2) Новое направление — к origin из точки a
        d = -a;

        // Ограничим число итераций (на практике GJK сходится быстро)
        for (int iter = 0; iter < 32; ++iter)
        {
            // Берём следующую точку в направлении d
            a = supportMinkowski(A, B, d);

            // Ключевой тест:
            // Если новая точка НЕ продвинулась в направлении d за origin,
            // значит origin не может оказаться внутри — пересечения нет.
            if (dot(a, d) <= 0.f)
                return false;

            simplex.push_back(a);

            // Если симплекс "накрыл" origin — пересечение есть.
            if (handleSimplex(simplex, d))
                return true;
        }

        // Если вдруг не сошлось за лимит (редко) — можно считать "нет",
        // либо "есть" в зависимости от твоих требований.
        return false;
    }

    static void EPA(Object &A, Object &B)
    {
        V2Simplex simplex;
        if(!GJK(A.shape, B.shape, simplex))
            return;


        // TODO
    }

private:
    // Перпендикуляр (на 90° влево)
    static sdl3::Vector2f perp(const sdl3::Vector2f &v)
    {
        return {-v.y, v.x};
    }

    // "Тройное произведение" для 2D, удобный трюк, чтобы получить вектор,
    // перпендикулярный a, направленный примерно "к origin", учитывая b.
    //
    // triple(a,b,c) = b*(a·c) - a*(b·c)
    // В GJK часто используют triple(ab, ao, ab), чтобы получить направление,
    // перпендикулярное AB в сторону origin.
    static sdl3::Vector2f tripleProduct(const sdl3::Vector2f &a, const sdl3::Vector2f &b, const sdl3::Vector2f &c)
    {
        const float ac = sdl3::dot(a, c);
        const float bc = sdl3::dot(b, c);
        return {b.x * ac - a.x * bc, b.y * ac - a.y * bc};
    }

    // Support точки разности Минковского
    static sdl3::Vector2f supportMinkowski(const std::unique_ptr<IPhysicsShape> &A, const std::unique_ptr<IPhysicsShape> &B, const sdl3::Vector2f &d)
    {
        // точка на A вдоль d минус точка на B вдоль -d
        return A->support(d) - B->support(-d);
    }

    // ====== Обработка симплекса ======
    //
    // simplex хранит точки так, что последняя добавленная — в конце.
    // Возвращает true если origin внутри симплекса => пересечение найдено.
    // И обновляет направление d для следующей итерации.

    static bool handleSimplex(std::vector<sdl3::Vector2f> &simplex, sdl3::Vector2f &d)
    {
        // O = (0,0) — начало координат. Мы проверяем, содержит ли симплекс origin.
        const sdl3::Vector2f O{0.f, 0.f};

        // A — последняя добавленная точка (ближе всего "в сторону d")
        const sdl3::Vector2f A = simplex.back();
        const sdl3::Vector2f AO = O - A;

        if (simplex.size() == 2)
        {
            // Линия: B ---- A
            const sdl3::Vector2f B = simplex[0];
            const sdl3::Vector2f AB = B - A;

            // Направление перпендикулярно AB в сторону origin.
            // Используем tripleProduct(AB, AO, AB) — даёт вектор, перпендикулярный AB.
            d = tripleProduct(AB, AO, AB);

            // Если вдруг AB и AO почти коллинеарны и d получилось ~0,
            // можно взять любой перпендикуляр к AB.
            if (sdl3::dot(d, d) < 1e-12f)
                d = perp(AB);

            return false;
        }

        // simplex.size() == 3
        // Треугольник: C, B, A (A — последняя)
        const sdl3::Vector2f B = simplex[1];
        const sdl3::Vector2f C = simplex[0];

        const sdl3::Vector2f AB = B - A;
        const sdl3::Vector2f AC = C - A;

        // Перпендикуляры к рёбрам, направленные наружу треугольника (в сторону origin)
        const sdl3::Vector2f abPerp = tripleProduct(AC, AB, AB); // перп. к AB "наружу"
        const sdl3::Vector2f acPerp = tripleProduct(AB, AC, AC); // перп. к AC "наружу"

        // Если origin находится "снаружи" относительно ребра AB — убираем точку C,
        // и продолжаем искать по направлению abPerp.
        if (dot(abPerp, AO) > 0.f)
        {
            // Удаляем C, оставляем линию B-A
            simplex.erase(simplex.begin()); // выкинули C
            d = abPerp;
            return false;
        }

        // Аналогично для ребра AC — убираем B
        if (dot(acPerp, AO) > 0.f)
        {
            // Удаляем B, оставляем линию C-A
            simplex.erase(simplex.begin() + 1); // выкинули B
            d = acPerp;
            return false;
        }

        // Если origin не снаружи ни относительно AB, ни относительно AC,
        // значит он внутри треугольника => пересечение найдено.
        return true;
    }
};