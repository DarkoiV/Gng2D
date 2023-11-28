#pragma once
#include "Gng2D/commons/types.hpp"
#include "Gng2D/components/meta/properties.hpp" // IWYU pragma: keep
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

    template <Component C>
    static void               registerComponent();
    static void               registerDefaultComponents();
    static const std::string& componentNameFromHash(const StringHash);
    static void               attachComponentHooks(entt::registry*);

    static void freeResources();

  private:
    inline const static std::string UNKNOWN_HASH{"UNKNOWN HASH"};

    inline static std::map<StringHash, Sprite>      sprites;
    inline static std::map<StringHash, std::string> spriteNames;

    using HookCache = std::function<void(entt::registry*)>;
    inline static std::map<StringHash, std::string> componentNames;
    inline static std::vector<HookCache>            cachedHooks;

    inline static entt::meta_ctx detailComponentCtx;
};
} // namespace Gng2D
#include "repository.ipp"
