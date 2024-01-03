#include "Gng2D/components/hoverable.hpp"
#include "Gng2D/components/meta/properties.hpp"
#include "util_macros.hpp"

using Gng2D::Hoverable;

std::optional<Hoverable> Hoverable::fromArgs(const ArgsVector& args, entt::registry::context&)
{
    Hoverable hoverable;
    for (auto&& [id, arg]: args)
    {
        switch (id)
        {
            COMP_ARG_CASE("width", hoverable.width);
            COMP_ARG_CASE("height", hoverable.height);
            UNKNOWN_ARG_CASE;
        }
    }
    return hoverable;
}

void Hoverable::registerData(MetaFactory factory)
{
    factory.data<&Hoverable::width>("width"_hash)
        .prop(ComponentFieldProperties::FIELD_TYPE, ComponentFieldType::FLOAT)
        .prop(ComponentFieldProperties::FIELD_NAME, "width");

    factory.data<&Hoverable::height>("height"_hash)
        .prop(ComponentFieldProperties::FIELD_TYPE, ComponentFieldType::FLOAT)
        .prop(ComponentFieldProperties::FIELD_NAME, "height");
}
