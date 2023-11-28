#include "Gng2D/commons/repository.hpp"
#include "Gng2D/commons/args_vector.hpp" // IWYU pragma: keep
#include "Gng2D/commons/log.hpp"
#include "Gng2D/components/info.hpp"
#include "Gng2D/components/lua_script.hpp"
#include "Gng2D/components/relationship.hpp"
#include "Gng2D/components/sprite.hpp"
#include "Gng2D/components/transform.hpp"
#include "Gng2D/core/global.hpp"
#include <SDL2/SDL_image.h>

using Gng2D::Repository;

void Repository::loadSprite(const std::string& name,
                            const std::string& directory,
                            const std::string& fileType)
{
    const auto fullPath       = GLOBAL::DATA_DIRECTORY / directory / (name + fileType);
    const auto spriteNameHash = entt::hashed_string{name.c_str()};
    if (auto it = spriteNames.find(spriteNameHash); it != spriteNames.end())
    {
        if (it->second == name) return LOG::WARN("Sprite already loaded");
        else LOG::ERROR("Sprite collision", name, " has the same hash as ", it->second);
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

Gng2D::Sprite Repository::getSprite(const std::string& name)
{
    return getSprite(entt::hashed_string::value(name.c_str()));
}

const std::string& Repository::spriteNameFromHash(const StringHash hash)
{
    if (auto it = spriteNames.find(hash); it != spriteNames.end())
    {
        return it->second;
    }
    else [[unlikely]] return UNKNOWN_HASH;
}

void Repository::registerDefaultComponents()
{
    using namespace entt::literals;
    registerComponent<Info>();
    registerComponent<Sprite>();
    registerComponent<Transform2d>();
    registerComponent<TransformLayer>();
    registerComponent<LuaScript>();
    registerComponent<Parent>();
    registerComponent<Children>();

    registerComponent<detail::Position>();
    registerComponent<detail::Layer>();
}

const std::string& Repository::componentNameFromHash(const StringHash hash)
{
    if (auto it = componentNames.find(hash); it != componentNames.end())
    {
        return it->second;
    }
    else [[unlikely]] return UNKNOWN_HASH;
}

void Repository::attachComponentHooks(entt::registry* r)
{
    for (auto&& attachHooks: cachedHooks)
        attachHooks(r);
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
    componentNames.clear();
    cachedHooks.clear();
    entt::meta_reset();
    entt::meta_reset(detailComponentCtx);
}
