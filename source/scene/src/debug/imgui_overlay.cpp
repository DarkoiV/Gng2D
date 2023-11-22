#include "Gng2D/scene/debug/imgui_overlay.hpp"
#include "Gng2D/commons/imgui.hpp"
#include "Gng2D/commons/log.hpp"
#include "Gng2D/components/meta/component_meta_info.hpp"
#include "Gng2D/components/meta/properties.hpp"
#include "Gng2D/core/global.hpp"
#include <SDL2/SDL.h>

using Gng2D::ImguiOverlay;

struct IMGUI_OVERLAY_EDITED
{
};

ImguiOverlay::ImguiOverlay(entt::registry& r)
    : SystemInterface(r)
{
    LOG::INFO("ImguiOverlay created");
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
}

void ImguiOverlay::onUpdate()
{
    // Left sidebar
    beginLeftSidebar();
    entityList();
    endLeftSidebar();

    // Floating windows
    editedEntities();
}

void ImguiOverlay::beginLeftSidebar()
{
    ImGui::SetNextWindowSize(ImVec2(IMGUI_LEFT_SIDEBAR,
                                    GLOBAL::RENDER_HEIGHT * GLOBAL::RENDER_SCALE));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    auto leftSidebarFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
    ImGui::Begin("leftSidebar", NULL, leftSidebarFlags);
}

void ImguiOverlay::endLeftSidebar()
{
    ImGui::End();
}

void ImguiOverlay::entityList()
{
    if (ImGui::BeginChild("entityList", ImVec2(0, 0), ImGuiChildFlags_None,
                          ImGuiWindowFlags_MenuBar))
    {
        ImGui::SetWindowFontScale(1.5f);
        for (auto&& [e]: reg.view<entt::entity>().each())
        {
            ImGui::Text(" Entity %05d", (uint32_t)e);
            if (ImGui::IsItemClicked())
            {
                if (reg.all_of<IMGUI_OVERLAY_EDITED>(e)) reg.erase<IMGUI_OVERLAY_EDITED>(e);
                else reg.emplace<IMGUI_OVERLAY_EDITED>(e);
            }
        }
    }
    ImGui::EndChild();
}

static void displayComponent(entt::registry& reg, entt::entity e, entt::meta_type type)
{
    using namespace entt::literals;
    const auto* metaInfo = Gng2D::getMetaInfo(type);
    ImGui::Text("%s", metaInfo->name.c_str());

    auto getRef      = type.func("getRef"_hs);
    auto patchSignal = type.func("patchSignal"_hs);
    GNG2D_ASSERT(getRef);
    GNG2D_ASSERT(patchSignal);

    entt::meta_any componentHandle{getRef.invoke({}, &reg, e)};
    GNG2D_ASSERT(componentHandle.type().id() == metaInfo->id);

    if (not metaInfo->data) return;
    auto data = *(metaInfo->data);
    for (auto& datum: data)
    {
        if (datum.type == entt::type_id<float>())
        {
            float value    = *((float*)type.get(datum.id, componentHandle).data());
            float oldValue = value;
            ImGui::InputFloat(datum.name.c_str(), &value, 0.0f, 0.0f, "%.1f",
                              ImGuiInputTextFlags_AutoSelectAll);
            type.data(datum.id).set(componentHandle, value);
            if (value != oldValue) patchSignal.invoke({}, &reg, e);
        }
    }
}

void ImguiOverlay::editedEntities()
{
    using namespace entt::literals;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1.f, 1.f));
    for (auto&& [e]: reg.view<IMGUI_OVERLAY_EDITED>().each())
    {
        bool              keepOpen = true;
        const std::string windowId = "Entity edit: " + std::to_string((uint32_t)e);
        ImGui::Begin(windowId.c_str(), &keepOpen);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(7.f, 7.f));
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 2.5f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        for (auto&& [id, type]: entt::resolve())
        {
            auto* storage = reg.storage(id);
            if (not storage or not storage->contains(e)) continue;

            std::string childWindowId = "Comp" + std::to_string(id);
            if (ImGui::BeginChild(childWindowId.c_str(), ImVec2(0, 0),
                                  ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY))
            {
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2.f));
                displayComponent(reg, e, type);
                ImGui::PopStyleVar();
            }
            ImGui::EndChild();
        }

        ImGui::PopStyleVar(3);
        ImGui::End();
        if (not keepOpen) reg.remove<IMGUI_OVERLAY_EDITED>(e);
    }
    ImGui::PopStyleVar();
}
