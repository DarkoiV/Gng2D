#include "Gng2D/core/application.hpp"
#include "Gng2D/scene/scene.hpp"

struct SandboxScene : Gng2D::Scene
{
};

struct Sandbox : Gng2D::Application
{
    void onCreate() override 
    {
        setNextScene<SandboxScene>();
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

