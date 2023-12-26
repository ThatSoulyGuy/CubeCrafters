#ifndef TRANSFORMI_HPP
#define TRANSFORMI_HPP

#include <glm/glm.hpp>
#include "math/Transform.hpp"

#define TRANSFORMI_DEFAULT Transform::Register({0, 0, 0})

struct TransformI
{
	glm::ivec3 position = {};
	glm::ivec3 rotation = {};
	glm::ivec3 scale = {};
	glm::ivec3 right = {};
	glm::ivec3 up = { 0, 1, 0 };
	glm::ivec3 pivot = {};

	void Translate(const glm::ivec3& translation)
	{
		position += translation;
	}

	void Rotate(const glm::ivec3& rotation)
	{
		this->rotation += rotation;
	}

	Transform ToTransform()
	{
		return Transform::Register(position, rotation, scale, pivot);
	}

	static TransformI Register(const glm::ivec3& position, const glm::ivec3& rotation = { 0.0f, 0.0f, 0.0f }, const glm::ivec3& scale = { 1.0f, 1.0f, 1.0f }, const glm::ivec3& pivot = {-1.0f, -1.0f, -1.0f})
	{
		TransformI out = {};

		out.position = position;
		out.rotation = rotation;
		out.scale = scale;

		if (pivot == glm::ivec3{ -1.0f, -1.0f, -1.0f })
			out.pivot = position;
		else
			out.pivot = pivot;

		return out;
	}
};

#endif // !TRANSFORMI_HPP