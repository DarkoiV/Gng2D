#include "Gng2D/components/sprite.hpp"
#include "Gng2D/commons/args_vector.hpp"
#include "Gng2D/commons/assert.hpp"
#include "Gng2D/commons/repository.hpp"
#include "util_macros.hpp"
#include <entt/entt.hpp>

using Gng2D::Sprite;

std::optional<Sprite> Sprite::fromArgs(const Gng2D::ArgsVector& args, entt::registry::context&)
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

    if (not sprite.texture) return std::nullopt;
    else return sprite;
}
