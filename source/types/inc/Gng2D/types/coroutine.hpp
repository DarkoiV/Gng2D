#pragma once
#include <coroutine>
#include <functional>
#include <variant>
#include "Gng2D/core/settings.hpp"

namespace Gng2D
{
struct Coroutine
{
    struct promise_type;

    using  Wait         = unsigned;
    struct Completed {};
    using  Status       = std::variant<Wait, Completed>;

    using HandleType    = std::coroutine_handle<promise_type>;
    struct promise_type
    {
        promise_type()  noexcept    = default;
        ~promise_type() noexcept    = default;

        Coroutine               get_return_object();
        std::suspend_always     initial_suspend() noexcept  { return {}; };
        std::suspend_always     final_suspend()   noexcept  { return {}; };
        void                    unhandled_exception();
        std::suspend_always     yield_value(Status);
        void                    return_void();
        Status retVal;
    };

    template<typename Coro, typename... Args>
    Coroutine(Coro coro, Args&&... args)
        : Coroutine(std::move(coro(std::forward<Args>(args)...)))
    {
    }

    Coroutine();
    ~Coroutine();
    Coroutine(Coroutine&)               = delete;
    Coroutine(Coroutine&&);
    explicit Coroutine(HandleType&&);
    Coroutine& operator=(Coroutine&)    = delete;
    Coroutine& operator=(Coroutine&&);
    Coroutine& operator=(HandleType&&);

    void operator()();
    bool isCompleted() const;

private:
    HandleType  handle;
    Status      status;

    bool isResumable();
};

consteval Coroutine::Wait operator""_s(unsigned long long s)
{
    return s * Gng2D::LOGIC_TICK;
}

consteval Coroutine::Wait operator""_s(long double s)
{
    return s * Gng2D::LOGIC_TICK;
}

}

