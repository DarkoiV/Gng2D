#pragma once
#include "Gng2D/commons/log.hpp"
#include "Gng2D/commons/types.hpp"
#include "Gng2D/components/properties.hpp"
#include "Gng2D/components/sprite.hpp"
#include <entt/entt.hpp>
#include <string>

namespace Gng2D {
struct Repository
{
    static void               loadSprite(const std::string& name,
                                         const std::string& directory = "sprites",
                                         const std::string& fileType  = ".png");
    static Sprite             getSprite(const StringHash);
    static Sprite             getSprite(const std::string&);
    static const std::string& spriteNameFromHash(const StringHash);

    template <Component::Concept C>
    static auto               registerComponent();
    static void               registerDefaultComponents();
    static const std::string& componentNameFromHash(const StringHash);

    static void freeResources();

  private:
    inline const static std::string UNKNOWN_HASH{"UNKNOWN HASH"};

    inline static std::map<StringHash, Sprite>      sprites;
    inline static std::map<StringHash, std::string> spriteNames;

    inline static std::map<StringHash, std::string> componentNames;

    template <Component::Concept C>
    static void emplaceComponent(entt::registry*, entt::entity, entt::meta_any&);
};

template <Component::Concept C>
void Repository::emplaceComponent(entt::registry* r, entt::entity e, entt::meta_any& c)
{
    const auto& metaInfo = *(C::metaInfo());
    LOG::TRACE("Emplacing:", metaInfo.name);

    r->emplace<C>(e, c.cast<C>());
}

template <Component::Concept Comp>
auto Repository::registerComponent()
{
    using namespace entt::literals;
    auto& metaInfo = *(Comp::metaInfo());
    auto& name     = metaInfo.name;

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

    return entt::meta<Comp>()
        .type(id)
        .prop("metaInfo"_hs, Comp::metaInfo())
        .template ctor<&Comp::fromArgs>()
        .template func<&Repository::emplaceComponent<Comp>>("emplace"_hs);
}

} // namespace Gng2D
