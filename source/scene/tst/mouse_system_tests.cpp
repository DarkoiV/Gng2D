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

    using ModFuncMock = ::testing::MockFunction<void(entt::registry&, entt::entity)>;
    ModFuncMock onEnterHover;
    ModFuncMock onLeaveHover;
    ModFuncMock onLeftClick;
    ModFuncMock onRightClick;

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

    entt::entity            entity1{reg.create()};
    const Gng2D::Position   pos1{30, -20};
    const Gng2D::V2d        size1{21, 20};
    const Gng2D::Hoverable  hoverable1{
        {entt::connect_arg<&MouseSystemTests::callOnEnter>, this},
        {entt::connect_arg<&MouseSystemTests::callOnLeave>, this},
        size0};


    MouseSystemTests()
    {
        reg.emplace<Gng2D::Position>(entity0, pos0);
        reg.emplace<Gng2D::Hoverable>(entity0, hoverable0);

        reg.emplace<Gng2D::Position>(entity1, pos1);
        reg.emplace<Gng2D::Hoverable>(entity1, hoverable1);
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

TEST_F(MouseSystemTests, MovingFromOneHoverableAreaToAnother_WillCallHoverModFuncsInOrder)
{
    {
        testing::InSequence s;

        EXPECT_CALL(onEnterHover, Call(_, entity0)).Times(1);
        EXPECT_CALL(onLeaveHover, Call(_, entity0)).Times(1);
        EXPECT_CALL(onEnterHover, Call(_, entity1)).Times(1);
    }

    moveMouse(pos0.x, pos0.y);
    moveMouse(-pos0.x + pos1.x, -pos0.y + pos1.y);
}

struct MouseClicksTests : MouseSystemTests 
{
    void callOnLeftClick(entt::registry& reg, entt::entity e)
    {
        onLeftClick.Call(reg, e);
    }

    void callOnRightClick(entt::registry& reg, entt::entity e)
    {
        onRightClick.Call(reg, e);
    }

    const Gng2D::Clickable clickable0{
        {entt::connect_arg<&MouseClicksTests::callOnLeftClick>, this},
        {entt::connect_arg<&MouseClicksTests::callOnRightClick>, this}};

    const Gng2D::Clickable clickable1{};

    MouseClicksTests()
    {
        reg.emplace<Gng2D::Clickable>(entity0, clickable0);
        reg.emplace<Gng2D::Clickable>(entity1, clickable1);
    }

    void pushLMB()
    {
        SDL_MouseButtonEvent event;
        event.type = SDL_MOUSEBUTTONDOWN;
        event.button = SDL_BUTTON_LEFT;
        event.state = SDL_PRESSED;
        event.x = mouseX;
        event.y = mouseY;

        sut.button(event);
    }

    void releaseLMB()
    {
        SDL_MouseButtonEvent event;
        event.type = SDL_MOUSEBUTTONUP;
        event.button = SDL_BUTTON_LEFT;
        event.state = SDL_RELEASED;
        event.x = mouseX;
        event.y = mouseY;

        sut.button(event);
    }

    void pushRMB()
    {
        SDL_MouseButtonEvent event;
        event.type = SDL_MOUSEBUTTONDOWN;
        event.button = SDL_BUTTON_RIGHT;
        event.state = SDL_PRESSED;
        event.x = mouseX;
        event.y = mouseY;

        sut.button(event);
    }

    void releaseRMB()
    {
        SDL_MouseButtonEvent event;
        event.type = SDL_MOUSEBUTTONUP;
        event.button = SDL_BUTTON_RIGHT;
        event.state = SDL_RELEASED;
        event.x = mouseX;
        event.y = mouseY;

        sut.button(event);
    }
};

TEST_F(MouseClicksTests, LeftClickingOnHoveredArea_CausesOnClickCallback)
{
    EXPECT_CALL(onEnterHover, Call(_,_));
    EXPECT_CALL(onLeftClick, Call(_, entity0)).Times(1);

    moveMouse(pos0.x, pos0.y);
    pushLMB();
    releaseLMB();
}

TEST_F(MouseClicksTests, LeftClickingOnHoveredArea_WhenNoCallbackProvied_WontCallClickCallback)
{
    EXPECT_CALL(onEnterHover, Call(_,_));
    EXPECT_CALL(onLeftClick, Call(_, entity1)).Times(0);

    moveMouse(pos1.x, pos1.y);
    pushLMB();
    releaseLMB();
}

TEST_F(MouseClicksTests, LeftPressingThenReleasingOutOfHoverableArea_WontCallClickCallback)
{
    EXPECT_CALL(onEnterHover, Call(_,_));
    EXPECT_CALL(onLeaveHover, Call(_,_));
    EXPECT_CALL(onLeftClick, Call(_, entity0)).Times(0);

    moveMouse(pos0.x, pos0.y);
    pushLMB();
    moveMouse(-pos0.x, -pos0.y);
    releaseLMB();
}

TEST_F(MouseClicksTests, RightClickingOnHoveredArea_CausesOnClickCallback)
{
    EXPECT_CALL(onEnterHover, Call(_,_));
    EXPECT_CALL(onRightClick, Call(_, entity0)).Times(1);

    moveMouse(pos0.x, pos0.y);
    pushRMB();
    releaseRMB();
}

TEST_F(MouseClicksTests, RightClickingOnHoveredArea_WhenNoCallbackProvied_WontCallClickCallback)
{
    EXPECT_CALL(onEnterHover, Call(_,_));
    EXPECT_CALL(onRightClick, Call(_, entity1)).Times(0);

    moveMouse(pos1.x, pos1.y);
    pushRMB();
    releaseRMB();
}

TEST_F(MouseClicksTests, RightPressingThenReleasingOutOfHoverableArea_WontCallClickCallback)
{
    EXPECT_CALL(onEnterHover, Call(_,_));
    EXPECT_CALL(onLeaveHover, Call(_,_));
    EXPECT_CALL(onRightClick, Call(_, entity0)).Times(0);

    moveMouse(pos0.x, pos0.y);
    pushRMB();
    moveMouse(-pos0.x, -pos0.y);
    releaseRMB();
}

