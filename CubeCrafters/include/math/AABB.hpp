#ifndef AABB_HPP
#define AABB_HPP

#include <glm/glm.hpp>

struct AABB
{
    glm::vec3 min;
    glm::vec3 max;
    glm::vec3 dimensions;
    glm::vec3 position;

    bool IsColliding(const AABB& other)
    {
        return (this->min.x <= other.max.x && this->max.x >= other.min.x) &&
            (this->min.y <= other.max.y && this->max.y >= other.min.y) &&
            (this->min.z <= other.max.z && this->max.z >= other.min.z);
    }

    void Update(const glm::vec3& position)
    {
        min =
        {
            position.x - dimensions.x / 2,
            position.y,
            position.z - dimensions.z / 2
        };

        max =
        {
            position.x + dimensions.x / 2,
            position.y + dimensions.y,
            position.z + dimensions.z / 2
        };

        this->position = position;
    }

    glm::vec3 GetHalfSize()
    {
        return { dimensions.x / 2, dimensions.y / 2, dimensions.z / 2 };
    }

    static AABB Register(const glm::vec3& dimensions)
    {
        return Register({ 0, 0, 0 }, dimensions);
    }

    static AABB Register(const glm::vec3& position, const glm::vec3& dimensions)
    {
        AABB aabb = {};

        aabb.dimensions = dimensions;
        aabb.Update(position);

        return aabb;
    }
};

#endif // !AABB_HPP