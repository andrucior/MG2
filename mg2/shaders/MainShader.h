//#pragma once
//
//#include "Shader.h"
//#include "Nameof.h";
//
//class MainShader : public Shader {
//private:
//	GLint uPinv, uMinv, uP;
//	Matrix4x4& Pinv;
//	Matrix4x4& Minv;
//	Matrix4x4& P;
//	unsigned int VAO;
//public: 
//	MainShader(
//		const std::string vertexCodePath, 
//		const std::string fragmentCodePath, 
//		Matrix4x4& Pinv,
//		Matrix4x4& Minv, 
//		Matrix4x4& P, 
//		unsigned int VAO
//	) : Shader(vertexCodePath, fragmentCodePath), Pinv(Pinv), Minv(Minv), P(P), VAO(VAO) {
//		uPinv = getUniform(nameof(uPinv));
//		uMinv = getUniform(nameof(uMinv));
//		uP = getUniform(nameof(uP));
//	}
//	
//	void Update() override {
//		
//		glUseProgram(program);
//		glUniformMatrix4fv(uPinv, 1, GL_TRUE, Pinv.toFloatArray().data());
//		glUniformMatrix4fv(uMinv, 1, GL_TRUE, Minv.toFloatArray().data());
//		glUniformMatrix4fv(uP, 1, GL_TRUE, P.toFloatArray().data());
//		glBindVertexArray(VAO);
//		glDrawArrays(GL_TRIANGLES, 0, 6);
//		
//	}
//};