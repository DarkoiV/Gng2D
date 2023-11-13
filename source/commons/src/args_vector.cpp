#include "Gng2D/commons/args_vector.hpp"

using Gng2D::ArgsVector;

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
