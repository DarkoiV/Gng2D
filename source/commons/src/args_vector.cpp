#include "Gng2D/commons/args_vector.hpp"

using Gng2D::ArgsVector;

static entt::meta_any transformLunaTypeToMetaAny(const Gng2D::Luna::Type& t)
{
    switch (t.index())
    {
    case Gng2D::Luna::TYPE::INT:
        return entt::meta_any(t.asInteger());
    case Gng2D::Luna::TYPE::BOOL:
        return entt::meta_any(t.asBool());
    case Gng2D::Luna::TYPE::FLOAT:
        return entt::meta_any(t.asFloat());
    case Gng2D::Luna::TYPE::STRING:
        using T = decltype(t.asString());
        return entt::meta_any(t.asString());
    }

    Gng2D::LOG::ERROR("LunaType has wrong type!", t.index());
    return entt::meta_any();
}

ArgsVector::ArgsVector(const Luna::TableRef& tr)
{
    for (auto&& [id, value]: tr)
    {
        if (not id.isInteger() and not id.isString())
        {
            LOG::ERROR("component id has to be either string or stringhash");
            continue;
        }
        StringHash argId =
            id.isInteger() ? id.asInteger() : entt::hashed_string::value(id.asString().c_str());
        auto metaValue = transformLunaTypeToMetaAny(value);
        if (metaValue)
        {
            LOG::TRACE("Emplacing in argsVector", argId, metaValue.type().info().name());
            addArg(argId, std::move(metaValue));
        }
    }
}

void ArgsVector::addArg(StringHash id, entt::meta_any arg)
{
    auto it =
        std::find_if(vec.begin(), vec.end(), [id](const NamedArg& arg) { return arg.id == id; });
    if (it == vec.end())
    {
        vec.emplace_back(NamedArg{id, std::move(arg)});
    }
    else
    {
        *it = NamedArg{id, std::move(arg)};
    }
}

void ArgsVector::addArg(const std::string& name, entt::meta_any arg)
{
    auto id = entt::hashed_string::value(name.c_str());
    addArg(id, std::move(arg));
}
