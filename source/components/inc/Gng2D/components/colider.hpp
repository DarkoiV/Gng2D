#pragma once
#include <type_traits>
#include <variant>

namespace Gng2D
{
struct CircleCollider
{
    float radius;
};

struct CapsuleCollider
{
    float radius;
    float length;
};

template<typename C>
concept CircleColliderType = std::is_base_of<CircleCollider, C>::value;

template<typename C>
concept CapsuleColliderType = std::is_base_of<CapsuleCollider, C>::value;

template<typename C>
concept ColliderType 
    = CapsuleColliderType<C> 
    or CircleColliderType<C>;

using Collider = std::variant<CircleCollider, CapsuleCollider>;
}

