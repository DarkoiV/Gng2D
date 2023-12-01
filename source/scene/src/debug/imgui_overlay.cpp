#include "Gng2D/scene/debug/imgui_overlay.hpp"
#include "Gng2D/commons/imgui.hpp"
#include "Gng2D/commons/log.hpp"
#include "Gng2D/components/info.hpp"
#include "Gng2D/components/meta/properties.hpp"
#include "Gng2D/components/meta/util_funcs.hpp"
#include "Gng2D/components/sprite.hpp"
#include "Gng2D/core/global.hpp"
#include <SDL2/SDL.h>
#include <misc/cpp/imgui_stdlib.h>

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
            if (auto* info = reg.try_get<Gng2D::Info>(e))
            {
                ImGui::Text(" (%05d)%s", (uint32_t)e, info->name.c_str());
            }
            else ImGui::Text(" (%05d)Unnamed", (uint32_t)e);

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

    auto getRef      = type.func("getRef"_hs);
    auto patchSignal = type.func("patchSignal"_hs);
    GNG2D_ASSERT(getRef);
    GNG2D_ASSERT(patchSignal);

    entt::meta_any componentHandle{getRef.invoke({}, &reg, e)};
    GNG2D_ASSERT(componentHandle);
    auto compName = type.prop("name"_hs).value();
    GNG2D_ASSERT(compName);
    ImGui::Text(" -- %s -- ", compName.cast<const char* const>());

    /* SPRITE IS SPECIAL CASE */
    if (type.id() == "Sprite"_hs)
    {
        const auto& sprite = componentHandle.cast<Gng2D::Sprite>();

        auto targetWidth        = ImGui::GetWindowWidth();
        targetWidth             = std::min(targetWidth, 150.f);
        float spriteScaleFactor = (targetWidth / sprite.srcRect.w) * .8f;
        float scaledWidth       = sprite.srcRect.w * spriteScaleFactor;
        float scaledHeight      = sprite.srcRect.h * spriteScaleFactor;

        ImGui::SetCursorPosX(scaledWidth * .1f);
        ImGui::Image((void*)sprite.texture, ImVec2(scaledWidth, scaledHeight));
        return;
    }

    for (auto&& [id, datum]: type.data())
    {
        auto displayField = [&]<typename FieldType>(std::function<bool(FieldType*)> input)
        {
            auto datumHandle = componentHandle.get(id);
            GNG2D_ASSERT(datumHandle);
            if (not datumHandle.allow_cast<FieldType>()) [[unlikely]]
            {
                ImGui::Text("INVALID INPUT TYPE");
                return;
            }
            FieldType value    = datumHandle.cast<FieldType>();
            FieldType oldValue = value;
            input(&value);

            Gng2D::ensureDatumBounds(datum, value);
            datum.set(componentHandle, value);
            if (value != oldValue) patchSignal.invoke({}, &reg, e);
        };

        auto field = datum.prop(Gng2D::ComponentFieldProperties::FIELD_TYPE)
                         .value()
                         .cast<Gng2D::ComponentFieldType>();
        auto  fieldName = datum.prop(Gng2D::ComponentFieldProperties::FIELD_NAME);
        auto* shownName = fieldName ? fieldName.value().cast<const char* const>() : "???";

        switch (field)
        {
        case Gng2D::FLOAT:
            displayField.operator()<float>([&](float* value) -> bool
            { return ImGui::InputFloat(shownName, value, 0.0f, 0.0f, "%.1f"); });
            break;
        case Gng2D::INTEGER:
            displayField.operator()<int>([&](int* value) -> bool
            { return ImGui::InputInt(shownName, value); });
            break;
        case Gng2D::STRING:
            displayField.operator()<std::string>([&](std::string* value) -> bool
            { return ImGui::InputText(shownName, value); });
            break;
        default:
            ImGui::Text("UNHANDLED INPUT");
            break;
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
            if (type.prop("isDetail"_hs)) continue;

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
