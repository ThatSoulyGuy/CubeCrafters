#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <memory>
#include "core/Input.hpp"	
#include "math/Camera.hpp"
#include "math/Raycast.hpp"

class Player
{

public:
	
	Camera camera = {};
	Transform transform = TRANSFORM_DEFAULT;
	AABB boundingBox;

	float mouseSensitivity = 0.08f;
	float moveSpeed = 0.1f;

	void Initialize(const glm::vec3& position)
	{
		Input::SetCursorMode(false);
		camera.Initialize(position);
		transform.position = position;

		wireframeBox = RenderableObject::Register(NameIDTag::Register(wireframeBox.get()), {}, {}, "wireframe");
		wireframeBox->wireframe = true;
		wireframeBox->active = false;
		wireframeBox->Generate();

		boundingBox = AABB::Register({ 0.4f, 1.89f, 0.4f });
	}

	void Update()
	{
		camera.Update(transform.position, transform.rotation, transform.right);

		UpdateControls();
		UpdateMouseLook();
		UpdateMovement();

		boundingBox.Update(transform.position);
	}

private:

	glm::vec2 oldMouse, newMouse;
	std::shared_ptr<RenderableObject> wireframeBox;

	void UpdateControls()
	{
		glm::ivec3 wireframeBlockPosition = Raycast::Shoot(camera.transform.position, camera.transform.rotation, 5.0f);

		if (wireframeBlockPosition != glm::ivec3{ -1, -1, -1 })
		{
			wireframeBox->active = true;
			wireframeBox->transform.position = { (float)wireframeBlockPosition.x + 0.5f, (float)wireframeBlockPosition.y + 0.5f, (float)wireframeBlockPosition.z + 0.5f };
		}
		else
			wireframeBox->active = false;

		Renderer::RegisterObject(wireframeBox);

		if (Input::GetMouseButton(0, GLFW_PRESS))
		{
			glm::ivec3 blockPosition = Raycast::Shoot(camera.transform.position, camera.transform.rotation, 5.0f);

			if (blockPosition != glm::ivec3{ -1, -1, -1 })
				World::SetBlock(blockPosition, BlockType::BLOCK_AIR);
		}

		if (Input::GetMouseButton(1, GLFW_PRESS))
		{
			glm::ivec3 blockPosition = Raycast::Shoot(camera.transform.position, camera.transform.rotation, 5.0f);

			if (blockPosition != glm::ivec3{ -1, -1, -1 })
			{
				BlockFace hitFace = Raycast::GetHitFace(blockPosition, camera.transform.position, camera.transform.rotation);

				glm::ivec3 newBlockPosition = blockPosition;

				switch (hitFace)
				{
					case BlockFace::TOP:    newBlockPosition.y += 1; break;
					case BlockFace::BOTTOM: newBlockPosition.y -= 1; break;
					case BlockFace::NORTH:  newBlockPosition.z -= 1; break;
					case BlockFace::SOUTH:  newBlockPosition.z += 1; break;
					case BlockFace::EAST:   newBlockPosition.x += 1; break;
					case BlockFace::WEST:   newBlockPosition.x -= 1; break;
				}

				World::SetBlock(newBlockPosition, BlockType::BLOCK_GRASS);
			}
		}
	}

	void UpdateMouseLook()
	{
		newMouse.x = Input::mousePosition.x;
		newMouse.y = Input::mousePosition.y;

		float dx = (newMouse.x - oldMouse.x);
		float dy = (newMouse.y - oldMouse.y);

		camera.yaw += dx * mouseSensitivity;
		camera.pitch -= dy * mouseSensitivity;

		if (camera.pitch > 90)
			camera.pitch = 89.99f;

		if (camera.pitch < -90)
			camera.pitch = -89.99f;

		oldMouse.x = newMouse.x;
		oldMouse.y = newMouse.y;
	}

	void UpdateMovement()
	{
		glm::vec3 proposedPosition = transform.position;

		if (Input::GetKey(GLFW_KEY_W, GLFW_PRESS))
			proposedPosition += moveSpeed * transform.rotation;

		if (Input::GetKey(GLFW_KEY_S, GLFW_PRESS))
			proposedPosition -= moveSpeed * transform.rotation;

		if (Input::GetKey(GLFW_KEY_A, GLFW_PRESS))
			proposedPosition += moveSpeed * transform.right;

		if (Input::GetKey(GLFW_KEY_D, GLFW_PRESS))
			proposedPosition -= moveSpeed * transform.right;

		glm::ivec3 currentChunk = World::WorldToChunkCoordinates(transform.position);
		auto chunk = World::GetChunk(currentChunk);

		AABB proposedAABB = boundingBox;
		proposedAABB.Update(proposedPosition);

		std::vector<AABB> nearbyAABBs;
		glm::ivec3 playerBlockPosition = Chunk::WorldToBlockCoordinates(transform.position);

		if (chunk != nullptr)
		{
			for (auto& blockAABB : chunk->blockAABBs)
			{
				if (IsWithinRadius(playerBlockPosition, blockAABB.first, 2))
					nearbyAABBs.push_back(blockAABB.second);
			}
		}
		
		for (auto& block : nearbyAABBs)
		{
			if (boundingBox.IsColliding(block))
			{
				glm::vec3 overlap = CalculateCollisionOverlap(proposedAABB, block);
				proposedPosition -= overlap;
				proposedPosition = transform.position;
				break;
			}
	}

		transform.position = proposedPosition;
	}

	glm::vec3 CalculateCollisionOverlap(const AABB& a, const AABB& b)
	{
		glm::vec3 overlap;

		float overlapX = std::min(a.max.x, b.max.x) - std::max(a.min.x, b.min.x);
		float overlapY = std::min(a.max.y, b.max.y) - std::max(a.min.y, b.min.y);
		float overlapZ = std::min(a.max.z, b.max.z) - std::max(a.min.z, b.min.z);

		if (overlapX < overlapY && overlapX < overlapZ) 
			overlap.x = overlapX;
		else if (overlapY < overlapZ) 
			overlap.y = overlapY;
		else 
			overlap.z = overlapZ;

		return overlap;
	}

	static bool IsWithinRadius(const glm::ivec3& center, const glm::ivec3& point, int radius)
	{
		int dx = center.x - point.x;
		int dy = center.y - point.y;
		int dz = center.z - point.z;
		return (dx * dx + dy * dy + dz * dz) <= radius * radius;
	}
};

#endif // !PLAYER_HPP