#pragma once
#include "Gng2D/commons/luna/type.hpp"
#include "Gng2D/commons/types.hpp"
#include "entt/entt.hpp"
#include <vector>

namespace Gng2D {
struct ArgsVector
{
    struct NamedArg
    {
        StringHash     id;
        entt::meta_any arg;
    };

    ArgsVector() = default;
    ArgsVector(std::initializer_list<NamedArg> l)
        : vec(l)
    {
    }
    ArgsVector(const Luna::TableRef&);
    ~ArgsVector() = default;

    void addArg(StringHash id, entt::meta_any arg);
    void addArg(const std::string& name, entt::meta_any arg);

    auto begin() const { return vec.begin(); };
    auto end() const { return vec.end(); };
    auto size() const { return vec.size(); };

  private:
    std::vector<NamedArg> vec;
};
} // namespace Gng2D
