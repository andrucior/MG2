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

#include "LightController.h"
#include "Scene.h"
#include "math/Matrix4x4.h"
#include "MouseController.h";
#include "shaders/Shader.h";
#include "shaders/TorusShader.h";
#include "Nameof.h";
#include "Torus.h";

GLFWwindow* initWindow(int& H, int& W);
void UpdateScreenSize(int& H, int& W);

int main() {
    int W, H;
    GLFWwindow* win;
    
    if ((win = initWindow(H, W)) == nullptr) return -1;

    // Fullscreen
    float verts[] = {
        -1, 1, 0,1,  -1,-1, 0,0,  1,-1, 1,0,
        -1, 1, 0,1,   1,-1, 1,0,  1, 1, 1,1
    };
    Matrix4x4 M, P = Matrix4x4::projection(float(W) / H, 20.0f, 0.5f, 60.0f * (M_PI / 180.0f)), V;

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO); glGenBuffers(1, &VBO);
    glBindVertexArray(VAO); glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);          
    glEnableVertexAttribArray(0);
    float R = 1, r = 0.4;
    int sides = 8;

    Torus torus = Torus(R, r, sides);
    TorusShader torusShader = TorusShader("./Shaders/vs_torus.glsl", "./Shaders/fs_torus.glsl", P, M, V, torus);

    GLuint tex;
    glGenTextures(1, &tex); 
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    
    Vector3 translation = Vector3(MouseController::g_tx, MouseController::g_ty, MouseController::g_tz);
    Vector3 rotation = Vector3(MouseController::g_rotX, MouseController::g_rotY, MouseController::g_rotZ);
    Vector3 scale = Vector3(MouseController::g_zoom, MouseController::g_zoom, MouseController::g_zoom);
    Transform transform = Transform(translation, rotation, scale);
    Scene scene = Scene(transform);
    
    bool changed = true, shouldRebuildVerts = true, shouldRebuildIndices = true;

    while (!glfwWindowShouldClose(win)) {
        bool isInteracting = MouseController::g_leftDown || 
            MouseController::g_rightDown || 
            ImGui::IsAnyItemActive() ||
            MouseController::g_scrolled;
        
        MouseController::g_scrolled = false;
        
        int newW, newH;
        glfwGetFramebufferSize(win, &newW, &newH);

        if (newW != W || newH != H) {
            W = newW;
            H = newH;

            glBindTexture(GL_TEXTURE_2D, tex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
            glViewport(0, 0, W, H);

            changed = true;
            float aspect = float(W) / H;
            P = Matrix4x4::projection(aspect, 20.0f, 0.5f, 60.0f * (M_PI / 180.0f));
        }
      
        ImGui_ImplOpenGL3_NewFrame(); 
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Sterowanie");
        
        shouldRebuildVerts |= ImGui::SliderFloat("Promien (R)", &R, 0.1f, 20.0f);
        
        r = std::min(r, R - 0.01f);

        shouldRebuildVerts |= ImGui::SliderFloat("Promien (r)", &r, 0.1f, R - 0.01f);
        changed |= ImGui::SliderFloat("Obrot (OX)", &MouseController::g_rotX, 0, 2 * M_PI);
        changed |= ImGui::SliderFloat("Obrot (OY)", &MouseController::g_rotY, 0, 2 * M_PI);
        changed |= ImGui::SliderFloat("Obrot (OZ)", &MouseController::g_rotZ, 0, 2 * M_PI);
        shouldRebuildIndices |= ImGui::SliderInt("Dokladnosc", &sides, 2, 120);
        changed |= ImGui::SliderFloat("Zoom", &MouseController::g_zoom, 0.1f, 10.0f);
        ImGui::Text("Obrot: Lewy przycisk myszy (dla osi Z + L Ctrl)");
        ImGui::Text("Przesuniecie: Prawy przycisk myszy");
        ImGui::End();

        if (changed || isInteracting) {
            rotation = Vector3(MouseController::g_rotX, MouseController::g_rotY, MouseController::g_rotZ);
            translation = Vector3(MouseController::g_tx, MouseController::g_ty, MouseController::g_tz);
            scale = Vector3(MouseController::g_zoom, MouseController::g_zoom, MouseController::g_zoom);

            M = scene.modelMatrix();
            V = scene.view();
        }

        if (shouldRebuildVerts || shouldRebuildIndices) {
            torusShader.Rebuild(shouldRebuildIndices);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS); 

        // Rasteryzator
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        torusShader.Update();
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        changed = false;
        shouldRebuildIndices = false;
        shouldRebuildVerts = false;

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

GLFWwindow* initWindow(int& H, int& W) {
    if (!glfwInit()) return nullptr;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    UpdateScreenSize(H, W);

    GLFWwindow* win = glfwCreateWindow(
        W, 
        H, 
        "Interfejs do interakcji w przestrzeni wirtualnej", 
        nullptr,
        nullptr
    );

    if (!win) {
        glfwTerminate();
        return nullptr;
    }

    glfwGetWindowSize(win, &W, &H);
    glfwMakeContextCurrent(win);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return nullptr;

    glfwSetMouseButtonCallback(win, MouseController::onSetMouseButtonCallback);
    glfwSetCursorPosCallback(win, MouseController::onSetCursorPosCallback);
    glfwSetScrollCallback(win, MouseController::onScrollCallback);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(win, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    return win;
}

void UpdateScreenSize(int& H, int& W) {
    GLFWmonitor* return_struct = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(return_struct);

    H = mode->height;
    W = mode->width;
}