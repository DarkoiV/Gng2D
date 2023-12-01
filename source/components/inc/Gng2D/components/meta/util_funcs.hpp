#pragma once
#include "Gng2D/commons/luna/stack.hpp"
#include "Gng2D/components/meta/properties.hpp"

namespace Gng2D {

[[nodiscard]] inline ErrOrSuccess tryPushDatumOnStack(Gng2D::Luna::Stack& stack,
                                                      entt::meta_any&     component,
                                                      Gng2D::StringHash   datumHash)
{
    auto datumHandle = component.get(datumHash);
    if (not datumHandle) [[unlikely]]
    {
        LOG::ERROR("LOL");
        return Err{"failed to get datumHandle"};
    }
    auto field = component.type()
                     .data(datumHash)
                     .prop(Gng2D::ComponentFieldProperties::FIELD_TYPE)
                     .value()
                     .cast<Gng2D::ComponentFieldType>();
    switch (field)
    {
    case Gng2D::INTEGER:
    case Gng2D::STRING_HASH:
        if (not datumHandle.allow_cast<Luna::Integer>()) break;
        stack.pushInt(datumHandle.cast<Luna::Integer>());
        return Success{};
    case Gng2D::FLOAT:
        if (not datumHandle.allow_cast<Luna::Float>()) break;
        stack.pushFloat(datumHandle.cast<Luna::Float>());
        return Success{};
    case Gng2D::STRING:
        if (not datumHandle.allow_cast<Luna::String>()) break;
        stack.pushString(datumHandle.cast<Luna::String>());
        return Success{};
    [[unlikely]] case Gng2D::UNDEF:
        break;
    }

    return Err{"failed to push datum"};
}

[[nodiscard]] inline ErrOrSuccess
trySetDatumFromLunaType(Gng2D::Luna::Type& value, entt::meta_any& component, StringHash datumHash)
{
    auto datumType = component.type().data(datumHash);
    auto field     = component.type()
                     .data(datumHash)
                     .prop(Gng2D::ComponentFieldProperties::FIELD_TYPE)
                     .value()
                     .cast<Gng2D::ComponentFieldType>();
    switch (field)
    {
    case Gng2D::STRING_HASH:
        if (value.isString())
        {
            if (datumType.set(component, value.asStringHash())) return Success{};
            else [[unlikely]] return Err{"cannot set from string"};
        }
        goto INTEGER_CASE_LABEL;
    case Gng2D::FLOAT:
        if (value.isFloat())
        {
            if (datumType.set(component, value.asFloat())) return Success{};
            else [[unlikely]] return Err{"cannot set from float"};
        }
    /* NO BREAK! */
    case Gng2D::INTEGER:
    INTEGER_CASE_LABEL:
        if (value.isInteger())
        {
            if (datumType.set(component, value.asInteger())) return Success{};
            else [[unlikely]] return Err{"cannot set from integer"};
        }
        break;
    case Gng2D::STRING:
        if (value.isString())
        {
            if (datumType.set(component, value.asString())) return Success{};
            else [[unlikely]] return Err{"cannot set from string"};
        }
        break;
    [[unlikely]] case Gng2D::UNDEF:
        break;
    }

    return Err{"unhandled case(?)"};
}
} // namespace Gng2D
