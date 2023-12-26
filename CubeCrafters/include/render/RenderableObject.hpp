#ifndef RENDERABLE_OBJECT_HPP
#define RENDERABLE_OBJECT_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "math/Transform.hpp"
#include "record/NameIDTag.hpp"
#include "thread/MainThreadExecutor.hpp"
#include "render/ShaderManager.hpp"
#include "render/TextureManager.hpp"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 textureCoordinates;

	static Vertex Register(const glm::vec3& position, const glm::vec2& textureCoordinates)
	{
		return Vertex::Register(position, { 1.0f, 1.0f, 1.0f }, textureCoordinates);
	}

	static Vertex Register(const glm::vec3& position, const glm::vec3& color, const glm::vec2& textureCoordinates)
	{
		Vertex out = {};

		out.position = position;
		out.color = color;
		out.textureCoordinates = textureCoordinates;

		return out;
	}
};

class RenderableObject
{

public:

	std::shared_ptr<NameIDTag> name;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	std::shared_ptr<ShaderObject> shader;

	std::map<std::string, Texture> textures;

	Transform transform = TRANSFORM_DEFAULT;

	std::map<std::string, unsigned int> buffers =
	{
		{"VAO", 0},
		{"VBO", 0},
		{"EBO", 0}
	};

	void GenerateSquare()
	{
		std::vector<Vertex> vertices;

		vertices.push_back(Vertex::Register({ -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f }));
		vertices.push_back(Vertex::Register({ 0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f }));
		vertices.push_back(Vertex::Register({ 0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f }));
		vertices.push_back(Vertex::Register({ -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f }));

		vertices.push_back(Vertex::Register({ -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f }));
		vertices.push_back(Vertex::Register({ 0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f }));
		vertices.push_back(Vertex::Register({ 0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f }));
		vertices.push_back(Vertex::Register({ -0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f }));

		vertices.push_back(Vertex::Register({ 0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f }));
		vertices.push_back(Vertex::Register({ 0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f }));
		vertices.push_back(Vertex::Register({ 0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f }));
		vertices.push_back(Vertex::Register({ 0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f }));

		vertices.push_back(Vertex::Register({ -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f }));
		vertices.push_back(Vertex::Register({ -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f }));
		vertices.push_back(Vertex::Register({ -0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f }));
		vertices.push_back(Vertex::Register({ -0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f }));

		vertices.push_back(Vertex::Register({ -0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f }));
		vertices.push_back(Vertex::Register({ 0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f }));
		vertices.push_back(Vertex::Register({ 0.5f, 0.5f,  0.5f }, { 1.0f, 0.0f }));
		vertices.push_back(Vertex::Register({ -0.5f, 0.5f,  0.5f }, { 0.0f, 0.0f }));

		vertices.push_back(Vertex::Register({ -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f }));
		vertices.push_back(Vertex::Register({ 0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f }));
		vertices.push_back(Vertex::Register({ 0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f }));
		vertices.push_back(Vertex::Register({ -0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f }));

		std::vector<unsigned int> indices =
		{
			16, 18, 17,
			18, 16, 19,

			20, 21, 22,
			22, 23, 20,

			0, 1, 2,
			2, 3, 0,

			4, 6, 5,
			6, 4, 7,

			8, 9, 10,
			10, 11, 8,

			12, 14, 13,
			14, 12, 15
		};

		RegisterTexture("block");
		RegisterData(vertices, indices);
		Generate();
	}

	void RegisterTexture(const std::string& texture, const TextureProperties& properties = DEFAULT_TEXTURE_PROPERTIES)
	{
		textures.insert({texture, TextureManager::GetTexture(texture)});
	}

	void RegisterData(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
	{
		this->vertices = vertices;
		this->indices = indices;
	}

	void Generate()
	{
		shader->Generate();

		glGenVertexArrays(1, &buffers["VAO"]);
		glGenBuffers(1, &buffers["VBO"]);
		glGenBuffers(1, &buffers["EBO"]);

		glBindVertexArray(buffers["VAO"]);

		glBindBuffer(GL_ARRAY_BUFFER, buffers["VBO"]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers["EBO"]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoordinates));
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);

		for (auto& [key, value] : textures)
			value.Generate();

		shader->Use();
		shader->SetUniform("diffuse", 0);

		int error = glGetError();
		if (error != GL_NO_ERROR)
			Logger_ThrowError(std::to_string(error), std::format("OpenGL error: {}", error), false);
	}

	void ReGenerate()
	{
		glGenVertexArrays(1, &buffers["VAO"]);
		glGenBuffers(1, &buffers["VBO"]);
		glGenBuffers(1, &buffers["EBO"]);

		glBindVertexArray(buffers["VAO"]);

		glBindBuffer(GL_ARRAY_BUFFER, buffers["VBO"]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers["EBO"]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoordinates));
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);

		int error = glGetError();
		if (error != GL_NO_ERROR)
			Logger_ThrowError(std::to_string(error), std::format("OpenGL error: {}", error), false);
	}

	void CleanUp()
	{
		MainThreadExecutor::QueueTask([this]()
		{
			glDeleteVertexArrays(1, &buffers["VAO"]);
			glDeleteBuffers(1, &buffers["VBO"]);
			glDeleteBuffers(1, &buffers["EBO"]);
			
			shader->CleanUp();
		});
		
		vertices.clear();
		indices.clear();
		buffers.clear();
	}

	static std::shared_ptr<RenderableObject> Register(std::shared_ptr<NameIDTag> name, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::string& shader)
	{
		std::shared_ptr<RenderableObject> out(new RenderableObject());

		out->name = name;
		out->vertices = vertices;
		out->indices = indices;
		out->shader = ShaderManager::GetShader(shader);

		return out;
	}
};

#endif // !RENDERABLE_OBJECT_HPP