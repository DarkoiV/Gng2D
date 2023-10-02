#pragma once
#include "Gng2D/gui/widget.hpp"

namespace Gng2D
{
struct Icon : Widget
{
    Icon(entt::registry&, Position, const std::string& spriteName);
    Icon(entt::registry&, Widget& parent, RelativePosition, const std::string& spriteName);

    V2d getDimensions() const override;

    void setSprite(const std::string& spriteName);
};
}

