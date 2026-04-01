#define _USE_MATH_DEFINES

#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <vector>
#include <cmath>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <memory>
#include <algorithm>

#include "Scene.h"
#include "math/Matrix4x4.h"
#include "shaders/main/MainShader.h"
#include "shaders/torus/TorusShader.h"
#include "Nameof.h"
#include "model/Torus.h"
#include "model/TransformableObject.h"
#include "model/Point.h"
#include "Grid.h"
#include "KeyboardController.h"
#include "Camera.h"
#include "shaders/primitive/PrimitiveShader.h"
#include "MouseController.h"
#include "math/Vector2.h"
#include "UI.h"

// ─── Forward declarations ───────────────────────────────────────────────────
GLFWwindow* initWindow(int& H, int& W);
void        UpdateScreenSize(int& H, int& W);
bool        handleResize(GLFWwindow* win, int panelWidth, int& W, int& H, Matrix4x4& P, GLuint tex);
void        handleAddOnClick(Scene& scene, Matrix4x4& P, Camera& camera, int W, int H, int panelWidth, bool& pointsDirty);
void        applyMouseToSelected(GLFWwindow* window, Scene& scene, bool& pointsDirty);
void        handleTransformable(TransformableObject* t, GLFWwindow* window);
void        handlePoint(Point* p, GLFWwindow* window, bool& pointsDirty);
void        handleCursorPlacement(Scene& scene, const Matrix4x4& P, Camera& camera, int W, int H, int panelWidth);
bool        handleSelectionOnClick(Scene& scene, const Matrix4x4& P, Camera& camera, int W, int H, int panelWidth);
float       wrapAngle(float angle);

// ─── Main ───────────────────────────────────────────────────────────────────
int main() {
    int W, H, panelWidth = 350;
    GLFWwindow* win;

    if ((win = initWindow(H, W)) == nullptr) return -1;

    std::shared_ptr<Camera> camera = std::make_shared<Camera>();
    KeyboardController keyboardController(*camera);

    Matrix4x4 P = Matrix4x4::projection(float(W - panelWidth) / H, 100.0f, 0.5f, 60.0f * (float)(M_PI / 180.0));

    // Quad VAO (używany przez mainShader do renderowania siatki)
    unsigned int VAO, VBO;
    {
        float verts[] = {
            -1, 1, 0,1,  -1,-1, 0,0,  1,-1, 1,0,
            -1, 1, 0,1,   1,-1, 1,0,  1, 1, 1,1
        };
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
    }

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Grid grid(10);
    Matrix4x4 Mgrid = Matrix4x4::Identity();
    MainShader mainShader("./shaders/main/vs.glsl", "./shaders/main/fs.glsl", P, *camera, Mgrid, grid);

    auto primitiveShader = std::make_shared<PrimitiveShader>(P, *camera);

    Scene scene;
    scene.addShader(primitiveShader);

    // Tekstura dla offscreen (używana przy resize)
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glViewport(panelWidth, 0, W - panelWidth, H);

    float lastTime = 0.0f;
    bool  pointsDirty = false;

    // ─── Główna pętla ──────────────────────────────────────────────────────
    while (!glfwWindowShouldClose(win)) {
        const float currentTime = (float)glfwGetTime();
        const float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        camera->update(deltaTime);
        MouseController::g_scrolled = false;

        handleResize(win, panelWidth, W, H, P, tex);

        // Interakcja z zaznaczonymi obiektami (musi być przed flush)
        applyMouseToSelected(win, scene, pointsDirty);

        // Kursor 3D i wykrywanie najbliższego punktu
        handleCursorPlacement(scene, P, *camera, W, H, panelWidth);
        scene.findClosestPoint(P, *camera, panelWidth, W, H);

        // Obsługa kliknięcia – najpierw próba zaznaczenia, potem dodawania
        if (MouseController::g_leftClicked) {
            const bool consumed = handleSelectionOnClick(scene, P, *camera, W, H, panelWidth);
            if (!consumed)
                handleAddOnClick(scene, P, *camera, W, H, panelWidth, pointsDirty);
        }

        MouseController::flush();

        // UI
        Panel::RenderPanel(panelWidth, H, W, P, *camera, scene, pointsDirty);

        // Synchronizacja flagi isHighlighted (OR z UI i viewport)
        for (auto& obj : scene.objects) {
            const bool highlight = obj->isHighlightedUI || obj->isHighlightedViewport;
            obj->isHighlighted = highlight;
        }

        // Przebudowa bufora prymitywów (punkty, środek, kursor)
        primitiveShader->Rebuild(scene.objects, scene.getCenterPoint(), scene.cursorPosition);
        pointsDirty = false;

        // ─── Render ───────────────────────────────────────────────────────
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        scene.draw();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        mainShader.Update();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}

// ─── Inicjalizacja okna ─────────────────────────────────────────────────────
GLFWwindow* initWindow(int& H, int& W) {
    if (!glfwInit()) return nullptr;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    UpdateScreenSize(H, W);
    GLFWwindow* win = glfwCreateWindow(W, H, "Interfejs do interakcji w przestrzeni wirtualnej", nullptr, nullptr);
    if (!win) { glfwTerminate(); return nullptr; }

    glfwGetWindowSize(win, &W, &H);
    glfwMakeContextCurrent(win);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return nullptr;

    glfwSetMouseButtonCallback(win, MouseController::onSetMouseButtonCallback);
    glfwSetCursorPosCallback(win, MouseController::onSetCursorPosCallback);
    glfwSetScrollCallback(win, MouseController::onScrollCallback);
    glfwSetKeyCallback(win, InputManager::key_callback);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(win, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    return win;
}

void UpdateScreenSize(int& H, int& W) {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    H = mode->height;
    W = mode->width;
}

// ─── Obsługa resize ─────────────────────────────────────────────────────────
bool handleResize(GLFWwindow* win, int panelWidth, int& W, int& H, Matrix4x4& P, GLuint tex) {
    int newW, newH;
    glfwGetFramebufferSize(win, &newW, &newH);
    if (newW == W && newH == H) return false;

    W = newW; H = newH;
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glViewport(panelWidth, 0, W - panelWidth, H);
    P = Matrix4x4::projection(float(W - panelWidth) / H, 100.0f, 0.5f, 60.0f * (float)(M_PI / 180.0));
    return true;
}

// ─── Dodawanie obiektów w miejscu kursora ───────────────────────────────────
void handleAddOnClick(Scene& scene, Matrix4x4& P, Camera& camera,
    int W, int H, int panelWidth, bool& pointsDirty)
{
    if (Panel::addMode == AddMode::None)      return;
    if (!MouseController::g_leftClicked)       return;
    if (MouseController::g_cursorX < panelWidth) return;

    const Vector3& cursorPos = scene.cursorPosition->position;

    if (Panel::addMode == AddMode::Point) {
        scene.addObject(std::make_shared<Point>(cursorPos));
        pointsDirty = true;
    }
    else if (Panel::addMode == AddMode::Torus) {
        auto torus = std::make_shared<Torus>(1.0f, 0.4f, 8,
            Transform(cursorPos, Vector3(), Vector3(1, 1, 1)));
        auto shader = std::make_shared<TorusShader>(P, camera, *torus);
        shader->Rebuild(true);
        scene.addObject(torus);
        scene.addShader(shader);
    }
}

// ─── Transformacje zaznaczonych obiektów przy użyciu myszy ─────────────────
void applyMouseToSelected(GLFWwindow* window, Scene& scene, bool& pointsDirty) {
    if (scene.chosen.empty()) return;

    Panel::addMode = AddMode::None;

    const float  mult = 0.005f;
    const bool   ctrl = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
    const size_t n = scene.chosen.size();

    // ── Tryb lokalny: jeden obiekt, obrót wokół siebie ──────────────────────
    if (n == 1 && Panel::rotateMode != RotateMode::Cursor) {
        auto& obj = scene.chosen[0];
        if (auto t = std::dynamic_pointer_cast<TransformableObject>(obj))
            handleTransformable(t.get(), window);
        else if (auto p = std::dynamic_pointer_cast<Point>(obj))
            handlePoint(p.get(), window, pointsDirty);
        return;
    }

    // ── Tryb grupowy: obrót/skalowanie wokół wspólnego pivotu ───────────────
    auto pivotPoint = (Panel::rotateMode == RotateMode::Cursor)
        ? scene.cursorPosition
        : scene.getCenterPoint();
    if (!pivotPoint) return;

    const Vector3& pivot = pivotPoint->position;

    // Rotacja – LPM
    if (MouseController::g_leftDown) {
        const float dx = MouseController::g_dx * mult;
        const float dy = MouseController::g_dy * mult;
        const float ax = ctrl ? 0.0f : -dy;
        const float ay = ctrl ? 0.0f : -dx;
        const float az = ctrl ? -dx : 0.0f;

        for (auto& obj : scene.chosen)
            obj->transformAroundPoint(ax, ay, az, 1.0f, 1.0f, 1.0f, pivot);

        pointsDirty = true;
    }

    // Skalowanie – scroll kółkiem
    if (MouseController::g_scrolled >= 0) {
        // Mnożnik: scroll +1 → ×1.1, scroll -1 → ×0.9
        float s = 1.0f + MouseController::g_scroll * 0.1f;
        s = std::clamp(s, 0.05f, 2.0f);

        for (auto& obj : scene.chosen)
            obj->transformAroundPoint(0.0f, 0.0f, 0.0f, s, s, s, pivot);

        pointsDirty = true;
    }

    // Przesuwanie wspólne – PPM (każdy obiekt przesuwa się lokalnie)
    if (MouseController::g_rightDown) {
        for (auto& obj : scene.chosen) {
            if (auto t = std::dynamic_pointer_cast<TransformableObject>(obj))
                handleTransformable(t.get(), window);
            else if (auto p = std::dynamic_pointer_cast<Point>(obj))
                handlePoint(p.get(), window, pointsDirty);
        }
    }
}

// ─── Transformacje lokalne – torus / obiekt z Transform ────────────────────
void handleTransformable(TransformableObject* transformable, GLFWwindow* window) {
    const float mult = 0.005f;
    const bool  ctrl = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
    Transform& t = transformable->transform;

    if (MouseController::g_leftDown) {
        if (ctrl)
            t.rotation.z = wrapAngle(t.rotation.z + MouseController::g_dx * mult);
        else {
            t.rotation.y = wrapAngle(t.rotation.y - MouseController::g_dx * mult);
            t.rotation.x = wrapAngle(t.rotation.x - MouseController::g_dy * mult);
        }
    }

    if (MouseController::g_rightDown) {
        if (ctrl) t.translation.z += MouseController::g_dy * mult * 2.0f;
        else {
            t.translation.x += MouseController::g_dx * mult * 2.0f;
            t.translation.y -= MouseController::g_dy * mult * 2.0f;
        }
    }

    if (MouseController::g_scrolled) {
        const float s = std::clamp(t.scale.x + MouseController::g_scroll * 0.1f, 0.1f, 10.0f);
        t.scale = Vector3(s, s, s);
    }
}

// ─── Transformacje lokalne – punkt ─────────────────────────────────────────
void handlePoint(Point* point, GLFWwindow* window, bool& pointsDirty) {
    const float mult = 0.005f;
    const bool  ctrl = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;

    if (MouseController::g_leftDown) {
        if (ctrl)
            point->position.z -= MouseController::g_dy * mult;
        else {
            point->position.x += MouseController::g_dx * mult;
            point->position.y -= MouseController::g_dy * mult;
        }
        pointsDirty = true;
    }
}

// ─── Ustawianie kursora 3D ──────────────────────────────────────────────────
void handleCursorPlacement(Scene& scene, const Matrix4x4& P, Camera& camera,
    int W, int H, int panelWidth)
{
    if (MouseController::g_cursorX < panelWidth) return;
    if (!scene.cursorFollow) return;

    scene.setCursor(camera.screenToWorld(
        MouseController::g_cursorX, MouseController::g_cursorY,
        W, H, panelWidth, P
    ), MouseController::g_leftClicked);
}

// ─── Zaznaczanie przez kliknięcie ──────────────────────────────────────────
bool handleSelectionOnClick(Scene& scene, const Matrix4x4& P, Camera& camera,
    int W, int H, int panelWidth)
{
    if (MouseController::g_cursorX < panelWidth) return false;

    auto best = scene.closestPoint;
    if (!best) return false;

    const bool ctrl = glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
    const bool shift = glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

    if (ctrl) {
        if (scene.isSelected(best)) scene.deselect(best);
        else                         scene.select(best);
    }
    else if (shift) {
        scene.select(best);
    }
    else {
        // Wyczyść poprzednie zaznaczenie i zaznacz kliknięty
        auto prev = scene.chosen;
        for (auto& s : prev) scene.deselect(s);
        scene.select(best);
    }

    return true;
}

// ─── Pomocnicze ────────────────────────────────────────────────────────────
float wrapAngle(float angle) {
    return fmod(fmod(angle, 2.0f * (float)M_PI) + 2.0f * (float)M_PI, 2.0f * (float)M_PI);
}
