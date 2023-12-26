#ifndef THREADED_CHUNK_MAP_HPP
#define THREADED_CHUNK_MAP_HPP

#include <mutex>
#include <unordered_map>
#include <memory>
#include <glm/glm.hpp>

class Chunk;

namespace std
{
    template<>
    struct hash<glm::ivec3>
    {
        size_t operator()(const glm::ivec3& vec) const
        {
            return std::hash<int>()(vec.x) ^ (std::hash<int>()(vec.y) << 1) ^ (std::hash<int>()(vec.z) << 2);
        }
    };
}


class ThreadedChunkMap 
{

public:

    void AddChunk(const glm::ivec3& key, std::shared_ptr<Chunk> chunk) 
    {
        std::lock_guard<std::mutex> guard(mapMutex);
        loadedChunks[key] = chunk;
    }

    bool Contains(const glm::ivec3& key) 
    {
        std::lock_guard<std::mutex> guard(mapMutex);
        return loadedChunks.find(key) != loadedChunks.end();
    }

    std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>> Copy() 
    {
        std::lock_guard<std::mutex> guard(mapMutex);
        return loadedChunks;
    }

    template<typename Func>
    void ForEach(Func f) 
    {
        std::lock_guard<std::mutex> guard(mapMutex);

        for (auto& pair : loadedChunks) 
            f(pair);
    }

    auto begin() 
    {
        return Copy().begin();
    }

    auto end() 
    {
        return Copy().end();
    }

    void RemoveChunk(const glm::ivec3& key) 
    {
        std::lock_guard<std::mutex> guard(mapMutex);

        MainThreadExecutor::QueueTask([this, key]()
        {
            loadedChunks.erase(key);
        });
    }

    std::shared_ptr<Chunk> GetChunk(const glm::ivec3& key) 
    {
        std::lock_guard<std::mutex> guard(mapMutex);
        auto it = loadedChunks.find(key);

        if (it != loadedChunks.end()) 
            return it->second;

        return nullptr;
    }


private:

    std::mutex mapMutex;
    std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>> loadedChunks;
};

#endif // !THREADED_CHUNK_MAP_HPP