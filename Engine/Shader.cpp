#include "Precompiled.h"
#include "Shader.h"
#include <sstream>
#include <fstream>
#include "../include/glm/gtc/type_ptr.hpp"
#include <iostream>
void Shader::CreateShaders(const char* vertexPath, const char* fragPath)
{
	std::string vertCode, fragCode;
	std::ifstream vertFile, fragFile;
	vertFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		vertFile.open(vertexPath);
		fragFile.open(fragPath);
		std::stringstream vertStream, fragStream;
		vertStream << vertFile.rdbuf();
		fragStream << fragFile.rdbuf();
		vertFile.close();
		fragFile.close();
		vertCode = vertStream.str();
		fragCode = fragStream.str();
	}
	catch (std::ifstream::failure e)
	{
		printf("Failed to open file");
	}
	int result;
	char buffer[1024];
	unsigned short vertex, fragment;
	const char* vertexCode = vertCode.c_str();
	const char* fragmentCode = fragCode.c_str();
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexCode, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertex, 1024, NULL, buffer);
		std::cout << "Vertex shader failed\n" << buffer << std::endl;
	}
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragment, 1024, NULL, buffer);
		std::cout << "Fragment shader failed\n" << buffer << std::endl;
	}
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(ID, 1024, NULL, buffer);
		std::cout << "Shader program link error:\n" << buffer << std::endl;
	}
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::Use()
{
	glUseProgram(ID);
}

void Shader::Delete()
{
	glDeleteProgram(ID);
}

void Shader::BindMat4(const std::string& name, glm::mat4 mat)
{
	unsigned int bindID = glGetUniformLocation(ID, name.c_str());
	glUniformMatrix4fv(bindID, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::BindVec3(const std::string& name, glm::vec3 vec)
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(vec));
}

void Shader::BindVec2(const std::string& name, glm::vec3 vec)
{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(vec));
}

void Shader::BindInt(const std::string& name, int id)
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), id);
}

void Shader::BindFloat(const std::string& name, float id)
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), id);
}

Shader::Shader(const char* vertexPath, const char* fragPath)
{
	CreateShaders(vertexPath, fragPath);
}

void Shader::BindBuffer(const std::string& name, const void* buffer, const size_t size)
{
	unsigned int bindID = glGetUniformLocation(ID, name.c_str());
	//std::cout << "uniform name: " << name << std::endl;
	std::cout << "buffer id: " << bindID << std::endl;
	glBindBuffer(GL_UNIFORM_BUFFER, bindID);
	glBufferData(GL_UNIFORM_BUFFER, size, buffer, GL_DYNAMIC_READ);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}