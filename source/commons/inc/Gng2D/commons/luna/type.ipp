#pragma once
#include "Gng2D/commons/assert.hpp"
#include "type.hpp"

namespace Gng2D::Luna {
template <typename T>
bool Type::tryAssignTo(T& target)
{
    if constexpr (std::is_same_v<T, Bool>)
    {
        if (this->isBool()) target = this->asBool();
        else return false;
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        if (this->isFloat()) target = this->asFloat();
        else return false;
    }
    else if constexpr (std::is_integral_v<T>)
    {
        if (this->isInteger()) target = this->asInteger();
        else return false;
    }
    else if constexpr (std::is_convertible_v<T, String>)
    {
        if (this->isString()) target = this->asString();
        else return false;
    }
    else if constexpr (std::is_same_v<T, TableRef>)
    {
        if (this->isTable()) target = this->asTable();
        else return false;
    }
    else GNG2D_ASSERT_CONSTEXPR("Target type not supported");

    return true;
}

template <typename T>
constexpr bool operator==(const T& lhs, const Type& rhs)
{
    if constexpr (std::is_same_v<T, Nil>)
    {
        if (rhs.isNil()) return true;
    }
    else if constexpr (std::is_same_v<T, Bool>)
    {
        if (rhs.isBool()) return rhs.asBool() == lhs;
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        if (rhs.isFloat()) return rhs.asFloat() == lhs;
    }
    else if constexpr (std::is_integral_v<T>)
    {
        if (rhs.isInteger()) return rhs.asInteger() == lhs;
    }
    else if constexpr (std::is_convertible_v<T, String>)
    {
        if (rhs.isString()) return rhs.asString() == lhs;
    }
    else if constexpr (std::is_same_v<T, TableRef>)
    {
        if (rhs.isTable()) return rhs.asTable() == lhs;
    }
    return false;
}

template <typename T>
constexpr bool operator==(const Type& lhs, const T& rhs)
{
    return rhs == lhs;
}
} // namespace Gng2D::Luna
