#include "Shader.h"


#include <istream>
#include <fstream>
#include <string>
#include <streambuf>
#include <iostream>

using namespace std;

#define ERROR_SIZE 1024





static GLuint getShader(LPCSTR path, GLenum type)
{
	ifstream file(path, ios::in);
	if (file.fail()) throw errno;

	string content;
	file.seekg(0, ios::end);
	content.resize(file.tellg());
	file.seekg(0, ios::beg);
	file.read(const_cast<char*>(content.c_str()), content.size());

	auto shader = glCreateShader(type);
	auto source = content.c_str(); //Need a variable to reference; cannot reference a ret-val
	glShaderSource(shader, 1, &source, 0);
	glCompileShader(shader);

	GLint hasCompiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
	if (!hasCompiled) {
		string infoLog;
		infoLog.resize(ERROR_SIZE);
		glGetShaderInfoLog(shader, ERROR_SIZE, nullptr,
			const_cast<char*>(infoLog.c_str()));
		MessageBoxA(nullptr, infoLog.c_str(), "Shader Error", 
			MB_ABORTRETRYIGNORE);
	} 
	return shader;
}

static GLuint createProgram(LPCSTR vertPath, LPCSTR fragPath, LPCSTR geomPath) {
	auto id = glCreateProgram();

	auto vertShader = getShader(vertPath, GL_VERTEX_SHADER);
	glAttachShader(id, vertShader);
	auto fragShader = getShader(fragPath, GL_FRAGMENT_SHADER);
	glAttachShader(id, fragShader);
	GLuint geomShader;
	if (geomPath)
	{
		geomShader = getShader(geomPath, GL_GEOMETRY_SHADER);
		glAttachShader(id, geomShader);
	}
	glLinkProgram(id);
	GLint hasLinked;
	glGetProgramiv(id, GL_LINK_STATUS, &hasLinked);
	if (!hasLinked) {
		string infoLog;
		infoLog.resize(ERROR_SIZE);
		glGetProgramInfoLog(id, ERROR_SIZE, 0,
			const_cast<char*>(infoLog.c_str()));
		cout << "Shader Linking error:" << infoLog;
		//abort();
	}


	return id;
}


void Shader::init(LPCSTR vertPath, LPCSTR fragPath, LPCSTR geomPath)
{
	id = createProgram(vertPath, fragPath, geomPath);
}
Shader::Shader(LPCSTR vertPath, LPCSTR fragPath, LPCSTR geomPath) 
{
	init(vertPath, fragPath, geomPath);
}





Shader::Shader() {
	id = 0;
}

Shader::~Shader() {
	glDeleteProgram(id);
}