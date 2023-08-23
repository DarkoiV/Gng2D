#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Gng2D/types/coroutine.hpp"

static Gng2D::Coroutine empty()
{
    co_return;
}

static Gng2D::Coroutine waitXTicks(unsigned ticks, bool& beforeYield, bool& afterYield)
{
    beforeYield = true;
    co_yield Gng2D::Coroutine::Wait{ticks};
    afterYield = true;
}

TEST(CoroutineTests, EmptyCoroutineObject_HasCompletedStatus)
{
    Gng2D::Coroutine emptyCoroutineObj;

    ASSERT_TRUE(emptyCoroutineObj.isCompleted());
}

TEST(CoroutineTests, EmptyCoroutineBody_IsNotCompletedUntillCalled)
{
    Gng2D::Coroutine emptyBodyCoro{empty};

    ASSERT_FALSE(emptyBodyCoro.isCompleted());
}

TEST(CoroutineTests, EmptyCoroutineBody_IsCompletedAfterBeingCalled)
{
    Gng2D::Coroutine emptyBodyCoro{empty};

    emptyBodyCoro();
    ASSERT_TRUE(emptyBodyCoro.isCompleted());
}

TEST(CoroutineTests, Coroutine_IsSafeToCallAfterCompletion)
{
    Gng2D::Coroutine emptyBodyCoro{empty};

    emptyBodyCoro();
    ASSERT_TRUE(emptyBodyCoro.isCompleted());
    emptyBodyCoro();
    emptyBodyCoro();
    ASSERT_TRUE(emptyBodyCoro.isCompleted());
}

TEST(CoroutineTests, Coroutine_CanBeMoved)
{
    Gng2D::Coroutine emptyBodyCoro{empty};
    auto newCoro = std::move(emptyBodyCoro);

    ASSERT_FALSE(newCoro.isCompleted());
    newCoro();
    ASSERT_TRUE(emptyBodyCoro.isCompleted());
    ASSERT_TRUE(newCoro.isCompleted());
}

TEST(CoroutineTests, CompletedCoroutine_CanBeMoved)
{
    Gng2D::Coroutine emptyBodyCoro{empty};
    emptyBodyCoro();
    ASSERT_TRUE(emptyBodyCoro.isCompleted());

    auto newCoro = std::move(emptyBodyCoro);
    ASSERT_TRUE(newCoro.isCompleted());
}

TEST(CoroutineTests, Coroutine_WillWaitXTicksBeforeResuming)
{
    constexpr unsigned X_TICKS = 20;
    bool checkpoint1{false}, checkpoint2{false};
    Gng2D::Coroutine waitXTicksCoro{waitXTicks, X_TICKS, checkpoint1, checkpoint2};

    waitXTicksCoro();
    ASSERT_TRUE(checkpoint1);

    for (unsigned i = 0; i < X_TICKS; i++) waitXTicksCoro();
    ASSERT_TRUE(checkpoint2);
}

TEST(CoroutineTests, Coroutine_MovingWillPreserveWaitTicks)
{
    constexpr unsigned X_TICKS = 20;
    bool checkpoint1{false}, checkpoint2{false};
    Gng2D::Coroutine waitXTicksCoro{waitXTicks, X_TICKS, checkpoint1, checkpoint2};

    waitXTicksCoro();
    ASSERT_TRUE(checkpoint1);

    for (unsigned i = 0; i < X_TICKS / 2; i++) waitXTicksCoro();
    ASSERT_FALSE(checkpoint2);

    auto movedCoro = std::move(waitXTicksCoro);
    for (unsigned i = 0; i < X_TICKS / 2; i++) movedCoro();
    ASSERT_TRUE(checkpoint2);
}

