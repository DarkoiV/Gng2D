#pragma once
#include <entt/entt.hpp>
#include <memory>

namespace Gng2D {
struct SystemInterface
{
    SystemInterface(entt::registry& r)
        : reg(r)
    {
    }
    SystemInterface(SystemInterface&)  = delete;
    SystemInterface(SystemInterface&&) = delete;
    virtual ~SystemInterface()         = default;

    auto operator=(SystemInterface&)  = delete;
    auto operator=(SystemInterface&&) = delete;

    virtual void onUpdate() = 0;

  protected:
    entt::registry& reg;
};
using SystemPtr = std::unique_ptr<SystemInterface>;
} // namespace Gng2D
