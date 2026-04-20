#pragma once

#include <vector>
#include <memory>
#include <algorithm>

#include "SceneObject.h"
#include "Point.h"
#include "Camera.h"
#include "Shader.h"
#include "Vector2.h"
#include "Matrix4x4.h"
#include "Torus/TorusShader.h"

class Scene {
public:
    std::vector<std::shared_ptr<SceneObject>> objects;
    std::vector<std::shared_ptr<SceneObject>> chosen;
    std::vector<std::shared_ptr<Shader>> shaders;

    std::shared_ptr<Point> cursorPosition;
    std::shared_ptr<Point> closestPoint;

    bool cursorExists = true;
    bool cursorFollow = true; 
    bool primitivesChanged = true;

    Scene() {
        cursorPosition = std::make_shared<Point>(Vector3());
    }

    void addObject(std::shared_ptr<SceneObject> obj) {
        objects.push_back(obj);
    }

    void addShader(std::shared_ptr<Shader> shader) {
        shaders.push_back(shader);
    }

    void removeObject(std::shared_ptr<SceneObject> obj);

    void select(std::shared_ptr<SceneObject> obj);

    void deselect(std::shared_ptr<SceneObject> obj);

    bool isSelected(const std::shared_ptr<SceneObject>& obj) const {
        return std::find(chosen.begin(), chosen.end(), obj) != chosen.end();
    }

    std::shared_ptr<Point> getCenterPoint() const;

    void setCursor(Vector3 pos, bool clicked);

    void draw();

    void findClosestPoint(Matrix4x4 P, Camera camera, int panelWidth, int W, int H);
};