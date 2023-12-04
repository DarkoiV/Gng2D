#include "Gng2D/components/collider.hpp"
#include "Gng2D/components/meta/properties.hpp"
#include "util_macros.hpp"

using namespace Gng2D;
using namespace entt::literals;

std::optional<BoxCollider> BoxCollider::fromArgs(const ArgsVector& args, entt::registry::context&)
{
    BoxCollider collider;
    for (auto&& [id, arg]: args)
    {
        switch (id)
        {
            COMP_ARG_CASE("width", collider.width);
            COMP_ARG_CASE("height", collider.height);
        }
    }

    return collider;
}

void BoxCollider::registerData(MetaFactory mf)
{
    mf.data<&BoxCollider::width>("width"_hs)
        .prop(ComponentFieldProperties::FIELD_TYPE, ComponentFieldType::INTEGER)
        .prop(ComponentFieldProperties::FIELD_NAME, "width");

    mf.data<&BoxCollider::height>("height"_hs)
        .prop(ComponentFieldProperties::FIELD_TYPE, ComponentFieldType::INTEGER)
        .prop(ComponentFieldProperties::FIELD_NAME, "height");
}
