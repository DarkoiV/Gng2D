#pragma once
#include <cmath>
#include <entt/entt.hpp>
#include "Gng2D/core/log.hpp"
#include "Gng2D/components/colider.hpp"
#include "Gng2D/components/position.hpp"
#include "Gng2D/components/rotation.hpp"

namespace Gng2D
{
template<ColliderType A, ColliderType B>
struct OverlapSystem
{
    OverlapSystem(entt::registry& r)
        : reg(r)
    {};

    void operator()()
    {
        if constexpr (CircleColliderType<A> and CircleColliderType<B>)
        {
            CircleOnCircle();
        }

        else if constexpr ((CircleColliderType<A> and CapsuleColliderType<B>)
                        or (CapsuleColliderType<A> and CircleColliderType<B>))
        {
            CircleOnCapsule();
        }
        else 
        {
            []<bool flag = false>()
                {static_assert(flag, "Collision not yet supported");}();
        }
    }

    virtual void onOverlap(entt::entity, entt::entity) = 0;

protected:
    entt::registry& reg;

    void CircleOnCircle()
    {
        for (const auto& [entityA, posA, colliderA] : reg.view<Position, A>().each())
        {
            for (const auto& [entityB, posB, colliderB] : reg.view<Position, B>().each())
            {
                auto sqrDistance    = V2d::sqrDistance(posA, posB);
                auto sqrRadius      = std::pow(colliderA.radius + colliderB.radius, 2);
                if (sqrDistance < sqrRadius)
                {
                    onOverlap(entityA, entityB);
                    if (not reg.valid(entityA)) break;
                }
            }
        }
    }

    void CircleOnCapsule()
    {
        constexpr bool CIRCLE_IS_ENTITY_A = CircleColliderType<A>;
        using Circle    = std::conditional_t<CIRCLE_IS_ENTITY_A, A, B>;
        using Capsule   = std::conditional_t<CIRCLE_IS_ENTITY_A, B, A>;

        for (const auto& [entityA, posA, colliderA] : reg.view<Position, Circle>().each())
        {
            for (const auto& [entityB, posB, colliderB] : reg.view<Position, Capsule>().each())
            {
                double rotB = 0.0; 
                if (auto* rotComp = reg.try_get<Rotation>(entityB))
                {
                    rotB = rotComp->angle;
                }
                const V2d capsuleVector     = V2d::rot(rotB, colliderB.length);
                const V2d capsuleBegin      = posB - (capsuleVector/2);
                const V2d circleToCapsule   = V2d{posA.x - capsuleBegin.x, posA.y - capsuleBegin.y};
                const auto lenSqr           = colliderB.length * colliderB.length;

                const float t 
                    = std::max(0.0f, 
                               std::min(lenSqr, 
                                        V2d::dot(capsuleVector, circleToCapsule))) 
                        / lenSqr;

                const V2d closestCircleOnCapsule{capsuleBegin.x + t * capsuleVector.x,
                                                 capsuleBegin.y + t * capsuleVector.y};

                auto sqrDistance    = V2d::sqrDistance(posA, closestCircleOnCapsule);
                auto sqrRadius      = std::pow(colliderA.radius + colliderB.radius, 2);

                if (sqrDistance < sqrRadius)
                {
                    if constexpr (CIRCLE_IS_ENTITY_A) onOverlap(entityA, entityB);
                    else onOverlap(entityB, entityA);
                    if (not reg.valid(entityA)) break;
                }
            }
        }
    }
};

}

#define GNG2D_OVERLAP(NAME, ColliderA, ColliderB) \
    struct NAME : Gng2D::OverlapSystem<ColliderA, ColliderB> \
{ \
    NAME(entt::registry& r) : Gng2D::OverlapSystem<ColliderA, ColliderB>(r) {} \
    void onOverlap(entt::entity, entt::entity) override; \
} 

