#pragma once
#include "Gng2D/commons/log.hpp"
#include "Gng2D/commons/types.hpp"
#include "Gng2D/components/sprite.hpp"
#include <entt/entt.hpp>
#include <string>

namespace Gng2D {
struct Repository
{
    enum COMPONENT_PROPERTIES : StringHash {
        NAME = entt::hashed_string::value("name"),
        ID   = entt::hashed_string::value("id"),
    };

    static void        loadSprite(const std::string& name,
                                  const std::string& path     = "sprites/",
                                  const std::string& fileType = ".png");
    static Sprite      getSprite(const StringHash);
    static Sprite      getSprite(const std::string&);
    static std::string getSpriteName(const StringHash);

    template <typename Component>
    static auto registerComponent(const std::string&);
    static void registerDefaultComponents();

    template <typename Component>
    static void emplaceComponent(entt::registry*, entt::entity, entt::meta_any&);

    static void freeResources();

  private:
    inline static std::map<StringHash, Sprite>      sprites;
    inline static std::map<StringHash, std::string> spriteNames;
};

template <typename Component>
void Repository::emplaceComponent(entt::registry* r, entt::entity e, entt::meta_any& c)
{
    LOG::INFO("Emplacing:",
              c.type().prop(COMPONENT_PROPERTIES::NAME).value().template cast<std::string>());
    r->emplace<Component>(e, c.cast<Component>());
}

template <typename Component>
auto Repository::registerComponent(const std::string& name)
{
    using namespace entt::literals;
    auto id = entt::hashed_string::value(name.c_str());
    LOG::INFO("Registering component:", name, "id:", id);
    return entt::meta<Component>()
        .type(id)
        .prop(COMPONENT_PROPERTIES::NAME, name)
        .prop(COMPONENT_PROPERTIES::ID, id)
        .template func<&Repository::emplaceComponent<Component>>("emplace"_hs);
}

} // namespace Gng2D
