#ifndef RAYCAST_HPP
#define RAYCAST_HPP

#include "world/World.hpp"

namespace Raycast
{
    glm::ivec3 Shoot(glm::vec3 origin, const glm::vec3& direction, float maxDistance)
    {
        glm::vec3 ray = origin;
        glm::vec3 step = glm::normalize(direction);

        for (float t = 0; t < maxDistance; t += 1.0f)
        {
            ray += step;

            std::shared_ptr<Chunk> chunk = World::GetChunk(ray);

            if (chunk != nullptr)
            {
                glm::ivec3 blockPos = Chunk::WorldToBlockCoordinates(ray);

                if (chunk->HasBlock(blockPos))
                    return { (int)floor(ray.x), (int)floor(ray.y), (int)floor(ray.z) };
            }
        }

        return {-1, -1, -1};
    }
}

#endif // !RAYCAST_HPP