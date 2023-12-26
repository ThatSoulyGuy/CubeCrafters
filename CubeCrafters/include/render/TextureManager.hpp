#ifndef TEXTURE_MANAGER_HPP
#define TEXTURE_MANAGER_HPP

#include "core/Logger.hpp"
#include "render/Texture.hpp"

namespace TextureManager
{
    std::map<std::string, Texture> registeredTextures;

    void RegisterTexture(const Texture& texture)
    {
        Logger_FunctionStart;

        Logger_WriteConsole("Registered Texture: '" + texture.name + "'", LogLevel::INFO);

        registeredTextures.insert({ texture.name, texture });

        Logger_FunctionEnd;
    }

    Texture GetTexture(const std::string& name)
    {
        if (!registeredTextures.contains(name))
            Logger_ThrowError("nullptr", std::format("Failed to load texture: '{}'", name), false);

        return registeredTextures[name];
    }
}

#endif // !TEXTURE_MANAGER_HPP