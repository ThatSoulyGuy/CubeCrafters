#ifndef BLOCK_MANAGER_HPP
#define BLOCK_MANAGER_HPP

#include <array>
#include <glm/glm.hpp>

#define AtlasSize 256

enum class BlockType
{
    BLOCK_AIR = 0,
    BLOCK_TEST = 1,
    BLOCK_GRASS = 2,
    BLOCK_DIRT = 3,
    BLOCK_STONE = 4
};

namespace BlockManager
{
    const int TilePixelSize = 16;
    const float PADDING_RATIO = 1.0f / (float)AtlasSize;

    std::array<glm::vec2, 4> GetTextureCoordinates(const glm::ivec2& position) 
    {
        float perTextureSize = (float)TilePixelSize / AtlasSize;

        float u0 = position.x * perTextureSize + PADDING_RATIO * perTextureSize;
        float v0 = position.y * perTextureSize + PADDING_RATIO * perTextureSize;
        float u1 = (position.x + 1) * perTextureSize - PADDING_RATIO * perTextureSize;
        float v1 = (position.y + 1) * perTextureSize - PADDING_RATIO * perTextureSize;

        return
        {
            glm::vec2(u0, v0),
            glm::vec2(u1, v0),
            glm::vec2(u1, v1),
            glm::vec2(u0, v1)
        };
    }

    std::array<glm::vec2, 4> GetTextureCoordinates(const glm::ivec2& position, float rotation) 
    {
        float perTextureSize = (float)TilePixelSize / AtlasSize;

        float u0 = position.x * perTextureSize + PADDING_RATIO * perTextureSize;
        float v0 = position.y * perTextureSize + PADDING_RATIO * perTextureSize;
        float u1 = (position.x + 1) * perTextureSize - PADDING_RATIO * perTextureSize;
        float v1 = (position.y + 1) * perTextureSize - PADDING_RATIO * perTextureSize;

        float centerX = (u0 + u1) / 2;
        float centerY = (v0 + v1) / 2;

        std::array<glm::vec2, 4> coordinates =
        {
            glm::vec2(u0, v0),
            glm::vec2(u1, v0),
            glm::vec2(u1, v1),
            glm::vec2(u0, v1)
        };

        float rad = (float)glm::radians(rotation);

        for (int i = 0; i < coordinates.size(); i++) 
        {
            float translatedX = coordinates[i].x - centerX;
            float translatedY = coordinates[i].y - centerY;

            float rotatedX = translatedX * (float)cos(rad) - translatedY * (float)sin(rad);
            float rotatedY = translatedX * (float)sin(rad) + translatedY * (float)cos(rad);

            coordinates[i].x = rotatedX + centerX;
            coordinates[i].y = rotatedY + centerY;
        }

        return coordinates;
    }

    std::array<glm::vec2, 6> GetBlockTexture(BlockType type)
    {
        std::array<glm::vec2, 6> coordinates = {};

        switch (type)
        {
        case BlockType::BLOCK_AIR:
        {
            coordinates[0] = glm::ivec2(9, 2);
            coordinates[1] = glm::ivec2(9, 2);
            coordinates[2] = glm::ivec2(9, 2);
            coordinates[3] = glm::ivec2(9, 2);
            coordinates[4] = glm::ivec2(9, 2);
            coordinates[5] = glm::ivec2(9, 2);
            break;
        }

        case BlockType::BLOCK_TEST:
        {
            coordinates[0] = glm::ivec2(9, 2);
            coordinates[1] = glm::ivec2(9, 2);
            coordinates[2] = glm::ivec2(9, 2);
            coordinates[3] = glm::ivec2(9, 2);
            coordinates[4] = glm::ivec2(9, 2);
            coordinates[5] = glm::ivec2(9, 2);
            break;
        }

        case BlockType::BLOCK_GRASS:
        {
            coordinates[0] = glm::ivec2(0, 15);
            coordinates[1] = glm::ivec2(2, 15);
            coordinates[2] = glm::ivec2(3, 15);
            coordinates[3] = glm::ivec2(3, 15);
            coordinates[4] = glm::ivec2(3, 15);
            coordinates[5] = glm::ivec2(3, 15);
            break;
        }

        case BlockType::BLOCK_DIRT:
        {
            coordinates[0] = glm::ivec2(2, 15);
            coordinates[1] = glm::ivec2(2, 15);
            coordinates[2] = glm::ivec2(2, 15);
            coordinates[3] = glm::ivec2(2, 15);
            coordinates[4] = glm::ivec2(2, 15);
            coordinates[5] = glm::ivec2(2, 15);
            break;
        }

        case BlockType::BLOCK_STONE:
        {
            coordinates[0] = glm::ivec2(1, 15);
            coordinates[1] = glm::ivec2(1, 15);
            coordinates[2] = glm::ivec2(1, 15);
            coordinates[3] = glm::ivec2(1, 15);
            coordinates[4] = glm::ivec2(1, 15);
            coordinates[5] = glm::ivec2(1, 15);
            break;
        }
        }

        return coordinates;
    }
}

#endif // !BLOCK_MANAGER_HPP