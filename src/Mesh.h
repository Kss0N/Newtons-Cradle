#pragma once
#include "math.h"
#include "framework.h"
#include <string>
#include <vector>
#include "Shader.h"



class Mesh
{
public:
	struct Vertex {
		PxVec3 pos;
		PxVec3 normal;
	};

	/// @brief Mesh contructor
	/// @param filePath to a Wavefront .Obj mesh file
	/// @param VAO a Vertex Attribute object, if none is supplied, the construtor
	/// creates one
	Mesh(std::string& filePath, GLuint VAO = 0);

	physx::PxTriangleMeshGeometry createPxGeometry(double scale);

	void draw(Shader& shader);

	/// @brief transfer the buffers to openGL
	void tansferBufferToGL(GLuint inVAO = 0);

private:
	//No textures for the moment; objects have colors instead
	PxVec4 RGBA;

	//Vertices
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
};

