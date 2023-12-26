#ifndef WORLD_HPP
#define WORLD_HPP

#include <thread>
#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include "thread/ThreadedChunkMap.hpp"
#include "thread/ThreadTaskExecutor.hpp"
#include "world/Chunk.hpp"

#define VIEW_DISTANCE 1

namespace World
{
    ThreadedChunkMap loadedChunks;

    glm::vec3 playerPosition;
    ThreadTaskExecutor worldExecutor;

    glm::ivec3 WorldToChunkCoordinates(const glm::vec3& worldPosition)
    {
        return glm::ivec3(
            (int)floor(worldPosition.x / CHUNK_SIZE),
            (int)floor(worldPosition.y / CHUNK_SIZE),
            (int)floor(worldPosition.z / CHUNK_SIZE)
        );
    }

    glm::ivec3 WorldToChunkCoordinates(const glm::ivec3& worldPosition)
    {
        return glm::ivec3(
            (int)floor((double)worldPosition.x / CHUNK_SIZE),
            (int)floor((double)worldPosition.y / CHUNK_SIZE),
            (int)floor((double)worldPosition.z / CHUNK_SIZE)
        );
    }

    void Update()
    {
        glm::ivec3 playerChunkCoordinates = WorldToChunkCoordinates(playerPosition);

        for (int x = -VIEW_DISTANCE; x <= VIEW_DISTANCE; x++)
        {
            for (int z = -VIEW_DISTANCE; z <= VIEW_DISTANCE; z++)
            {
                glm::ivec3 chunkCoordinate = glm::ivec3(playerChunkCoordinates.x + x, 0, playerChunkCoordinates.z + z);

                if (!loadedChunks.Contains(chunkCoordinate))
                {
                    std::shared_ptr<Chunk> chunk(new Chunk());

                    glm::ivec3 worldPosition = { chunkCoordinate.x * CHUNK_SIZE, 0, chunkCoordinate.z * CHUNK_SIZE };

                    if (chunk != nullptr)
                    {
                        chunk->Initialize(worldPosition);

                        loadedChunks.AddChunk(chunkCoordinate, chunk);
                    }
                }
            }
        }

        std::unordered_set<glm::ivec3> chunkSet;

        loadedChunks.ForEach([&chunkSet](const auto& pair) 
        {
            //if (chunkSet.find(pair.first) != chunkSet.end()) 
                chunkSet.insert(pair.first);
        });
            
        for (const auto& chunkCoordinate : chunkSet)
        {
            if ((std::abs(chunkCoordinate.x - playerChunkCoordinates.x) > VIEW_DISTANCE) && (std::abs(chunkCoordinate.z - playerChunkCoordinates.z) > VIEW_DISTANCE))
            {
                loadedChunks.GetChunk(chunkCoordinate)->CleanUp();

                loadedChunks.RemoveChunk(chunkCoordinate);
            }
        }
    }

    void StartUpdating()
    {
        worldExecutor.AddTask([&]
        {
            try
            {
                while (true)
                {
                    Update();
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }
            catch (...)
            {
                Logger_ThrowError("...", "Thread error! Thread 'world' crashed unexpectedly!", true);
            }
        });
    }

    void StopUpdating()
    {
        worldExecutor.Terminate();
    }
}

#endif // !WORLD_HPP