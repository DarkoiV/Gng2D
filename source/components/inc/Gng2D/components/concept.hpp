#pragma once
#include "Gng2D/commons/args_vector.hpp"
#include "Gng2D/commons/component_args.hpp"

namespace Gng2D::Concepts {
template <typename C>
concept Component = requires(const C c, const Gng2D::ArgsVector& av) {
    {
        C::fromArgs(av)
    } -> std::same_as<C>;
    {
        C::argsInfo()
    } -> std::same_as<const Gng2D::ComponentArgs*>;
};
}
