#pragma once

#include <GL/glew.h>

#include <string>

class Texture
{
public:
    Texture();
    Texture(std::string fileLocation);

    ~Texture()
    {
        clearTexture();
    }

    void loadTexture();
    void useTexture();
    void clearTexture();

    int getTextureWidth()
    {
        return m_width;
    }

    int getTextureHeight()
    {
        return m_height;
    }

protected:
    GLuint m_texture_id;

    int m_width;
    int m_height;
    int m_bit_depth;

    std::string m_file_location;
};
