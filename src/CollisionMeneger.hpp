#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include <vector>

#include <SDLWrapper/Math/VectorMath.hpp>

#include "EngineConfig.hpp"
#include "PhysicObjects/Object.hpp"
#include "PhysicObjects/PhysicsShapes.hpp"

template <typename T>
using Simplex = std::vector<T>;

using V2Simplex = Simplex<sdl3::Vector2f>;

class CollisionMeneger
{
private:
    struct EPAResult
    {
        bool hit = false;
        sdl3::Vector2f normal{0.f, 0.f}; // from B to A
        float depth = 0.f;

        sdl3::Vector2f pointA{0.f, 0.f};
        sdl3::Vector2f pointB{0.f, 0.f};
    };

    struct EPAEdge
    {
        int index = 0; // next vertex after the edge is index, previous is index-1
        sdl3::Vector2f normal{0.f, 0.f};
        float distance = 0.f; // distance from origin along normal
    };

public:
    static bool GJK(const std::unique_ptr<IPhysicsShape> &A, const std::unique_ptr<IPhysicsShape> &B, V2Simplex &simplex)
    {
        // initial direction between centers
        sdl3::Vector2f d = A->getCenterPosition() - B->getCenterPosition();
        if (sdl3::dot(d, d) < engine::cfg::epsilon)
            d = {1.f, 0.f};

        simplex.clear();
        simplex.reserve(3);

        sdl3::Vector2f a = supportMinkowski(A, B, d);
        simplex.push_back(a);
        d = -a;

        for (int iter = 0; iter < engine::cfg::gjkMaxIterations; ++iter)
        {
            a = supportMinkowski(A, B, d);
            if (sdl3::dot(a, d) <= 0.f)
                return false;

            simplex.push_back(a);
            if (handleSimplex(simplex, d))
                return true;
        }

        return false;
    }

    static void EPA(Object &A, Object &B)
    {
        V2Simplex simplex;
        if (!GJK(A.shape, B.shape, simplex))
            return;

        EPAResult res = runEPA(A.shape, B.shape, simplex);
        if (!res.hit)
            return;

        const PhysicBodyType typeA = A.body.getType();
        const PhysicBodyType typeB = B.body.getType();
        if (typeA != PhysicBodyType::Dynamic && typeB != PhysicBodyType::Dynamic)
            return;

        const float depth = std::max(0.f, res.depth - engine::cfg::correctionSlop);
        const float corrMag = std::min(depth * engine::cfg::correctionPercent, engine::cfg::correctionMax);
        const sdl3::Vector2f correction = res.normal * corrMag;

        auto &shA = A.shape->getShape();
        auto &shB = B.shape->getShape();

        sdl3::Vector2f posA = shA.getPosition();
        sdl3::Vector2f posB = shB.getPosition();

        if (typeA == PhysicBodyType::Dynamic && typeB == PhysicBodyType::Dynamic)
        {
            posA += correction * 0.5f;
            posB -= correction * 0.5f;
        }
        else if (typeA == PhysicBodyType::Dynamic)
        {
            posA += correction;
        }
        else // typeB dynamic
        {
            posB -= correction;
        }

        shA.setPosition(posA);
        shB.setPosition(posB);

        resolveImpulse(A, B, res);
    }

private:
    static void resolveImpulse(Object &objA, Object &objB, const EPAResult &res)
    {
        PhysicBody &A = objA.body;
        PhysicBody &B = objB.body;

        const float invMassA = A.getInvMass();
        const float invMassB = B.getInvMass();
        const float invMassSum = invMassA + invMassB;
        if (invMassSum <= engine::cfg::epsilon)
            return;

        const sdl3::Vector2f n = res.normal;
        const sdl3::Vector2f contact = (res.pointA + res.pointB) * 0.5f;
        const sdl3::Vector2f centerA = objA.shape->getCenterPosition();
        const sdl3::Vector2f centerB = objB.shape->getCenterPosition();

        const sdl3::Vector2f rA = contact - centerA;
        const sdl3::Vector2f rB = contact - centerB;

        const sdl3::Vector2f vA = A.getVelocity() + perp(rA) * A.getAngularVelocity();
        const sdl3::Vector2f vB = B.getVelocity() + perp(rB) * B.getAngularVelocity();
        const sdl3::Vector2f rv = vA - vB;

        const float velAlongNormal = sdl3::dot(rv, n);
        if (velAlongNormal > 0.f)
            return;

        float restitution = std::min(A.getRestitution(), B.getRestitution());
        if (-velAlongNormal < engine::cfg::bounceStopSpeed)
            restitution = 0.f;

        const float raCrossN = cross(rA, n);
        const float rbCrossN = cross(rB, n);
        float denom = invMassSum +
                      (raCrossN * raCrossN) * A.getInvInertia() +
                      (rbCrossN * rbCrossN) * B.getInvInertia();
        if (denom <= engine::cfg::epsilon)
            return;

        float j = -(1.f + restitution) * velAlongNormal;
        j /= denom;

        const sdl3::Vector2f impulse = n * j;

        A.applyImpulseAtPoint(impulse, contact, centerA);
        B.applyImpulseAtPoint(-impulse, contact, centerB);

        sdl3::Vector2f t = rv - n * sdl3::dot(rv, n);
        const float tLen2 = sdl3::dot(t, t);
        if (tLen2 > engine::cfg::epsilon)
        {
            t *= 1.0f / std::sqrt(tLen2);

            const float raCrossT = cross(rA, t);
            const float rbCrossT = cross(rB, t);
            float denomF = invMassSum +
                           (raCrossT * raCrossT) * A.getInvInertia() +
                           (rbCrossT * rbCrossT) * B.getInvInertia();
            if (denomF > engine::cfg::epsilon)
            {
                float jt = -sdl3::dot(rv, t);
                jt /= denomF;

                const float muS = 0.5f * (A.getStaticFriction() + B.getStaticFriction());
                const float muD = 0.5f * (A.getDynamicFriction() + B.getDynamicFriction());

                sdl3::Vector2f frictionImpulse;
                if (std::abs(jt) < j * muS)
                    frictionImpulse = t * jt;
                else
                    frictionImpulse = t * (-j * muD * (jt < 0.f ? -1.f : 1.f));

                A.applyImpulseAtPoint(frictionImpulse, contact, centerA);
                B.applyImpulseAtPoint(-frictionImpulse, contact, centerB);
            }
        }
    }

    static sdl3::Vector2f perp(const sdl3::Vector2f &v)
    {
        return {-v.y, v.x};
    }

    static sdl3::Vector2f tripleProduct(const sdl3::Vector2f &a, const sdl3::Vector2f &b, const sdl3::Vector2f &c)
    {
        const float ac = sdl3::dot(a, c);
        const float bc = sdl3::dot(b, c);
        return {b.x * ac - a.x * bc, b.y * ac - a.y * bc};
    }

    static sdl3::Vector2f supportMinkowski(const std::unique_ptr<IPhysicsShape> &A,
                                           const std::unique_ptr<IPhysicsShape> &B,
                                           const sdl3::Vector2f &d)
    {
        return A->support(d) - B->support(-d);
    }

    static bool handleSimplex(std::vector<sdl3::Vector2f> &simplex, sdl3::Vector2f &d)
    {
        const sdl3::Vector2f O{0.f, 0.f};
        const sdl3::Vector2f A = simplex.back();
        const sdl3::Vector2f AO = O - A;

        if (simplex.size() == 2)
        {
            const sdl3::Vector2f B = simplex[0];
            const sdl3::Vector2f AB = B - A;
            d = tripleProduct(AB, AO, AB);
            if (sdl3::dot(d, d) < engine::cfg::epsilon)
                d = perp(AB);
            return false;
        }

        const sdl3::Vector2f B = simplex[1];
        const sdl3::Vector2f C = simplex[0];

        const sdl3::Vector2f AB = B - A;
        const sdl3::Vector2f AC = C - A;

        const sdl3::Vector2f abPerp = tripleProduct(AC, AB, AB);
        const sdl3::Vector2f acPerp = tripleProduct(AB, AC, AC);

        if (sdl3::dot(abPerp, AO) > 0.f)
        {
            simplex.erase(simplex.begin()); // remove C
            d = abPerp;
            return false;
        }

        if (sdl3::dot(acPerp, AO) > 0.f)
        {
            simplex.erase(simplex.begin() + 1); // remove B
            d = acPerp;
            return false;
        }

        return true;
    }

    static float cross(const sdl3::Vector2f &a, const sdl3::Vector2f &b)
    {
        return a.x * b.y - a.y * b.x;
    }

    static EPAEdge findClosestEdge(const std::vector<sdl3::Vector2f> &polytope)
    {
        EPAEdge best;
        best.distance = std::numeric_limits<float>::infinity();

        const int n = static_cast<int>(polytope.size());
        for (int i = 0; i < n; ++i)
        {
            const int j = (i + 1) % n;
            const sdl3::Vector2f a = polytope[i];
            const sdl3::Vector2f b = polytope[j];

            const sdl3::Vector2f e = b - a;
            sdl3::Vector2f nrm = sdl3::normalize(perp(e));
            float dist = sdl3::dot(nrm, a);

            if (dist < 0.f)
            {
                dist = -dist;
                nrm = -nrm;
            }

            if (dist < best.distance)
            {
                best.distance = dist;
                best.normal = nrm;
                best.index = j;
            }
        }

        return best;
    }

    static EPAResult runEPA(const std::unique_ptr<IPhysicsShape> &A,
                            const std::unique_ptr<IPhysicsShape> &B,
                            std::vector<sdl3::Vector2f> polytope)
    {
        EPAResult out;
        if (polytope.size() < 3)
            return out;

        const float area2 = cross(polytope[1] - polytope[0], polytope[2] - polytope[0]);
        if (area2 < 0.f)
            std::swap(polytope[1], polytope[2]);

        for (int iter = 0; iter < engine::cfg::epaMaxIterations; ++iter)
        {
            const EPAEdge edge = findClosestEdge(polytope);

            const sdl3::Vector2f p = supportMinkowski(A, B, edge.normal);
            const float d = sdl3::dot(edge.normal, p);

            if ((d - edge.distance) < engine::cfg::epaTolerance)
            {
                out.hit = true;
                out.normal = edge.normal;
                out.depth = d;

                const sdl3::Vector2f centers = A->getCenterPosition() - B->getCenterPosition();
                if (sdl3::dot(out.normal, centers) < 0.f)
                    out.normal = -out.normal;

                out.pointA = A->support(out.normal);
                out.pointB = B->support(-out.normal);

                out.depth = std::abs(sdl3::dot(out.normal, out.pointA - out.pointB));
                return out;
            }

            polytope.insert(polytope.begin() + edge.index, p);
        }

        return out;
    }
};
