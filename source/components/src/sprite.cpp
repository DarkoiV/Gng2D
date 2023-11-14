#include "Gng2D/components/sprite.hpp"
#include "Gng2D/commons/args_vector.hpp"
#include "Gng2D/commons/assert.hpp"
#include "Gng2D/commons/comp_util_macros.hpp"
#include "Gng2D/commons/repository.hpp"
#include <entt/entt.hpp>

using Gng2D::Sprite;
using Arg = Gng2D::ComponentArg;
using entt::type_id;

const static Gng2D::ComponentArgs SPRITE_ARGS{
    Arg{.name          = "sprite",
        .description   = "Name or numeric hash of loaded sprite to display",
        .acceptedTypes = {type_id<Gng2D::StringHash>(), type_id<std::string>()},
        .required      = true}
};

Sprite Sprite::fromArgs(const Gng2D::ArgsVector& args)
{
    using namespace entt::literals;
    Gng2D::Sprite sprite;
    for (auto&& [id, arg]: args)
    {
        switch (id)
        {
        case "sprite"_hs:
            if (arg.type().info() == entt::type_id<StringHash>())
            {
                const auto& data = arg.cast<StringHash>();
                sprite           = Repository::getSprite(data);
            }
            else if (arg.type().info() == entt::type_id<std::string>())
            {
                const auto& data = arg.cast<std::string>();
                sprite           = Repository::getSprite(data);
            }
            else [[unlikely]]
            {
                GNG2D_ASSERT(true, "sprite has to be either StringHash or std::string, actual:",
                             arg.type().info().name());
            }
            break;
            UNKNOWN_ARG_CASE;
        }
    }

    return sprite;
}

const Gng2D::ComponentArgs* Sprite::argsInfo()
{
    return &SPRITE_ARGS;
}
