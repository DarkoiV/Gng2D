#pragma once
#include "Gng2D/commons/types.hpp"
#include "Gng2D/components/sprite.hpp"
#include <entt/entt.hpp>
#include <string>

namespace Gng2D {
struct Repository
{
    enum class COMPONENT_PROPERTIES : uint32_t {
        NAME,
        ID,
    };

    static void        loadSprite(const std::string& name,
                                  const std::string& path     = "sprites/",
                                  const std::string& fileType = ".png");
    static Sprite      getSprite(const StringHash);
    static std::string getSpriteName(const StringHash);

    template <typename Component>
    static auto registerComponent(const std::string&);
    static void registerDefaultComponents();

    static void freeResources();

  private:
    inline static std::map<StringHash, Sprite>      sprites;
    inline static std::map<StringHash, std::string> spriteNames;
};

template <typename Component>
auto Repository::registerComponent(const std::string& name)
{
    using namespace entt::literals;
    auto id = entt::hashed_string::value(name.c_str());
    return entt::meta<Component>()
        .type(id)
        .prop(static_cast<uint32_t>(COMPONENT_PROPERTIES::NAME), name)
        .prop(static_cast<uint32_t>(COMPONENT_PROPERTIES::ID), id);
}

} // namespace Gng2D
