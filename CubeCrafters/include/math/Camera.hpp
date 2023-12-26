#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "core/Window.hpp"
#include "math/Transform.hpp"

class Camera
{

public:
	
    float fov;
    float nearPlane, farPlane;

    Transform transform = TRANSFORM_DEFAULT;
    float pitch = 0, yaw = 0;

    glm::mat4 projection = glm::mat4(1.0f), view = glm::mat4(1.0f);

    void Initialize(const glm::vec3& position, float fov = 45.0f, float nearPlane = 0.004f, float farPlane = 500.0f)
    {
        transform.position = position;
        this->fov = fov;
        this->nearPlane = nearPlane;
        this->farPlane = farPlane;

        projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(fov), (float)Window::size.x / (float)Window::size.y, nearPlane, farPlane);
    }

    void Update(const glm::vec3& position, glm::vec3& rotationOut, glm::vec3& rightOut)
    {
        projection = glm::perspective(glm::radians(fov), (float)Window::size.x / (float)Window::size.y, nearPlane, farPlane);

        glm::vec3 direction = glm::vec3(
            cos(glm::radians(yaw)) * (float)cos(glm::radians(pitch)),
            sin(glm::radians(pitch)),
            sin(glm::radians(yaw)) * (float)cos(glm::radians(pitch))
            );

        transform.rotation = glm::normalize(direction);

        transform.right = glm::normalize(glm::cross(transform.up, direction));

        transform.position = position;
        rotationOut = transform.rotation;
        rightOut = transform.right; 

        view = glm::mat4(1.0f);
        view = glm::lookAt(transform.position, transform.position + direction, transform.up);
    }
};

#endif // !CAMERA_HPP