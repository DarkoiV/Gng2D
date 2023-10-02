#include "Gng2D/core/application.hpp"
#include "Gng2D/core/log.hpp"
#include "Gng2D/scene/scene.hpp"
#include "Gng2D/gui/icon.hpp"

struct SandboxScene : Gng2D::Scene
{
    Gng2D::Widget* icon;

    void onEnter() 
    {
        Gng2D::LOG::INFO("Hello");
        icon = new Gng2D::Icon(reg, Gng2D::Position{200, 200}, "icon");
    };

    void onExit()
    {
        Gng2D::LOG::INFO("Good bye");
    }
};

struct Sandbox : Gng2D::Application
{
    void onCreate() override 
    {
        setNextScene<SandboxScene>();
        assetRegistry.loadSprite("icon");
    }

    void onQuit() override 
    {
        stopRunning();
    }
};

int main()
{
    Sandbox().run();
}

