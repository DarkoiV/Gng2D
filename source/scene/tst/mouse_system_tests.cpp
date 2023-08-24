#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Gng2D/scene/mouse_system.hpp"
#include "Gng2D/components/mouse_controlls.hpp"
#include "Gng2D/components/position.hpp"

struct MouseSystemTests : ::testing::Test 
{
    entt::registry      reg;
    Gng2D::MouseSystem  sut{reg};

    int         mouseX = 0;
    int         mouseY = 0;

    ::testing::MockFunction<void(entt::registry&, entt::entity)> onEnterHover;
    ::testing::MockFunction<void(entt::registry&, entt::entity)> onLeaveHover;

    void callOnEnter(entt::registry& reg, entt::entity e)
    {
        onEnterHover.Call(reg, e);
    }

    void callOnLeave(entt::registry& reg, entt::entity e)
    {
        onLeaveHover.Call(reg, e);
    }
    entt::entity            entity0{reg.create()};
    const Gng2D::Position   pos0{10, 20};
    const Gng2D::V2d        size0{10, 20};
    const Gng2D::Hoverable  hoverable0{
        {entt::connect_arg<&MouseSystemTests::callOnEnter>, this},
        {entt::connect_arg<&MouseSystemTests::callOnLeave>, this},
        size0};


    MouseSystemTests()
    {
        reg.emplace<Gng2D::Position>(entity0, pos0);
        reg.emplace<Gng2D::Hoverable>(entity0, hoverable0);
    }


    void moveMouse(int x, int y)
    {
        mouseX += x;
        mouseY += y;

        SDL_MouseMotionEvent event;
        event.type = SDL_MOUSEMOTION;
        event.xrel = x;
        event.yrel = y;
        event.x = mouseX;
        event.y = mouseY;

        sut.motion(event);
    }

    void pushLMB()
    {
        SDL_MouseButtonEvent event;
        event.type = SDL_MOUSEBUTTONDOWN;
        event.button = SDL_BUTTON_LEFT;
        event.x = mouseX;
        event.y = mouseY;

        sut.button(event);
    }

    void releaseLMB()
    {
        SDL_MouseButtonEvent event;
        event.type = SDL_MOUSEBUTTONUP;
        event.button = SDL_BUTTON_LEFT;
        event.x = mouseX;
        event.y = mouseY;

        sut.button(event);
    }

};

using ::testing::_;

TEST_F(MouseSystemTests, EnteringHover_WillCallOnEnterHoverFunction)
{
    EXPECT_CALL(onEnterHover, Call(_, entity0)).Times(1);

    moveMouse(pos0.x, pos0.y);
};

TEST_F(MouseSystemTests, LeavingHover_WillCallOnLeaveHoverFunction)
{
    EXPECT_CALL(onEnterHover, Call(_, entity0)).Times(1);
    EXPECT_CALL(onLeaveHover, Call(_, entity0)).Times(1);

    moveMouse(pos0.x, pos0.y);
    moveMouse(-pos0.x, -pos0.y);
}

TEST_F(MouseSystemTests, MovingInsideHoverableArea_WontCallAdditionLeaveOrEnterHover)
{
    EXPECT_CALL(onEnterHover, Call(_, entity0)).Times(1);
    EXPECT_CALL(onLeaveHover, Call(_, entity0)).Times(0);

    moveMouse(pos0.x, pos0.y);
    moveMouse(size0.x/2 - 1, size0.y/2 - 1);
    moveMouse(0, -size0.y + 2);
    moveMouse(-size0.x + 2, 0);
}

