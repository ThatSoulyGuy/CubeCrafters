#ifndef RAYCAST_HPP
#define RAYCAST_HPP

#include "world/World.hpp"

enum class BlockFace
{
    TOP,
    BOTTOM,
    NORTH,
    SOUTH,
    EAST,
    WEST
};

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

    BlockFace GetHitFace(const glm::vec3& blockPosition, const glm::vec3& rayOrigin, const glm::vec3& rayDirection)
    {
        glm::vec3 tMin = (blockPosition - rayOrigin) / rayDirection;
        glm::vec3 tMax = (blockPosition + glm::vec3(1, 1, 1) - rayOrigin) / rayDirection;

        glm::vec3 t1 = glm::min(tMin, tMax);
        glm::vec3 t2 = glm::max(tMin, tMax);

        float tNear = glm::max(glm::max(t1.x, t1.y), t1.z);
        float tFar = glm::min(glm::min(t2.x, t2.y), t2.z);

        if (tNear == t1.x) return (rayDirection.x > 0) ? BlockFace::WEST : BlockFace::EAST;
        if (tNear == t1.y) return (rayDirection.y > 0) ? BlockFace::BOTTOM : BlockFace::TOP;
        if (tNear == t1.z) return (rayDirection.z > 0) ? BlockFace::NORTH : BlockFace::SOUTH;

        return BlockFace::TOP;
    }
}

#endif // !RAYCAST_HPP