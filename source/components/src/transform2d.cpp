#include "Gng2D/components/transform2d.hpp"
#include "Gng2D/commons/args_vector.hpp"
#include "Gng2D/commons/comp_util_macros.hpp"
#include <entt/entt.hpp>

using Gng2D::Transform2d;

Transform2d Transform2d::fromArgs(const Gng2D::ArgsVector& args)
{
    using namespace entt::literals;
    Gng2D::Transform2d transform;
    for (auto&& [id, arg]: args)
    {
        switch (id)
        {
            COMP_ARG_CASE("x", float, transform.x);
            COMP_ARG_CASE("y", float, transform.y);
            COMP_ARG_CASE("layerMain", int8_t, transform.layer.main);
            COMP_ARG_CASE("layerSub", int8_t, transform.layer.sub);
        }
    }

    return transform;
}
