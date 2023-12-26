#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "core/Input.hpp"	
#include "math/Camera.hpp"
#include "math/Raycast.hpp"

class Player
{

public:
	
	Camera camera = {};
	Transform transform = TRANSFORM_DEFAULT;

	float mouseSensitivity = 0.08f;
	float moveSpeed = 0.1f;

	void Initialize(const glm::vec3& position)
	{
		Input::SetCursorMode(false);
		camera.Initialize(position);
		transform.position = position;
	}

	void Update()
	{
		camera.Update(transform.position, transform.rotation, transform.right);

		UpdateControls();
		UpdateMouseLook();
		UpdateMovement();
	}

private:

	glm::vec2 oldMouse, newMouse;

	void UpdateControls()
	{
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
		if (Input::GetKey(GLFW_KEY_W, GLFW_PRESS))
			transform.position += moveSpeed * transform.rotation;

		if (Input::GetKey(GLFW_KEY_S, GLFW_PRESS))
			transform.position -= moveSpeed * transform.rotation;

		if (Input::GetKey(GLFW_KEY_A, GLFW_PRESS))
			transform.position += moveSpeed * transform.right;

		if (Input::GetKey(GLFW_KEY_D, GLFW_PRESS))
			transform.position -= moveSpeed * transform.right;
	}
};

#endif // !PLAYER_HPP