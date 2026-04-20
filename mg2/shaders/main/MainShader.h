#pragma once

#include "shaders/Shader.h"
#include "Nameof.h"
#include "view/Grid.h"
#include "view/Camera.h"

class MainShader : public Shader {
private:
	GLint uP, uV, uM;
	Matrix4x4& P;
	Camera& camera;
	Matrix4x4& M;
	GLuint VAO, VBO;
	Grid& grid;
public: 
	MainShader(
		const std::string vertexCodePath, 
		const std::string fragmentCodePath, 
		Matrix4x4& P, 
		Camera& camera,
		Matrix4x4& M,
		Grid& grid
	) : Shader(vertexCodePath, fragmentCodePath), P(P), M(M), camera(camera), grid(grid) {
		uP = getUniform(nameof(uP));
		uV = getUniform(nameof(uV));
		uM = getUniform(nameof(uM));

		grid.generate();

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glGenBuffers(1, &VBO);

		// dane wierzchołków
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(
			GL_ARRAY_BUFFER,
			grid.vertices.size() * sizeof(float),
			grid.vertices.data(),
			GL_STATIC_DRAW
		);

		glVertexAttribPointer(
			0,  // layout(location = 0) aPos;
			3,
			GL_FLOAT,
			GL_FALSE,
			3 * sizeof(float), (void*)0
		);

		glEnableVertexAttribArray(0);
	}

	void Update() override {
		glUseProgram(program);
		glUniformMatrix4fv(uP, 1, GL_TRUE, P.toFloatArray().data());
		glUniformMatrix4fv(uV, 1, GL_TRUE, camera.view().toFloatArray().data());
		glUniformMatrix4fv(uM, 1, GL_TRUE, M.toFloatArray().data());
	}

	void draw() {
		glUseProgram(program);
		glBindVertexArray(VAO);
		glDrawArrays(GL_LINES, 0, grid.vertices.size() / 3);
	}
};