#pragma once

#include "Scene.h"
#include "math/Matrix4x4.h"
#include "Camera.h"

enum class AddMode { None, Point, Torus };
enum class RotateMode { Center, Cursor, None };

class Panel {
private:
	static void renderCameraSection(Camera& camera);
	static void renderAddSection(Scene& scene);
	static void renderObjectList(Scene& scene, bool& pointsDirty, Camera& camera,
		int panelWidth, int H, int W, Matrix4x4 P);
	static void modeButton(const char* label, AddMode mode, Scene& scene);
public:
	static void RenderPanel(int panelWidth, int H, int W, Matrix4x4& P, Camera& camera, Scene& scene, bool& pointsDirty);
	static inline AddMode addMode = AddMode::None;
	static inline RotateMode rotateMode = RotateMode::None;
	static inline bool rotate = false;
};