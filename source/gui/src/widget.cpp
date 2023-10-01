#include "Gng2D/gui/widget.hpp"
#include "Gng2D/components/relationship.hpp"

using Gng2D::Widget;

Widget::Widget(entt::registry& r, Position p)
    : reg(r)
    , root(reg.create()) 
{
    reg.emplace<Gng2D::Position>(root, p);
}

Widget::Widget(entt::registry& r, Widget& parent, RelativePosition rp)
    : reg(r)
    , root(reg.create()) 
{
    reg.emplace<Parent>(root, parent.root);
    reg.emplace<Gng2D::RelativePosition>(root, rp);
}

Widget::~Widget()
{
    if (reg.valid(root)) reg.destroy(root);
}

const Gng2D::Position Widget::getAbsolutePos() const 
{
    return reg.get<Gng2D::Position>(root);
}

const Gng2D::Position Widget::getPos() const 
{
    if (auto* rpos = reg.try_get<Gng2D::RelativePosition>(root)) return static_cast<Gng2D::Position>(*rpos);
    else return reg.get<Gng2D::Position>(root);
}

void Widget::setPos(float x, float y)
{
    if (reg.all_of<Gng2D::RelativePosition>(root)) reg.replace<Gng2D::RelativePosition>(root, x, y);
    else reg.replace<Gng2D::Position>(root, x, y);
}

