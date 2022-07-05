#include "texture.hpp"

#include "stb_image.h"

#include <iostream>
#include <filesystem>

Texture::Texture()
    : m_texture_id(0)
    , m_width(0)
    , m_height(0)
    , m_bit_depth(0)
    , m_file_location("")
{}

Texture::Texture(std::string fileLocation)
    : m_texture_id(0)
    , m_width(0)
    , m_height(0)
    , m_bit_depth(0)
    , m_file_location(fileLocation)
{}

void
Texture::loadTexture()
{
    // Load image
    unsigned char* textureData = stbi_load(m_file_location.c_str(),
                                           &m_width,
                                           &m_height,
                                           &m_bit_depth,
                                           0);
    if (textureData == nullptr) {
        std::cout << "Failed to find: " << m_file_location << std::endl;

        return;
    }

    // Bind texture
    glGenTextures(1, &m_texture_id);
    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    // How to handle texture when sampling across the border
    // GL_TEXTURE_WRAP_S -> x axis
    // GL_TEXTURE_WRAP_T -> y axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Texture filtering can be set for magnifying and minifying operations
    // (when scaling up or downwards)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Pass to gpu
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 m_width,
                 m_height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 textureData);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // Free image
    stbi_image_free(textureData);
}

void
Texture::useTexture()
{
    // Texture Unit fo sampler2D in shader
    glActiveTexture(GL_TEXTURE0);
    // Bind texture to texture Unit
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
}

void
Texture::clearTexture()
{
    // Delete texture from memory
    glDeleteTextures(1, &m_texture_id);

    m_texture_id = 0;
    m_width = 0;
    m_height = 0;
    m_bit_depth = 0;
    m_file_location = "";
}