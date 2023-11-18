#include "Gng2D/core/app.hpp"
#include "Gng2D/core/global.hpp"
#include "Gng2D/editor/editor.hpp"

int main()
{
    Gng2D::initApp();
    Gng2D::GLOBAL::NEXT_SCENE = std::make_unique<Gng2D::Editor>();
    Gng2D::runApp();
    Gng2D::closeApp();
    return 0;
}
