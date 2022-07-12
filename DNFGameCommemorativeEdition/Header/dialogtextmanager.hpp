#pragma once

#include "shaderprogram.hpp"
#include "constant.hpp"
#include "dialogchar.hpp"

#include <map>

#include <glm.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

/*
 * FreeType is a software development library that is able to load fonts, render them to bitmaps,
 * and provide support for several font-related operations. It is a popular library used by Mac OS
 * X, Java, PlayStation, Linux, and Android to name a few. What makes FreeType particularly
 * attractive is that it is able to load TrueType fonts.
 *
 * A TrueType font is a collection of character glyphs not defined by pixels or any other
 * non-scalable solution, but by mathematical equations (combinations of splines). Similar to vector
 * images, the rasterized font images can be procedurally generated based on the preferred font
 * height you'd like to obtain them in. By using TrueType fonts you can easily render character
 * glyphs of various sizes without any loss of quality.
 *
 * FreeType loads these TrueType fonts and, for each glyph, generates a bitmap image and calculates
 * several metrics. We can extract these bitmap images for generating textures and position each
 * character glyph appropriately using the loaded metrics.
 *
 */

class DialogTextManager
{
public:
    DialogTextManager(ShaderProgram* shader);
    ~DialogTextManager() {}

    void setDialogTextInitialTrans(const glm::vec2& initialTrans);
    void setDialogNameShift(const glm::vec2& nameShift);
    void setDialogTextShift(const glm::vec2& textShift);

    // Assume the string text is for english chars,
    // wstring is for chinese chars (names).
    std::vector<DialogChar*> getDialogTextCharNodeListFromString(const std::string& string);
    std::vector<DialogChar*> getDialogNameCharNodeListFromString(const std::wstring& string);

private:
    void initialize();

    ShaderProgram* m_shader;

    // Free type lib
    FT_Library m_free_type_lib;

    // Glyph (∏Ò¿Ÿ‹Ω) texture id eng map.
    std::map<char, Fonts::CharacterInfo> m_glyph_tex_map_eng;

    // Glyph (∏Ò¿Ÿ‹Ω) texture id ch map.
    std::map<wchar_t, Fonts::CharacterInfo> m_glyph_tex_map_ch;

    glm::vec2 m_char_trans;
    glm::vec2 m_char_name_trans;
    glm::vec2 m_char_text_trans;
};