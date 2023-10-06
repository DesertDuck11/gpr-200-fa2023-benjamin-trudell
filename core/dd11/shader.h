#pragma once
#include <sstream>
#include <fstream>
#include "../ew/external/glad.h"
#include "../ew/ewMath/mat4.h"
#include "../ew/ewMath/vec3.h"

namespace dd11 {
	std::string loadShaderSourceFromFile(const std::string& filePath);

	class Shader {
	public:
		Shader(const std::string& vertexShader, const std::string& fragmentShader);
		void use();
		void setInt(const std::string& name, int v) const;
		void setFloat(const std::string& name, float v) const;
		void setVec2(const std::string& name, float x, float y) const;
		void setVec3(const std::string& name, float x, float y, float z) const;
		void setVec4(const std::string& name, float x, float y, float z, float w) const;
		void setVec3Array(const std::string& name, int arrLength, float arr[][3]) const;
		void Shader::setMat4(const std::string& name, const ew::Mat4& v) const;
	private:
		unsigned int m_id; //OpenGL program handle
	};
}
