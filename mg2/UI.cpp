#include "UI.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>
#include <unordered_map>
#include <string>
#include <cmath>
#include "model/Torus.h"
#include "shaders/torus/TorusShader.h"
#include "model/Point.h"

void Panel::renderCameraSection(Camera& camera) {
    if (ImGui::CollapsingHeader("Kamera")) {
        ImGui::DragFloat("Pos X", &camera.cameraPosition.x, 0.1f, -5, 5);
        ImGui::DragFloat("Pos Y", &camera.cameraPosition.y, 0.1f, -5, 5);
        ImGui::DragFloat("Pos Z", &camera.cameraPosition.z, 0.1f, -5, 5);

        ImGui::DragFloat("Target X", &camera.targetGoal.x, 0.1f, -5, 5);
        ImGui::DragFloat("Target Y", &camera.targetGoal.y, 0.1f, -5, 5);
        ImGui::DragFloat("Target Z", &camera.targetGoal.z, 0.1f, -5, 5);
    }
}

void Panel::renderAddSection(Scene& scene) {
    ImGui::Text("Tryb dodawania:");

    modeButton("+ Dodaj punkt", AddMode::Point, scene);
    modeButton("+ Dodaj torus", AddMode::Torus, scene);
    if (Panel::addMode != AddMode::None)
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Kliknij na scene aby dodac");
}

void Panel::modeButton(const char* label, AddMode mode, Scene& scene) {
    bool active = Panel::addMode == mode;
    
    if (active) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
    }

    if (ImGui::Button(label, ImVec2(-1, 0))) {
        Panel::addMode = active ? AddMode::None : mode;
    }

    if (active) { 
        ImGui::PopStyleColor();
    }
}

void Panel::renderObjectList(Scene& scene, bool& pointsDirty, Camera& camera, 
    int panelWidth, int H, int W, Matrix4x4 P) {
    std::shared_ptr<SceneObject> toRemove = nullptr;

    if (scene.cursorExists) {
        ImGui::PushID("__cursor_entry__");
        
        ImGui::Checkbox("###Kursor", &scene.cursorFollow);
        ImGui::SameLine();

        if (ImGui::TreeNode("##cursor_node", "Kursor")) {
            ImGui::Checkbox("Rotacja wokol kursora", &Panel::rotate);
            
            if (Panel::rotate){
                rotateMode = RotateMode::Cursor;
            }
            else if (scene.chosen.size() > 1) {
                rotateMode = RotateMode::Center;
            }
            else {
                rotateMode = RotateMode::None;
            }

            std::shared_ptr<Point> cp = scene.cursorPosition;
            pointsDirty |= ImGui::DragFloat("Cursor X", &cp->position.x, 0.1f);
            pointsDirty |= ImGui::DragFloat("Cursor Y", &cp->position.y, 0.1f);
            pointsDirty |= ImGui::DragFloat("Cursor Z", &cp->position.z, 0.1f);

            ImGui::Separator();
            ImGui::TextDisabled("Wspolrzedne ekranowe");

            float depth;
            Vector2 sc = camera.worldToScreen(
                cp->position, P,
                panelWidth, 0, W - panelWidth, H,
                depth
            );

            
            static float editSX = sc.x, editSY = sc.y;
            bool changed = false;
            changed |= ImGui::DragFloat("Screen (X)", &editSX, 1.0f, (float)panelWidth, (float)W);
            changed |= ImGui::DragFloat("Screen (Y)", &editSY, 1.0f, 0.0f, (float)H);
            if (changed) {
                cp->position = camera.screenToWorld(editSX, editSY, W, H, panelWidth, P);
                scene.cursorFollow = false;  
            }
            else {
                editSX = sc.x;
                editSY = sc.y;
            }

            ImGui::TreePop();
        }
        ImGui::PopID();
    }

    for (const auto& obj : scene.objects) {
        bool isChosen = scene.isSelected(obj);
        ImGui::PushID(obj->toHashCode().c_str());

        if (ImGui::Checkbox("##check", &isChosen)) {
            if (isChosen) {
                scene.select(obj);
                pointsDirty = scene.chosen.size() > 1;
            }
            else {
                pointsDirty = true;
                scene.deselect(obj);
            }
        }
        ImGui::SameLine();
        static std::unordered_map<std::string, bool> editingMap;
        bool& editing = editingMap[obj->toHashCode()];

        bool nodeOpen = ImGui::TreeNode(("##node" + obj->toHashCode()).c_str(), "%s", obj->getName().c_str());
        obj->isHighlightedUI = ImGui::IsItemHovered();

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
            editing = true;

        ImGui::SameLine(ImGui::GetContentRegionAvail().x - 20);
        if (ImGui::SmallButton("x"))
            toRemove = obj;

        if (nodeOpen) {
            char buf[128];
            strncpy_s(buf, obj->getName().c_str(), sizeof(buf));
            ImGui::SetNextItemWidth(200);
            if (ImGui::InputText("Nazwa", buf, sizeof(buf), ImGuiInputTextFlags_EnterReturnsTrue)) {
                obj->setName(buf);
            }

            if (auto torus = std::dynamic_pointer_cast<Torus>(obj)) {
                camera.targetGoal = -torus->transform.translation;

                bool rebuildVerts = false, rebuildIndices = false;

                rebuildVerts |= ImGui::DragFloat("R", &torus->R, 0.1f, 0.2f, 20.0f);
                torus->r = std::min(torus->r, torus->R - 0.01f);
                rebuildVerts |= ImGui::DragFloat("r", &torus->r, 0.1f, 0.1f, torus->R - 0.01f);
                rebuildIndices |= ImGui::DragInt("Dokladnosc", &torus->sides, 1, 2, 120);

                ImGui::DragFloat("Obrot X", &torus->transform.rotation.x, 0.1f, 0, 2 * M_PI);
                ImGui::DragFloat("Obrot Y", &torus->transform.rotation.y, 0.1f, 0, 2 * M_PI);
                ImGui::DragFloat("Obrot Z", &torus->transform.rotation.z, 0.1f, 0, 2 * M_PI);

                rebuildVerts |= ImGui::DragFloat("X", &torus->transform.translation.x, 0.1f, -100, 100)
                    || ImGui::DragFloat("Y", &torus->transform.translation.y, 0.1f, -100, 100)
                    || ImGui::DragFloat("Z", &torus->transform.translation.z, 0.1f, -100, 100);

                float scale = torus->transform.scale.x;
                if (ImGui::DragFloat("Zoom", &scale, 0.1f, 0.1f, 10.0f))
                    torus->transform.scale = Vector3(scale, scale, scale);

                if (rebuildVerts || rebuildIndices) {
                    for (auto& s : scene.shaders) {
                        if (auto ts = std::dynamic_pointer_cast<TorusShader>(s)) {
                            if (&ts->torus == torus.get()) {
                                ts->Rebuild(rebuildIndices);
                                break;
                            }
                        }
                    }
                }
            }

            if (auto point = std::dynamic_pointer_cast<Point>(obj)) {
                camera.targetGoal = -point->position;
                
                pointsDirty |= ImGui::DragFloat("X", &point->position.x, 0.1f, -100, 100)
                    || ImGui::DragFloat("Y", &point->position.y, 0.1f, -100, 100)
                    || ImGui::DragFloat("Z", &point->position.z, 0.1f, -100, 100);
            }
            ImGui::TreePop();
        }

        ImGui::PopID();
    }

    if (toRemove) {
        pointsDirty = true;
        scene.removeObject(toRemove);
    }
}

void Panel::RenderPanel(int panelWidth, int H, int W, Matrix4x4& P, Camera& camera, Scene& scene, bool& pointsDirty)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiStyle& style = ImGui::GetStyle();
    style.ItemSpacing = ImVec2(8, 12);
    style.FramePadding = ImVec2(6, 6);
    style.WindowPadding = ImVec2(14, 14);
    style.GrabMinSize = 12.0f;
    style.FrameRounding = 1.0f;

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(panelWidth, H), ImGuiCond_Always);
    ImGui::Begin("Sterowanie", nullptr,
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus
    );

    renderCameraSection(camera);
    ImGui::Separator();
    renderAddSection(scene);
    ImGui::Separator();
    renderObjectList(scene, pointsDirty, camera, panelWidth, H, W, P);
    ImGui::End();
}
