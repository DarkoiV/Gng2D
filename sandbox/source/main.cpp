#include <iostream>
#include "Gng2D/core/app.hpp"
#include "Gng2D/core/global.hpp"
#include "Gng2D/scene/scene.hpp"

int main()
{
    Gng2D::initApp();
    Gng2D::GLOBAL::NEXT_SCENE = std::make_unique<Gng2D::Scene>();
    Gng2D::runApp();
    Gng2D::closeApp();
    return 0;
}