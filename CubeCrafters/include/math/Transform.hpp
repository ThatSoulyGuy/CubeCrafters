#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <glm/glm.hpp>

#define TRANSFORM_DEFAULT Transform::Register({0.0f, 0.0f, 0.0f})

struct Transform
{
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	glm::vec3 right;
	glm::vec3 up = { 0, 1, 0 };
	glm::vec3 pivot;

	void Translate(const glm::vec3& translation)
	{
		position += translation;
	}

	void Rotate(const glm::vec3& rotation)
	{
		this->rotation += rotation;
	}

	static Transform Register(const glm::vec3& position, const glm::vec3& rotation = { 0.0f, 0.0f, 0.0f }, const glm::vec3& scale = { 1.0f, 1.0f, 1.0f }, const glm::vec3& pivot = {-1.0f, -1.0f, -1.0f})
	{
		Transform out = {};

		out.position = position;
		out.rotation = rotation;
		out.scale = scale;

		if (pivot == glm::vec3{ -1.0f, -1.0f, -1.0f })
			out.pivot = position;
		else
			out.pivot = pivot;

		return out;
	}
};

#endif // !TRANSFORM_HPP