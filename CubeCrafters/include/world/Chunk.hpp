#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <mutex>
#include <vector>
#include <array>
#include <memory>
#include "math/TransformI.hpp"
#include "render/Renderer.hpp"
#include "world/BlockManager.hpp"

#define CHUNK_SIZE 16

class Chunk
{

public:
	
	void Initialize(const glm::ivec3& position, bool generateNothing = false)
	{
		transform.position = position;

		for (int x = 0; x < CHUNK_SIZE; ++x)
		{
			for (int y = 0; y < CHUNK_SIZE; ++y)
			{
				for (int z = 0; z < CHUNK_SIZE; ++z)
				{
					if (generateNothing)
						blocks[x][y][z] = (int)BlockType::BLOCK_AIR;
                    else
                    {
                        if (y < 15)
                            blocks[x][y][z] = (int)BlockType::BLOCK_DIRT;

                        if (y < 12)
                            blocks[x][y][z] = (int)BlockType::BLOCK_STONE;

                        blocks[x][15][z] = (int)BlockType::BLOCK_GRASS;
                    }
				}
			}
		}
		
		mesh = RenderableObject::Register(NameIDTag::Register(std::format("Chunk_{}_{}_{}", position.x, position.y, position.z), this), {}, {}, "default");
		mesh->transform = transform.ToTransform();

        MainThreadExecutor::QueueTask([this]()
        {
            mesh->RegisterTexture("atlas");
        });

		Rebuild();
	}

	void Rebuild()
	{
        std::lock_guard<std::mutex> guard(chunkMutex);

		vertices.clear();
		indices.clear();
		indicesIndex = 0;

		for (int x = 0; x < CHUNK_SIZE; ++x)
		{
			for (int y = 0; y < CHUNK_SIZE; ++y)
			{
				for (int z = 0; z < CHUNK_SIZE; ++z)
				{
                    if (blocks[x][y][z] == (int)BlockType::BLOCK_AIR)
                        continue;

                    std::array<glm::vec2, 6> textureCoordinates = BlockManager::GetBlockTexture((BlockType)blocks[x][y][z]);

                    if (ShouldRenderFace(x, y, z, "top"))
                        GenerateTopFace({x, y, z}, BlockManager::GetTextureCoordinates(textureCoordinates[0]));

                    if (ShouldRenderFace(x, y, z, "bottom"))
                        GenerateBottomFace({x, y, z}, BlockManager::GetTextureCoordinates(textureCoordinates[1]));

                    if (ShouldRenderFace(x, y, z, "front"))
                        GenerateFrontFace({x, y, z}, BlockManager::GetTextureCoordinates(textureCoordinates[2]));

                    if (ShouldRenderFace(x, y, z, "back"))
                        GenerateBackFace({x, y, z}, BlockManager::GetTextureCoordinates(textureCoordinates[3]));

                    if (ShouldRenderFace(x, y, z, "right"))
                        GenerateRightFace({x, y, z}, BlockManager::GetTextureCoordinates(textureCoordinates[4]));

                    if (ShouldRenderFace(x, y, z, "left"))
                        GenerateLeftFace({x, y, z}, BlockManager::GetTextureCoordinates(textureCoordinates[5]));
				}
			}
		}

        MainThreadExecutor::QueueTask([this]()
        {

            mesh->RegisterData(vertices, indices);

            if (firstRebuild)
            {

                mesh->Generate();
                firstRebuild = false;
            }
            else
                mesh->ReGenerate();

            Renderer::RegisterObject(mesh);
        });
	}

    void SetBlock(const glm::ivec3& position, BlockType type)
    {
        if (position.x < 0 || position.x >= CHUNK_SIZE)
            return;

        if (position.y < 0 || position.y >= CHUNK_SIZE)
            return;

        if (position.z < 0 || position.z >= CHUNK_SIZE)
            return;

        blocks[position.x][position.y][position.z] = (int)type;

        Rebuild();
    }

    glm::ivec3 WorldToBlockCoordinates(const glm::vec3& worldPosition)
    {
        int blockX = (int)floor(worldPosition.x) % CHUNK_SIZE;
        int blockY = (int)floor(worldPosition.y) % CHUNK_SIZE;
        int blockZ = (int)floor(worldPosition.z) % CHUNK_SIZE;

        blockX = (blockX < 0) ? CHUNK_SIZE + blockX : blockX;
        blockY = (blockY < 0) ? CHUNK_SIZE + blockY : blockY;
        blockZ = (blockZ < 0) ? CHUNK_SIZE + blockZ : blockZ;

        return { blockX, blockY, blockZ };
    }

    glm::ivec3 WorldToBlockCoordinates(const glm::ivec3& worldPosition)
    {
        int blockX = (int)floor(worldPosition.x) % CHUNK_SIZE;
        int blockY = (int)floor(worldPosition.y) % CHUNK_SIZE;
        int blockZ = (int)floor(worldPosition.z) % CHUNK_SIZE;

        blockX = (blockX < 0) ? CHUNK_SIZE + blockX : blockX;
        blockY = (blockY < 0) ? CHUNK_SIZE + blockY : blockY;
        blockZ = (blockZ < 0) ? CHUNK_SIZE + blockZ : blockZ;

        return { blockX, blockY, blockZ };
    }

    void CleanUp() const
    {
        Renderer::RemoveObject(mesh->name);
    }

	TransformI transform;
	std::shared_ptr<RenderableObject> mesh;

private:

    mutable std::mutex chunkMutex;
	std::vector<Vertex> vertices = {};
	std::vector<unsigned int> indices = {};

	int indicesIndex = 0;
	bool firstRebuild = true;

	unsigned int blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];

    bool ShouldRenderFace(int x, int y, int z, const std::string& face)
    {
        if (face == "top")
            return y == CHUNK_SIZE - 1 || blocks[x][y + 1][z] == (int)BlockType::BLOCK_AIR;

        if (face == "bottom")
            return y == 0 || blocks[x][y - 1][z] == (int)BlockType::BLOCK_AIR;

        if (face == "front")
            return z == CHUNK_SIZE - 1 || blocks[x][y][z + 1] == (int)BlockType::BLOCK_AIR;

        if (face == "back")
            return z == 0 || blocks[x][y][z - 1] == (int)BlockType::BLOCK_AIR;

        if (face == "right")
            return x == CHUNK_SIZE - 1 || blocks[x + 1][y][z] == (int)BlockType::BLOCK_AIR;

        if (face == "left")
            return x == 0 || blocks[x - 1][y][z] == (int)BlockType::BLOCK_AIR;
       
        return false;
    }

    void GenerateTopFace(const glm::vec3& position, const std::array<glm::vec2, 4>& uvs)
    {
        vertices.push_back(Vertex::Register({0.0f + position.x, 1.0f + position.y, 1.0f + position.z}, uvs[0]));
        vertices.push_back(Vertex::Register({0.0f + position.x, 1.0f + position.y, 0.0f + position.z}, uvs[1]));
        vertices.push_back(Vertex::Register({1.0f + position.x, 1.0f + position.y, 0.0f + position.z}, uvs[2]));
        vertices.push_back(Vertex::Register({1.0f + position.x, 1.0f + position.y, 1.0f + position.z}, uvs[3]));

        indices.push_back(indicesIndex);
        indices.push_back(indicesIndex + 2);
        indices.push_back(indicesIndex + 1);

        indices.push_back(indicesIndex);
        indices.push_back(indicesIndex + 3);
        indices.push_back(indicesIndex + 2);

        indicesIndex += 4;
    }

    void GenerateBottomFace(const glm::vec3& position, const std::array<glm::vec2, 4>& uvs)
    {
        vertices.push_back(Vertex::Register({0.0f + position.x, 0.0f + position.y, 0.0f + position.z}, uvs[0]));
        vertices.push_back(Vertex::Register({1.0f + position.x, 0.0f + position.y, 0.0f + position.z}, uvs[1]));
        vertices.push_back(Vertex::Register({1.0f + position.x, 0.0f + position.y, 1.0f + position.z}, uvs[2]));
        vertices.push_back(Vertex::Register({0.0f + position.x, 0.0f + position.y, 1.0f + position.z}, uvs[3]));

        indices.push_back(indicesIndex);
        indices.push_back(indicesIndex + 1);
        indices.push_back(indicesIndex + 2);

        indices.push_back(indicesIndex);
        indices.push_back(indicesIndex + 2);
        indices.push_back(indicesIndex + 3);

        indicesIndex += 4;
    }

    void GenerateFrontFace(const glm::vec3& position, const std::array<glm::vec2, 4>& uvs)
    {
        vertices.push_back(Vertex::Register({0.0f + position.x, 0.0f + position.y, 1.0f + position.z}, uvs[0]));
        vertices.push_back(Vertex::Register({1.0f + position.x, 0.0f + position.y, 1.0f + position.z}, uvs[1]));
        vertices.push_back(Vertex::Register({1.0f + position.x, 1.0f + position.y, 1.0f + position.z}, uvs[2]));
        vertices.push_back(Vertex::Register({0.0f + position.x, 1.0f + position.y, 1.0f + position.z}, uvs[3]));

        indices.push_back(indicesIndex);
        indices.push_back(indicesIndex + 1);
        indices.push_back(indicesIndex + 2);

        indices.push_back(indicesIndex);
        indices.push_back(indicesIndex + 2);
        indices.push_back(indicesIndex + 3);

        indicesIndex += 4;
    }

    void GenerateBackFace(const glm::vec3& position, const std::array<glm::vec2, 4>& uvs)
    {
        vertices.push_back(Vertex::Register({1.0f + position.x, 0.0f + position.y, 0.0f + position.z}, uvs[0]));
        vertices.push_back(Vertex::Register({0.0f + position.x, 0.0f + position.y, 0.0f + position.z}, uvs[1]));
        vertices.push_back(Vertex::Register({0.0f + position.x, 1.0f + position.y, 0.0f + position.z}, uvs[2]));
        vertices.push_back(Vertex::Register({1.0f + position.x, 1.0f + position.y, 0.0f + position.z}, uvs[3]));

        indices.push_back(indicesIndex);
        indices.push_back(indicesIndex + 1);
        indices.push_back(indicesIndex + 2);

        indices.push_back(indicesIndex);
        indices.push_back(indicesIndex + 2);
        indices.push_back(indicesIndex + 3);

        indicesIndex += 4;
    }

    void GenerateRightFace(const glm::vec3& position, const std::array<glm::vec2, 4>& uvs)
    {
        vertices.push_back(Vertex::Register({1.0f + position.x, 0.0f + position.y, 1.0f + position.z}, uvs[0]));
        vertices.push_back(Vertex::Register({1.0f + position.x, 0.0f + position.y, 0.0f + position.z}, uvs[1]));
        vertices.push_back(Vertex::Register({1.0f + position.x, 1.0f + position.y, 0.0f + position.z}, uvs[2]));
        vertices.push_back(Vertex::Register({1.0f + position.x, 1.0f + position.y, 1.0f + position.z}, uvs[3]));

        indices.push_back(indicesIndex);
        indices.push_back(indicesIndex + 1);
        indices.push_back(indicesIndex + 2);

        indices.push_back(indicesIndex);
        indices.push_back(indicesIndex + 2);
        indices.push_back(indicesIndex + 3);

        indicesIndex += 4;
    }

    void GenerateLeftFace(const glm::vec3& position, const std::array<glm::vec2, 4>& uvs)
    {
        vertices.push_back(Vertex::Register({0.0f + position.x, 0.0f + position.y, 0.0f + position.z}, uvs[0]));
        vertices.push_back(Vertex::Register({0.0f + position.x, 0.0f + position.y, 1.0f + position.z}, uvs[1]));
        vertices.push_back(Vertex::Register({0.0f + position.x, 1.0f + position.y, 1.0f + position.z}, uvs[2]));
        vertices.push_back(Vertex::Register({0.0f + position.x, 1.0f + position.y, 0.0f + position.z}, uvs[3]));

        indices.push_back(indicesIndex);
        indices.push_back(indicesIndex + 1);
        indices.push_back(indicesIndex + 2);

        indices.push_back(indicesIndex);
        indices.push_back(indicesIndex + 2);
        indices.push_back(indicesIndex + 3);

        indicesIndex += 4;
    }
};

#endif // !CHUNK_HPP