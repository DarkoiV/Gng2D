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

    static void               loadSprite(const std::string& name,
                                         const std::string& path     = "sprites/",
                                         const std::string& fileType = ".png");
    static Sprite             getSprite(const StringHash);
    static Sprite             getSprite(const std::string&);
    static const std::string& spriteNameFromHash(const StringHash);

    template <typename Component>
    static auto               registerComponent(const std::string&);
    static void               registerDefaultComponents();
    static const std::string& componentNameFromHash(const StringHash);

    template <typename Component>
    static void emplaceComponent(entt::registry*, entt::entity, entt::meta_any&);

    static void freeResources();

  private:
    inline static std::map<StringHash, Sprite>      sprites;
    inline static std::map<StringHash, std::string> spriteNames;

    inline static std::map<StringHash, std::string> componentNames;

    inline const static std::string UNKNOWN_HASH{"UNKNOWN HASH"};
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
    if (auto it = componentNames.find(id); it != componentNames.end())
    {
        if (it->second == name) LOG::FATAL("Component", name, "already registered");
        else LOG::FATAL("Component hash collision", name, "has the same hash as", it->second);
    }
    else
    {
        LOG::INFO("Registering component:", name, "with id:", id);
        componentNames[id] = name;
    }
    return entt::meta<Component>()
        .type(id)
        .prop(COMPONENT_PROPERTIES::NAME, name)
        .prop(COMPONENT_PROPERTIES::ID, id)
        .template func<&Repository::emplaceComponent<Component>>("emplace"_hs);
}

} // namespace Gng2D
