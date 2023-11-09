#include "Gng2D/scene/input_handler.hpp"
#include "Gng2D/commons/log.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#define KEY_PRESS_BOILERPLATE                                                                      \
    .type = SDL_KEYDOWN, .timestamp = 0, .windowID = 0, .state = SDL_PRESSED, .repeat = 0,         \
    .padding2 = 0, .padding3 = 0,

static SDL_KeyboardEvent KEY_P_PRESSED = {
    KEY_PRESS_BOILERPLATE.keysym = {.scancode = SDL_SCANCODE_P,
                                    .sym      = SDLK_p,
                                    .mod      = KMOD_NONE,
                                    .unused   = 0},
};

static SDL_KeyboardEvent KEY_UP_PRESSED = {
    KEY_PRESS_BOILERPLATE.keysym = {.scancode = SDL_SCANCODE_UP,
                                    .sym      = SDLK_UP,
                                    .mod      = KMOD_NONE,
                                    .unused   = 0},
};

static SDL_KeyboardEvent KEY_DOWN_PRESSED = {
    KEY_PRESS_BOILERPLATE.keysym = {.scancode = SDL_SCANCODE_DOWN,
                                    .sym      = SDLK_DOWN,
                                    .mod      = KMOD_NONE,
                                    .unused   = 0},
};

using namespace entt::literals;

struct InputHandlerTest : ::testing::Test
{
    using ActionCallback = testing::MockFunction<void(entt::entity, Gng2D::ActionListener::Action)>;

    InputHandlerTest()
    {
        initEntityOne();
        initEntityTwo();
    }

    void initEntityOne()
    {
        entity_one = registry.create();

        auto& kphandler = registry.emplace<Gng2D::ActionListener>(entity_one);
        kphandler.actions.emplace_back("up"_hs);
        kphandler.actions.emplace_back("down"_hs);

        entt::sink sink{kphandler.signal};
        sink.connect<&InputHandlerTest::callCallback>(this);
    }

    void initEntityTwo()
    {
        entity_two = registry.create();

        auto& kphandler = registry.emplace<Gng2D::ActionListener>(entity_two);
        kphandler.actions.emplace_back("down"_hs);

        entt::sink sink{kphandler.signal};
        sink.connect<&InputHandlerTest::callCallback>(this);
    }

    void callCallback(entt::entity e, Gng2D::ActionListener::Action a)
    {
        actionCallback.Call(e, a);
    }

    entt::registry      registry;
    entt::entity        entity_one;
    entt::entity        entity_two;
    Gng2D::InputHandler SUT{registry};
    ActionCallback      actionCallback;
};

TEST_F(InputHandlerTest, NothingHappensOnNonProccessedAction)
{
    SUT.handleKeyPress(KEY_P_PRESSED);
    EXPECT_CALL(actionCallback, Call(entity_one, testing::_)).Times(0);
    EXPECT_CALL(actionCallback, Call(entity_two, testing::_)).Times(0);
}

TEST_F(InputHandlerTest, OnHandledKeyPress_KeyPressComponentsWillPublishToSinks)
{
    SUT.registerKeyPressAction(SDL_SCANCODE_UP, "up"_hs);
    SUT.registerKeyPressAction(SDL_SCANCODE_DOWN, "down"_hs);

    EXPECT_CALL(actionCallback, Call(entity_one, entt::hashed_string::value("up"))).Times(1);
    EXPECT_CALL(actionCallback, Call(entity_two, entt::hashed_string::value("up"))).Times(0);

    EXPECT_CALL(actionCallback, Call(entity_one, entt::hashed_string::value("down"))).Times(1);
    EXPECT_CALL(actionCallback, Call(entity_two, entt::hashed_string::value("down"))).Times(1);

    SUT.handleKeyPress(KEY_UP_PRESSED);
    SUT.handleKeyPress(KEY_DOWN_PRESSED);
}

