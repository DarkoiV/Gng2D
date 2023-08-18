#include "Gng2D/types/coroutine.hpp"
#include "Gng2D/core/log.hpp"

using namespace Gng2D;

Coroutine Coroutine::promise_type::get_return_object()
{
    return Coroutine{std::coroutine_handle<promise_type>::from_promise(*this)};
}

void Coroutine::promise_type::unhandled_exception()
{
    LOG::ERROR("Unhandled exception in coroutine");
}

std::suspend_always Coroutine::promise_type::yield_value(Status s)
{
    retVal = s;
    return {};
}

void Coroutine::promise_type::return_void()
{
    retVal = Completed{};
}

Coroutine::Coroutine()
{
    handle = nullptr;
    status = Completed{};
}

Coroutine::~Coroutine()
{
    if (handle) handle.destroy();
}

Coroutine::Coroutine(Coroutine&& from)
{
    status = from.status;
    handle = from.handle;
    from.handle = nullptr;
    from.status = Completed{};
}

Coroutine::Coroutine(HandleType&& h)
{
    handle = h;
    h = nullptr;
}

Coroutine& Coroutine::operator=(Coroutine&& from)
{
    if (handle) handle.destroy();
    handle = from.handle;
    status = from.status;
    from.handle = nullptr;
    from.status = Completed{};

    return *this;
}

Coroutine& Coroutine::operator=(HandleType&& h)
{
    handle = h;
    h = nullptr;
    return *this;
}

void Coroutine::operator()()
{
    if (not isResumable()) return;
    handle.resume();
    status = handle.promise().retVal;
}

bool Coroutine::isCompleted() const
{
    if(std::holds_alternative<Completed>(status)) return true;
    return false;
}

template<class... Ts> struct Overload : Ts... { using Ts::operator()...; };
template<class... Ts> Overload(Ts...) -> Overload<Ts...>;

bool Coroutine::isResumable()
{
    return std::visit(Overload{
        [](Completed&) -> bool 
        { 
            return false; 
        },
        [](Wait& wt) -> bool
        {
            // Waiting 0 ticks has no logical sens imho,
            // But idk how to constrain it for now
            // so it will act as waiting 1 tick
            if (wt <= 1) return true;
            --wt;
            return false;
        }
    }, status);
}

