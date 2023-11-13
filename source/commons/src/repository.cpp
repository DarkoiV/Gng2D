#include "Gng2D/commons/repository.hpp"
#include "Gng2D/commons/args_vector.hpp"
#include "Gng2D/commons/assert.hpp"
#include "Gng2D/commons/log.hpp"
#include "Gng2D/components/input.hpp"
#include "Gng2D/components/relationship.hpp"
#include "Gng2D/components/sprite.hpp"
#include "Gng2D/components/transform2d.hpp"
#include "Gng2D/core/global.hpp"
#include <SDL2/SDL_image.h>

using Gng2D::Repository;

void Repository::loadSprite(const std::string& name,
                            const std::string& path,
                            const std::string& fileType)
{
    const auto fullPath       = GLOBAL::DATA_DIRECTORY + path + name + fileType;
    const auto spriteNameHash = entt::hashed_string{name.c_str()};
    if (sprites.contains(spriteNameHash))
    {
        if (spriteNames[spriteNameHash] == name) return LOG::WARN("Sprite already loaded");
        else
        {
            LOG::ERROR("Sprite collision", name, " has the same hash as ",
                       spriteNames[spriteNameHash]);
        }
        return;
    }

    Gng2D::Sprite loadedSprite;
    loadedSprite.texture = IMG_LoadTexture(GLOBAL::RENDERER, fullPath.c_str());
    if (not loadedSprite.texture) return LOG::ERROR("Failed to load", name, IMG_GetError());
    SDL_QueryTexture(loadedSprite.texture, nullptr, nullptr, &loadedSprite.srcRect.w,
                     &loadedSprite.srcRect.h);
    sprites[spriteNameHash] = loadedSprite;

    LOG::OK("Loaded sprite:", name);
}

Gng2D::Sprite Repository::getSprite(const entt::hashed_string::hash_type id)
{
    auto it = sprites.find(id);
    if (it != sprites.end())
    {
        return it->second;
    }

    LOG::ERROR("Sprite not found:", id);
    return Gng2D::Sprite{};
}

std::string Repository::getSpriteName(const StringHash hash)
{
    if (auto it = spriteNames.find(hash); it != spriteNames.end())
    {
        return it->second;
    }
    else [[unlikely]] return "ERROR";
}

static Gng2D::Sprite spriteConstructor(const Gng2D::ArgsVector& args)
{
    using namespace entt::literals;
    Gng2D::Sprite sprite;
    for (auto&& [id, arg]: args)
    {
        switch (id)
        {
        case "spriteId"_hs:
            auto data = arg.try_cast<Gng2D::StringHash>();
            GNG2D_ASSERT(data, "spriteId has to be StringHash");
            sprite = Repository::getSprite(*(Gng2D::StringHash*)data);
            break;
        }
    }

    return sprite;
}

static Gng2D::Transform2d transformCtor(const Gng2D::ArgsVector& args)
{
    using namespace entt::literals;
    Gng2D::Transform2d transform;
    for (auto&& [id, arg]: args)
    {
        switch (id)
        {
        case "x"_hs:
            {
                auto data = arg.try_cast<float>();
                GNG2D_ASSERT(data, "x has to be float");
                transform.x = *(float*)data;
                break;
            }
        case "y"_hs:
            {
                auto data = arg.try_cast<float>();
                GNG2D_ASSERT(data, "y has to be float");
                transform.y = *(float*)data;
            }
            break;
        }
    }

    return transform;
}

void Repository::registerDefaultComponents()
{
    using namespace entt::literals;
    // clang-format off
    registerComponent<InputListener>("InputListener")
        .data<&InputListener::actions>("actions"_hs)
        .data<&InputListener::signal>("signal"_hs);
    registerComponent<Children>("Children")
        .data<&Children::list>("childrenList"_hs);
    registerComponent<Parent>("Parent")
        .data<&Parent::id>("id"_hs);
    registerComponent<Sprite>("Sprite")
        .ctor<&spriteConstructor>();
    registerComponent<Transform2d>("Transform2d")
        .ctor<&transformCtor>()
        .ctor<float, float>()
        .data<&Transform2d::x>("x"_hs)
        .data<&Transform2d::y>("y"_hs)
        .data<&Transform2d::layer>("layer"_hs);
    // clang-format on
}

void Repository::freeResources()
{
    LOG::INFO("Deleting global assets");
    for (auto& [_, sprite]: sprites)
    {
        SDL_DestroyTexture(sprite.texture);
    }
    sprites.clear();
    spriteNames.clear();
    entt::meta_reset();
}
