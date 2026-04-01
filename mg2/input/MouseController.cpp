#include "MouseController.h"

void MouseController::onSetCursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (ImGui::GetIO().WantCaptureMouse) return;
    g_cursorX = (float)xpos;
    g_cursorY = (float)ypos;
    g_dx = (xpos - g_lastX) * multiplicator;
    g_dy = (ypos - g_lastY) * multiplicator;
    g_lastX = xpos * multiplicator;
    g_lastY = ypos * multiplicator;
}

void MouseController::onSetMouseButtonCallback(GLFWwindow*, int btn, int action, int)
{
    if (ImGui::GetIO().WantCaptureMouse) {
        g_leftDown = g_rightDown = false;
        return;
    }
    if (btn == GLFW_MOUSE_BUTTON_LEFT)  g_leftDown = (action == GLFW_PRESS);
    if (btn == GLFW_MOUSE_BUTTON_RIGHT) g_rightDown = (action == GLFW_PRESS);
}

void MouseController::onScrollCallback(GLFWwindow*, double, double yoffset)
{
    g_scroll = (float)yoffset * 0.1f;
    g_scrolled = true;
}

void MouseController::flush()
{
    // Kliknięcie = był wciśnięty w poprzedniej klatce, teraz puszczony
    g_leftClicked = g_leftWasDown && !g_leftDown;
    g_leftWasDown = g_leftDown;
    g_dx = g_dy = g_scroll = 0;
    g_scrolled = false;
}