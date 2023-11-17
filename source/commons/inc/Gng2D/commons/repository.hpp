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

    template <Component::Default C>
    static auto               registerComponent(const std::string&);
    static void               registerDefaultComponents();
    static const std::string& componentNameFromHash(const StringHash);

    static void freeResources();

  private:
    inline const static std::string UNKNOWN_HASH{"UNKNOWN HASH"};

    inline static std::map<StringHash, Sprite>      sprites;
    inline static std::map<StringHash, std::string> spriteNames;

    inline static std::map<StringHash, std::string> componentNames;

    template <Component::Default C>
    static void emplaceComponent(entt::registry*, entt::entity, entt::meta_any&);
};

template <Component::Default C>
void Repository::emplaceComponent(entt::registry* r, entt::entity e, entt::meta_any& c)
{
    LOG::TRACE("Emplacing:", Component::getName<C>(), Component::getId<C>());

    r->emplace<C>(e, c.cast<C>());
}

template <Component::Default Comp>
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
    LOG::TRACE("Accepted args: ", *(Comp::argsInfo()));
    return entt::meta<Comp>()
        .type(id)
        .prop(Component::PROPERTIES::NAME, name)
        .prop(Component::PROPERTIES::ID, id)
        .prop(Component::PROPERTIES::ARGS_INFO, Comp::argsInfo())
        .template ctor<&Comp::fromArgs>()
        .template func<&Repository::emplaceComponent<Comp>>("emplace"_hs);
}

} // namespace Gng2D
