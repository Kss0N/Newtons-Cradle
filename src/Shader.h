#pragma once
#include "framework.h"


/// @brief openGL Shader program wrapper
class Shader
{
	GLuint id;
public:
	constexpr operator GLuint() {
		return id;
	}

	/// @brief Shader constructor
	/// @param vertPath path for a file containing a vertex shader
	/// @param fragPath path for a fle containing a fragment shader
	/// @param geomPath (maybe null) path for a file containing a geometry shader
	Shader(LPCSTR vertPath, LPCSTR fragPath, LPCSTR geomPath = nullptr);

	void init(LPCSTR verPath, LPCSTR fragPath, LPCSTR geomPath = nullptr);

	Shader();

	~Shader();
};

