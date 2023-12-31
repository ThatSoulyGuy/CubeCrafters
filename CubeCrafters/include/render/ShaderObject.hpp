#ifndef SHADER_OBJECT_HPP
#define SHADER_OBJECT_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "core/Logger.hpp"
#include "core/Settings.hpp"

std::string LoadFile(const std::string& path)
{
	std::ifstream shaderFile(path);
	std::stringstream shaderStream;

	if (!shaderFile.is_open())
	{
		Logger_ThrowError("nullptr", std::format("File failed to open: '{}'", path), false);
		return "";
	}

	shaderStream << shaderFile.rdbuf();
	shaderFile.close();

	return shaderStream.str();
}

class ShaderObject
{

public:
    std::string name = "";
    std::string vertexPath = "", fragmentPath = "";
    std::string vertexData = "", fragmentData = "";
    unsigned int shaderProgram = 1;

	void Generate()
	{
		unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexData);
		unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentData);

		LinkProgram(vertexShader, fragmentShader);

		int error = glGetError();

		if (error != GL_NO_ERROR)
			Logger_ThrowError(std::to_string(error), std::format("OpenGL error: {}", error), false);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	void Use()
	{
		glUseProgram(shaderProgram);
	}

	void SetUniform(const std::string& name, const bool& value) const
	{
		glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), (int)value);
	}

	void SetUniform(const std::string& name, const int& value) const
	{
		glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
	}

	void SetUniform(const std::string& name, const float& value) const
	{
		glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
	}

	void SetUniform(const std::string& name, const glm::vec2& value) const
	{
		glUniform2fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
	}

	void SetUniform(const std::string& name, const glm::vec3& value) const
	{
		glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
	}

	void SetUniform(const std::string& name, const glm::vec4& value) const
	{
		glUniform4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
	}

	void SetUniform(const std::string& name, const glm::mat2& mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void SetUniform(const std::string& name, const glm::mat3& mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void SetUniform(const std::string& name, const glm::mat4& mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void SetUniform(const std::string& name, float x, float y) const
	{
		SetUniform(name, glm::vec2(x, y));
	}

	void SetUniform(const std::string& name, float x, float y, float z) const
	{
		SetUniform(name, glm::vec3(x, y, z));
	}

	void SetUniform(const std::string& name, float x, float y, float z, float w) const
	{
		SetUniform(name, glm::vec4(x, y, z, w));
	}

	void CleanUp()
	{
		glDeleteProgram(shaderProgram);
	}

	static std::unique_ptr<ShaderObject> Register(const std::string& path, const std::string& name, const std::string& domain = Settings::defaultDomain)
	{
		std::unique_ptr<ShaderObject> out(new ShaderObject());

		out->name = name;

		out->vertexPath = "assets/" + domain + "/" + path + "Vertex.glsl";
		out->fragmentPath = "assets/" + domain + "/" + path + "Fragment.glsl";

		out->vertexData = LoadFile(out->vertexPath);
		out->fragmentData = LoadFile(out->fragmentPath);

		return out;
	}

private:

	unsigned int CompileShader(GLenum type, const std::string& source) const
	{
		unsigned int shader = glCreateShader(type);
		const char* src = source.c_str();
		glShaderSource(shader, 1, &src, nullptr);
		glCompileShader(shader);

		int success;
		char infoLog[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		int error = glGetError();

		if (error != GL_NO_ERROR)
			Logger_ThrowError(std::to_string(error), std::format("OpenGL error: {}", error), false);

		return shader;
	}

	void LinkProgram(unsigned int vertexShader, unsigned int fragmentShader)
	{
		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);

		int success;
		char infoLog[512];
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

		if (!success)
		{
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}

		int error = glGetError();

		if (error != GL_NO_ERROR)
			Logger_ThrowError(std::to_string(error), std::format("OpenGL error: {}", error), false);
	}
};

#endif // !SHADER_OBJECT_HPP