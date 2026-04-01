#pragma once

#include <glfw/glfw3.h>
#include <vector>
#include <functional>

#include "math/Vector3.h"
#include "Camera.h"

class InputManager {
public:
	// Lista funkcji, które chcą słuchać o kliknięciach
	static inline std::vector<std::function<void(int, int)>> listeners;

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		for (auto& listener : listeners) {
			listener(key, action);
		}
	}
};

class KeyboardController {
private: 
	Camera& camera;
public:
	KeyboardController(Camera& camera) : camera(camera) {
		InputManager::listeners.push_back([this](int key, int action) {
			key_callback(key, action);
		});
	}

	void key_callback(int key, int action) {
		switch (key) {
			case GLFW_KEY_UP:
				camera.cameraPosition.z += 0.01;

				break;
			case GLFW_KEY_DOWN:
				camera.cameraPosition.z -= 0.01;
				break;

			case GLFW_KEY_T:
				camera.cameraPosition.y += 0.01;
				break;

			case GLFW_KEY_Y:
				camera.cameraPosition.y -= 0.01;
				break;

			case GLFW_KEY_X:
				camera.cameraPosition.x += 0.01;
				break;

			case GLFW_KEY_C:
				camera.cameraPosition.x -= 0.01;
				break;
		}
	}
};
