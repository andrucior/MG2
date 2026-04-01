#include "Scene.h"
#include "MouseController.h"
#include <imgui/imgui.h>

void Scene::removeObject(std::shared_ptr<SceneObject> obj) {
    deselect(obj);

    objects.erase(
        std::remove(objects.begin(), objects.end(), obj),
        objects.end()
    );

    shaders.erase(
        std::remove_if(shaders.begin(), shaders.end(), [&](const std::shared_ptr<Shader>& s) {
            return s->ownsObject(obj.get());
            }),
        shaders.end()
    );
}

std::shared_ptr<Point> Scene::getCenterPoint() const {
    if (chosen.size() <= 1) return nullptr;
    Vector3 avg;
    for (const auto& obj : chosen) {
        if (auto p = std::dynamic_pointer_cast<Point>(obj))
            avg = avg + p->position;
        else if (auto t = std::dynamic_pointer_cast<TransformableObject>(obj))
            avg = avg + t->transform.translation;
    }
    return std::make_shared<Point>(avg / chosen.size(), true);
}

void Scene::setCursor(Vector3 pos, bool clicked) {
    if (cursorFollow) {
        cursorPosition->position = pos;
    }
    
    if (clicked) {
        cursorFollow = false;
    }
}

void Scene::draw() {
    for (auto& shader : shaders) {
        shader->Update();
    }
}

void Scene::findClosestPoint(Matrix4x4 P, Camera camera, int panelWidth, int W, int H) {
    const float pointThresholdPx = 10.0f;
    Vector2 mousePos(MouseController::g_cursorX, MouseController::g_cursorY);

    if (MouseController::g_cursorX < panelWidth || ImGui::GetIO().WantCaptureMouse) {
        if (closestPoint) {
            closestPoint->isHighlighted = false;
            closestPoint.reset();
        }
        return;
    }

    std::shared_ptr<Point> best = nullptr;
    float bestDepth = FLT_MAX;

    for (const auto& obj : objects) {
        auto pt = std::dynamic_pointer_cast<Point>(obj);
        if (!pt) continue;

        float depth;
        Vector2 screen = camera.worldToScreen(pt->position, P, panelWidth, 0, W - panelWidth, H, depth);

        float dx = screen.x - mousePos.x;
        float dy = screen.y - mousePos.y;
        float d = std::sqrt(dx * dx + dy * dy);
        if (d <= pointThresholdPx) {
            if (depth < bestDepth) {
                best = pt;
                bestDepth = depth;
            }
        }
    }
    
    if (best != closestPoint) {
        if (closestPoint) {
            closestPoint->isHighlightedViewport = false;
        }
        closestPoint = best;
        if (closestPoint) closestPoint->isHighlightedViewport = true;
    }
}

void Scene::select(std::shared_ptr<SceneObject> obj) {
    obj->isSelected = true;
    if (!isSelected(obj))
        chosen.push_back(obj);
}

void Scene::deselect(std::shared_ptr<SceneObject> obj) {
    obj->isSelected = false;
    chosen.erase(std::remove(chosen.begin(), chosen.end(), obj), chosen.end());
}