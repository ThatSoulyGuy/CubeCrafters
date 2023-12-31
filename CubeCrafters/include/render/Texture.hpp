#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>
#include <vector>
#include <map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <STBI/stb_image.h>
#include "core/Logger.hpp"
#include "core/Settings.hpp"

#define DEFAULT_TEXTURE_PROPERTIES TextureProperties::Register(GL_REPEAT, GL_NEAREST, true, TextureType::DIFFUSE)

#define TEXTURE_PROPERTIES_WRAPPING(wrapping) TextureProperties::Register(wrapping, GL_NEAREST, true, TextureType::DIFFUSE)
#define TEXTURE_PROPERTIES_PRECISION(precision) TextureProperties::Register(GL_REPEAT, precision, true, TextureType::DIFFUSE)
#define TEXTURE_PROPERTIES_FLIP(flip) TextureProperties::Register(GL_REPEAT, GL_NEAREST, flip, TextureType::DIFFUSE)
#define TEXTURE_PROPERTIES_TYPE(type) TextureProperties::Register(GL_REPEAT, GL_NEAREST, true, type)

enum class TextureType
{
    DIFFUSE,
    SPECULAR,
    NORMAL
};

std::string TextureType2String(TextureType type)
{
    switch (type)
    {
    case TextureType::DIFFUSE:
        return "texture_diffuse";

    case TextureType::SPECULAR:
        return "texture_specular";

    case TextureType::NORMAL:
        return "texture_normal";

    default:
        return "";
    }
}

TextureType String2TextureType(const std::string& type)
{
    if (type == "texture_diffuse")
        return TextureType::DIFFUSE;
    else if (type == "texture_specular")
        return TextureType::SPECULAR;
    else
        return TextureType::NORMAL;
}

struct TextureProperties
{
    int wrapping, precision;
    bool flip;
    TextureType type;

    static TextureProperties Register(int wrapping, int precision, bool flip, TextureType type)
    {
        TextureProperties out = {};

        out.wrapping = wrapping;
        out.precision = precision;
        out.flip = flip;
        out.type = type;

        return out;
    }
};

struct Texture
{
    std::string name = "";
    std::string path = "";
    glm::ivec2 size = { 0.0f, 0.0f };
    unsigned int textureID = 0;
    unsigned char* data = NULL;

    virtual void Generate(const TextureProperties& properties = DEFAULT_TEXTURE_PROPERTIES)
    {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, properties.wrapping);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, properties.wrapping);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, properties.precision);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, properties.precision);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        int nrComponents;
        stbi_set_flip_vertically_on_load(properties.flip);

        data = stbi_load(path.c_str(), &size.x, &size.y, &nrComponents, 0);

        int error = glGetError();
        if (error != GL_NO_ERROR)
            Logger_ThrowError(std::to_string(error), std::format("OpenGL error: {}", error), false);

        if (data)
        {
            GLenum internalFormat = 0;
            GLenum dataFormat = 0;

            if (nrComponents == 1)
                internalFormat = dataFormat = GL_RED;
            else if (nrComponents == 3)
            {
                internalFormat = GL_SRGB;
                dataFormat = GL_RGB;
            }
            else if (nrComponents == 4)
            {
                internalFormat = GL_SRGB_ALPHA;
                dataFormat = GL_RGBA;
            }

            glTexImage2D(GL_TEXTURE_2D, 0, dataFormat, size.x, size.y, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        }
        else
            Logger_ThrowError("null", std::format("Failed to load texture from path: {}", path), false);

        stbi_image_free(data);
    }

    static Texture Register(const std::string& localPath, const std::string& name, const std::string& domain = Settings::defaultDomain)
    {
        Texture out = {};

        out.name = name;
        out.path = "assets/" + domain + "/" + localPath;

        return out;
    }

    static Texture RegisterGlobalPath(const std::string& path, const std::string& name)
    {
        Texture out = {};

        out.name = name;
        out.path = path;

        return out;
    }
};

#endif // !TEXTURE_HPP