#include "dialogtextmanager.hpp"
#include "gamewindow.hpp"

#include "glerrorcheck.hpp"

#include <gtc/matrix_transform.hpp>

DialogTextManager::DialogTextManager(ShaderProgram* shader)
    : m_shader(shader)
{
    initialize();
}

void
DialogTextManager::initialize()
{
    // In this game, several chinese characters are needed to be rendered,
    // we need to use unicode encoding (ft_encoding_unicode) to do that.
    //
    // Note that, we only need some fancy chinese characters and 26 english
    // characters, so we will only generate these characters as textures.

    // Init free type
    if (FT_Init_FreeType(&m_free_type_lib))
        throw("DialogText: initialize free type error.");

    // Load english font as free type face
    FT_Face faceEN;
    if (FT_New_Face(m_free_type_lib, Fonts::englishFontPath.c_str(), 0, &faceEN))
        throw("DialogText: failed to load font.");

    // Set size for both height and width.
    FT_Set_Pixel_Sizes(faceEN, Fonts::fontTextSize, Fonts::fontTextSize);

    // The bitmap generated from the glyph is a grayscale 8-bit image where each color is
    // represented by a single byte. For this reason we'd like to store each byte of the bitmap
    // buffer as the texture's single color value. We accomplish this by creating a texture where
    // each byte corresponds to the texture color's red component (first byte of its color vector)
    // https://www.khronos.org/opengl/wiki/Pixel_Transfer#Pixel_layout
    //
    // OpenGL requires that textures all have a 4-byte alignment.
    //
    // Disable byte-alignment restriction.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Generate 26 english letter textures first
    for (int i = 0; i < Fonts::englishLetters.size(); ++i) {
        if (FT_Load_Char(faceEN, Fonts::englishLetters[i], FT_LOAD_RENDER)) {
            throw("DialogText: dailed to load Glyph");
        }

        // Generate texture
        GLuint textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RED,
                     faceEN->glyph->bitmap.width,
                     faceEN->glyph->bitmap.rows,
                     0,
                     GL_RED,
                     GL_UNSIGNED_BYTE,
                     faceEN->glyph->bitmap.buffer);
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Now store character for later use
        m_glyph_tex_map_eng[Fonts::englishLetters[i]] = {textureId,
                                                         faceEN->glyph->bitmap.width,
                                                         faceEN->glyph->bitmap.rows,
                                                         faceEN->glyph->bitmap_left,
                                                         faceEN->glyph->bitmap_top,
                                                         faceEN->glyph->advance.x};
    }

    // Load chinese font as free type face
    FT_Face faceCH;
    if (FT_New_Face(m_free_type_lib, Fonts::chineseFontPath.c_str(), 0, &faceCH))
        throw("DialogText: failed to load font.");

    // Set size for both height and width.
    FT_Set_Pixel_Sizes(faceCH, Fonts::fontTextSize, Fonts::fontTextSize);

    // Unicode encoding for chinese characters.
    FT_Select_Charmap(faceCH, ft_encoding_unicode);

    // Generate chinese characters textures
    // Note: Chinese letters that are used are stored in wide characters wchar_t
    // Usually UTF-8 character literals (u8 prefix should be fine), but seems like
    // only wchar_t work here (L prefix).
    for (int i = 0; i < Fonts::chineseLetters.size(); ++i) {
        if (FT_Load_Glyph(faceCH,
                          FT_Get_Char_Index(faceCH, Fonts::chineseLetters[i]),
                          FT_LOAD_RENDER)) {
            throw("DialogText: dailed to load Glyph");
        }

        // Generate texture
        GLuint textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RED,
                     faceCH->glyph->bitmap.width,
                     faceCH->glyph->bitmap.rows,
                     0,
                     GL_RED,
                     GL_UNSIGNED_BYTE,
                     faceCH->glyph->bitmap.buffer);
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Now store character for later use
        m_glyph_tex_map_ch[Fonts::chineseLetters[i]] = {textureId,
                                                        faceCH->glyph->bitmap.width,
                                                        faceCH->glyph->bitmap.rows,
                                                        faceCH->glyph->bitmap_left,
                                                        faceCH->glyph->bitmap_top,
                                                        faceCH->glyph->advance.x};
    }

    // Unbind texture.
    glBindTexture(GL_TEXTURE_2D, 0);

    // Destroy FreeType once we're finished.
    FT_Done_Face(faceEN);
    FT_Done_Face(faceCH);
    FT_Done_FreeType(m_free_type_lib);

    // Restore to four.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

void
DialogTextManager::setDialogTextInitialTrans(const glm::vec2& initialTrans)
{
    m_char_trans = initialTrans;
}

void
DialogTextManager::setDialogNameShift(const glm::vec2& nameShift)
{
    m_char_name_trans = nameShift;
}

void
DialogTextManager::setDialogTextShift(const glm::vec2& textShift)
{
    m_char_text_trans = textShift;
}

std::vector<DialogChar*>
DialogTextManager::getDialogTextCharNodeListFromString(const std::string& string)
{
    // Will consider multi-line here.
    float xAdvanceTotal {0.0f};
    float yAdvanceTotal {0.0f};
    float xCharSpace {0.0f};
    float yCharOffsetFromOrigin {0.0f};

    std::vector<DialogChar*> charList;
    charList.reserve(string.size());

    for (auto charC = string.begin(); charC != string.end(); charC++) {
        auto dialogChar = new DialogChar(StringContant::dialogCharNodeName + *charC,
                                         m_shader,
                                         m_glyph_tex_map_eng[*charC],
                                         Fonts::fontTextScale);

        // See more details on README about glyph metrics on the use of y bearing.
        yCharOffsetFromOrigin = static_cast<float>(m_glyph_tex_map_eng[*charC].textureHeight
                                                   - m_glyph_tex_map_eng[*charC].bearingY);

        dialogChar->translate(
            glm::vec3(m_char_trans.x + xAdvanceTotal + m_char_text_trans.x,
                      m_char_trans.y + m_char_text_trans.y - yAdvanceTotal - yCharOffsetFromOrigin,
                      0.0f));

        // Now advance cursors for next glyph.
        if (m_glyph_tex_map_eng[*charC].textureWidth == 0) {
            // Note that advance is number of 1 / 64 pixels,
            // so we need to divide by 64 (right shift is faster here)
            // Use advance only for spaces
            xCharSpace = static_cast<float>(m_glyph_tex_map_eng[*charC].advance >> 6);
        } else {
            // Advance by actual char width
            xCharSpace = Fonts::fontTextScale
                         * (static_cast<float>(m_glyph_tex_map_eng[*charC].textureWidth)
                            / static_cast<float>(Fonts::fontTextSize));
        }

        xAdvanceTotal = xAdvanceTotal + xCharSpace;

        // Multi-line
        if (xAdvanceTotal > Fonts::fontTextLineWidth) {
            xAdvanceTotal = 0.0f;
            yAdvanceTotal = yAdvanceTotal + Fonts::fontTextLineHeight;
        }

        charList.push_back(dialogChar);
    }

    return charList;
}

std::vector<DialogChar*>
DialogTextManager::getDialogNameCharNodeListFromString(const std::wstring& string)
{
    // No need to consider next line here.
    float xAdvanceTotal {0.0f};

    std::vector<DialogChar*> charList;
    charList.reserve(string.size());

    for (auto charC = string.begin(); charC != string.end(); charC++) {
        auto dialogChar = new DialogChar(StringContant::dialogCharNodeName + std::to_string(*charC),
                                         m_shader,
                                         m_glyph_tex_map_ch[*charC],
                                         Fonts::fontNameScale);

        dialogChar->translate(glm::vec3(m_char_trans.x + xAdvanceTotal + m_char_name_trans.x,
                                        m_char_trans.y + m_char_name_trans.y,
                                        0.0f));

        xAdvanceTotal = xAdvanceTotal + Fonts::fontNameScale + Fonts::fontNameAdvance;

        charList.push_back(dialogChar);
    }

    return charList;
}
