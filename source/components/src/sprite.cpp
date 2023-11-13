#include "Gng2D/components/sprite.hpp"
#include "Gng2D/commons/args_vector.hpp"
#include "Gng2D/commons/assert.hpp"
#include "Gng2D/commons/repository.hpp"
#include <entt/entt.hpp>

using Gng2D::Sprite;

Sprite Sprite::fromArgs(const Gng2D::ArgsVector& args)
{
    using namespace entt::literals;
    Gng2D::Sprite sprite;
    for (auto&& [id, arg]: args)
    {
        switch (id)
        {
        case "spriteId"_hs:
            {
                auto data = arg.try_cast<Gng2D::StringHash>();
                GNG2D_ASSERT(data, "spriteId has to be StringHash");
                sprite = Repository::getSprite(*data);
            }
            break;
        case "spriteName"_hs:
            {
                auto data = arg.try_cast<std::string>();
                GNG2D_ASSERT(data, "spriteName has to be std::string");
                sprite = Repository::getSprite(*data);
            }
            break;
        }
    }

    return sprite;
}
