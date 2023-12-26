#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "math/Camera.hpp"
#include "render/RenderableObject.hpp"

#define str std::to_string

namespace Renderer
{
	std::map<std::shared_ptr<NameIDTag>, std::shared_ptr<RenderableObject>> registeredObjects;
	glm::mat4 model;
	glm::vec3 eulerAngles;
	glm::mat4 rotationMatrix;
	bool drawLines;
	std::mutex mutex;

	void RegisterObject(std::shared_ptr<RenderableObject> object)
	{
		std::lock_guard<std::mutex> lock{ mutex };

		if (registeredObjects.contains(object->name))
		{
			registeredObjects[object->name] = object;
			return;
		}

		registeredObjects.insert({ object->name, object });
	}

	std::shared_ptr<RenderableObject> GetRenderableObject(std::shared_ptr<NameIDTag> name)
	{
		std::lock_guard<std::mutex> lock{ mutex };

		return registeredObjects[name];
	}

	void RemoveObject(std::shared_ptr<NameIDTag> name)
	{
		std::lock_guard<std::mutex> lock{ mutex };

		if (registeredObjects.contains(name))
		{
			registeredObjects[name]->CleanUp();

			MainThreadExecutor::QueueTask([name]() 
			{
				registeredObjects.erase(name);
			});
		}
		else
			Logger_ThrowError("nullptr", "No element named '" + name->name + "' was found in registeredObjects.", false);
	}

	void RenderObjects(const Camera& camera)
	{
		std::lock_guard<std::mutex> lock{ mutex };

		for (auto& [key, object] : registeredObjects)
		{
			model = glm::mat4(1.0f);

			int count = 0;

			for (auto& [key, texture] : object->textures)
			{
				glActiveTexture(GL_TEXTURE0 + count);
				glBindTexture(GL_TEXTURE_2D, texture.textureID);

				int error = glGetError();
				if (error != GL_NO_ERROR)
					Logger_ThrowError(std::to_string(error), std::format("OpenGL error: {}", error), false);

				++count;
			}

			eulerAngles = object->transform.rotation;
			rotationMatrix = glm::eulerAngleYXZ(eulerAngles.x, eulerAngles.y, eulerAngles.z);

			model = model * rotationMatrix;

			model = glm::translate(model, object->transform.position);

			object->shader->Use();
			object->shader->SetUniform("model", model);
			object->shader->SetUniform("view", camera.view);
			object->shader->SetUniform("projection", camera.projection);
			
			int error = glGetError();
			if (error != GL_NO_ERROR)
				Logger_ThrowError(std::to_string(error), std::format("OpenGL error: {}", error), false);

			glBindVertexArray(object->buffers["VAO"]);

			error = glGetError();
			if (error != GL_NO_ERROR)
				Logger_ThrowError(std::to_string(error), std::format("OpenGL error: {}", error), false);

			if (drawLines)
				glDrawElements(GL_LINES, object->indices.size(), GL_UNSIGNED_INT, 0);
			else
				glDrawElements(GL_TRIANGLES, object->indices.size(), GL_UNSIGNED_INT, 0);

			error = glGetError();
			if (error != GL_NO_ERROR)
				Logger_ThrowError(std::to_string(error), std::format("OpenGL error: {}", error), false);

			model = glm::mat4(1.0f);
		}
	}

	void CleanUp()
	{
		std::lock_guard<std::mutex> lock{ mutex };

		for (auto& [key, object] : registeredObjects)
			object->CleanUp();

		registeredObjects.clear();
	}
}

#endif //!RENDERER_HPP