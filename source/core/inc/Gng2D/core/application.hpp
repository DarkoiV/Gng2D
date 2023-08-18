#pragma once
#include "Gng2D/scene/scene.hpp"
#include "Gng2D/core/window.hpp"
#include "Gng2D/core/asset_registry.hpp"

namespace Gng2D
{
struct Application
{
    Application();
    virtual ~Application();

    virtual void onCreate() = 0;
    virtual void onQuit()   = 0;

    void run();
    static void stopRunning();

    template<typename S, typename... Args>
        requires(std::is_base_of<Scene, S>::value)
    static void setNextScene(Args&&... args)
    {
        instance->nextScene = std::make_unique<S>(std::forward<Args>(args)...);
    }

protected:
    AssetRegistry   assetRegistry;

    using ScenePtr          = std::unique_ptr<Scene>;
    inline static ScenePtr  currentScene;
    inline static ScenePtr  nextScene;

private:
    void mainLoop();
    void eventLoop(Scene&);
    void switchScene();

    bool        isRunning{true};
    Window      window;
    uint64_t    previousTS{0};
    uint32_t    logicLag{0};

    static inline Application* instance{nullptr};
};
}

