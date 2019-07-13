////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2017 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SPSS/Graphics/MulticolorText.h>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <cmath>

namespace spss {

    // Add an underline or strikethrough line to the vertex array
    void addLine(sf::VertexArray& vertices, float lineLength, float lineTop, const sf::Color& color, float offset, float thickness, float outlineThickness = 0)
    {
        float top = std::floor(lineTop + offset - (thickness / 2) + 0.5f);
        float bottom = top + std::floor(thickness + 0.5f);

        vertices.append(sf::Vertex(sf::Vector2f(-outlineThickness,             top    - outlineThickness), color, sf::Vector2f(1, 1)));
        vertices.append(sf::Vertex(sf::Vector2f(lineLength + outlineThickness, top    - outlineThickness), color, sf::Vector2f(1, 1)));
        vertices.append(sf::Vertex(sf::Vector2f(-outlineThickness,             bottom + outlineThickness), color, sf::Vector2f(1, 1)));
        vertices.append(sf::Vertex(sf::Vector2f(-outlineThickness,             bottom + outlineThickness), color, sf::Vector2f(1, 1)));
        vertices.append(sf::Vertex(sf::Vector2f(lineLength + outlineThickness, top    - outlineThickness), color, sf::Vector2f(1, 1)));
        vertices.append(sf::Vertex(sf::Vector2f(lineLength + outlineThickness, bottom + outlineThickness), color, sf::Vector2f(1, 1)));
    }

    // Add a glyph quad to the vertex array
    void addGlyphQuad(sf::VertexArray& vertices, sf::Vector2f position, const sf::Color& color, const sf::Glyph& glyph, float italicShear, float outlineThickness = 0)
    {
        float padding = 1.0;

        float left   = glyph.bounds.left - padding;
        float top    = glyph.bounds.top - padding;
        float right  = glyph.bounds.left + glyph.bounds.width + padding;
        float bottom = glyph.bounds.top  + glyph.bounds.height + padding;

        float u1 = static_cast<float>(glyph.textureRect.left) - padding;
        float v1 = static_cast<float>(glyph.textureRect.top) - padding;
        float u2 = static_cast<float>(glyph.textureRect.left + glyph.textureRect.width) + padding;
        float v2 = static_cast<float>(glyph.textureRect.top  + glyph.textureRect.height) + padding;

        vertices.append(sf::Vertex(sf::Vector2f(position.x + left  - italicShear * top    - outlineThickness, position.y + top    - outlineThickness), color, sf::Vector2f(u1, v1)));
        vertices.append(sf::Vertex(sf::Vector2f(position.x + right - italicShear * top    - outlineThickness, position.y + top    - outlineThickness), color, sf::Vector2f(u2, v1)));
        vertices.append(sf::Vertex(sf::Vector2f(position.x + left  - italicShear * bottom - outlineThickness, position.y + bottom - outlineThickness), color, sf::Vector2f(u1, v2)));
        vertices.append(sf::Vertex(sf::Vector2f(position.x + left  - italicShear * bottom - outlineThickness, position.y + bottom - outlineThickness), color, sf::Vector2f(u1, v2)));
        vertices.append(sf::Vertex(sf::Vector2f(position.x + right - italicShear * top    - outlineThickness, position.y + top    - outlineThickness), color, sf::Vector2f(u2, v1)));
        vertices.append(sf::Vertex(sf::Vector2f(position.x + right - italicShear * bottom - outlineThickness, position.y + bottom - outlineThickness), color, sf::Vector2f(u2, v2)));
    }
} // namespace

namespace spss {

    ////////////////////////////////////////////////////////////
    MulticolorText::MulticolorText()
        : m_string                (),
          m_font                  (NULL),
          m_characterSize         (30),
          m_letterSpacingFactor   (1.f),
          m_lineSpacingFactor     (1.f),
          m_style                 (Regular),
          m_styleFillColor        (255, 255, 255),
          m_styleOutlineColor     (0, 0, 0),
          m_styleOutlineThickness (0),
          m_vertices              (sf::Triangles),
          m_outlineVertices       (sf::Triangles),
          m_bounds                (),
          m_geometryNeedUpdate    (false),
          m_fillColors            (),
          m_outlineColors         (),
          m_outlineThicknesses    (),
          m_vertexIndeces         () {
    }

    ////////////////////////////////////////////////////////////
    MulticolorText::MulticolorText(const sf::String& string,
                                   const sf::Font&   font,
                                   unsigned int  characterSize)
        : m_string                  (string),
          m_font                    (&font),
          m_characterSize(characterSize),
          m_letterSpacingFactor     (1.f),
          m_lineSpacingFactor       (1.f),
          m_style                   (Regular),
          m_styleFillColor          (255, 255, 255),
          m_styleOutlineColor       (0, 0, 0),
          m_styleOutlineThickness   (0),
          m_vertices                (sf::Triangles),
          m_outlineVertices         (sf::Triangles),
          m_bounds                  (),
          m_geometryNeedUpdate      (true),
          m_fillColors              (),
          m_outlineColors           (),
          m_outlineThicknesses      (),
          m_vertexIndeces           () {
    }

    ////////////////////////////////////////////////////////////
    //When the string is changed, we're going to have to clear
    //all colour/thickness data, because the positions will no
    //longer be accurate. They must be re-set if needed.
    ////////////////////////////////////////////////////////////
    void MulticolorText::setString(const sf::String& string) {
        if (m_string != string) {
            m_fillColors.clear();
            m_outlineColors.clear();
            m_outlineThicknesses.clear();
            m_string             = string;
            m_geometryNeedUpdate = true;
        }
    }

    ////////////////////////////////////////////////////////////
    void MulticolorText::setFont(const sf::Font& font) {
        if (m_font != &font) {
            m_font               = &font;
            m_geometryNeedUpdate = true;
        }
    }

    ////////////////////////////////////////////////////////////
    void MulticolorText::setCharacterSize(unsigned int size) {
        if (m_characterSize != size) {
            m_characterSize      = size;
            m_geometryNeedUpdate = true;
        }
    }

    ////////////////////////////////////////////////////////////
    void MulticolorText::setLetterSpacing(float spacingFactor)
    {
        if (m_letterSpacingFactor != spacingFactor)
        {
            m_letterSpacingFactor = spacingFactor;
            m_geometryNeedUpdate = true;
        }
    }


    ////////////////////////////////////////////////////////////
    void MulticolorText::setLineSpacing(float spacingFactor)
    {
        if (m_lineSpacingFactor != spacingFactor)
        {
            m_lineSpacingFactor = spacingFactor;
            m_geometryNeedUpdate = true;
        }
    }

    ////////////////////////////////////////////////////////////
    void MulticolorText::setStyle(sf::Uint32 style) {
        if (m_style != style) {
            m_style              = style;
            m_geometryNeedUpdate = true;
        }
    }

    ////////////////////////////////////////////////////////////
    void MulticolorText::setFillColor(const sf::Color& color,
                                      size_t       _startPos,
                                      size_t       _endPos) {
        if (_endPos == 0) {
            _endPos = m_string.getSize() - 1;
        }

        for (std::size_t i = _startPos; i <= _endPos; ++i) {
            if (m_fillColors.count(i)) {
                m_fillColors[i] = color;
            }
            else {
                m_fillColors.insert(std::make_pair(i, color));
            }
        }

        m_geometryNeedUpdate = true;
    }

    void MulticolorText::setTransparency(int _alpha) {
        for (std::size_t i = 0; i < m_vertices.getVertexCount(); ++i)
            m_vertices[i].color.a = _alpha;

        for (std::size_t i = 0; i < m_outlineVertices.getVertexCount(); ++i)
            m_outlineVertices[i].color.a = _alpha;
    }

    ////////////////////////////////////////////////////////////
    void MulticolorText::setStyleFillColor(const sf::Color& color) {
        m_styleFillColor = color;
    }

    void MulticolorText::setStyleOutlineColor(const sf::Color& color) {
        m_styleOutlineColor = color;
    }

    ////////////////////////////////////////////////////////////
    void MulticolorText::setOutlineColor(const sf::Color& color,
                                         size_t       _startPos,
                                         size_t       _endPos) {
        if (_endPos == 0) {
            _endPos = m_string.getSize() - 1;
        }

        for (std::size_t i = _startPos; i <= _endPos; ++i) {
            if (m_outlineColors.count(i)) {
                m_outlineColors[i] = color;
            }
            else {
                m_outlineColors.insert(std::make_pair(i, color));
            }
        }

        //Since m_vertexIndeces only caches the positions of letters and the indeces of the first
        //vertex used to draw them, and doesn't account for outlines, we'll have to update the geometry
        m_geometryNeedUpdate = true;
    }

    ////////////////////////////////////////////////////////////
    void MulticolorText::setOutlineThickness(float  thickness,
            size_t _startPos,
            size_t _endPos) {
        if(m_string.isEmpty()) {
            return;
        }

        if (_endPos == 0) {
            _endPos = m_string.getSize() - 1;
        }

        for (std::size_t i = _startPos; i <= _endPos; ++i) {
            if (m_outlineThicknesses.count(i)) {
                m_outlineThicknesses[i] = thickness;
            }
            else {
                m_outlineThicknesses.insert(std::make_pair(i, thickness));
            }
        }

        m_geometryNeedUpdate = true;
    }

    ////////////////////////////////////////////////////////////
    const sf::String& MulticolorText::getString() const {
        return m_string;
    }

    ////////////////////////////////////////////////////////////
    const sf::Font* MulticolorText::getFont() const {
        return m_font;
    }

    ////////////////////////////////////////////////////////////
    unsigned int MulticolorText::getCharacterSize() const {
        return m_characterSize;
    }

    ////////////////////////////////////////////////////////////
    float MulticolorText::getLetterSpacing() const
    {
        return m_letterSpacingFactor;
    }


    ////////////////////////////////////////////////////////////
    float MulticolorText::getLineSpacing() const
    {
        return m_lineSpacingFactor;
    }

    ////////////////////////////////////////////////////////////
    sf::Uint32 MulticolorText::getStyle() const {
        return m_style;
    }

    ////////////////////////////////////////////////////////////
    const sf::Color MulticolorText::getFillColor(size_t _index) const {
        if (m_fillColors.empty() || (!m_fillColors.count(_index))) {
            return {255, 255, 255};
        }

        return m_fillColors[_index];
    }

    ////////////////////////////////////////////////////////////
    const sf::Color MulticolorText::getOutlineColor(size_t _index) const {
        if (m_outlineColors.empty() || (!m_outlineColors.count(_index))) {
            return {0, 0, 0};
        }

        return m_outlineColors[_index];
    }

    ////////////////////////////////////////////////////////////
    float MulticolorText::getOutlineThickness(size_t _index) const {
        if (m_outlineThicknesses.empty() ||
                (!m_outlineThicknesses.count(_index))) {
            return 0;
        }

        return m_outlineThicknesses[_index];
    }

    ////////////////////////////////////////////////////////////
    float MulticolorText::getStyleOutlineThickness() const {
        return m_styleOutlineThickness;
    }

    ////////////////////////////////////////////////////////////
    sf::Vector2f MulticolorText::findCharacterPos(std::size_t index) const {
        // Make sure that we have a valid font
        if (!m_font)
            return sf::Vector2f();

        // Adjust the index if it's out of range
        if (index > m_string.getSize())
            index = m_string.getSize();

        // Precompute the variables needed by the algorithm
        bool  isBold          = m_style & Bold;
        float whitespaceWidth = m_font->getGlyph(L' ', m_characterSize, isBold).advance;
        float letterSpacing   = ( whitespaceWidth / 3.f ) * ( m_letterSpacingFactor - 1.f );
        whitespaceWidth      += letterSpacing;
        float lineSpacing     = m_font->getLineSpacing(m_characterSize) * m_lineSpacingFactor;

        // Compute the position
        sf::Vector2f position;
        sf::Uint32 prevChar = 0;
        for (std::size_t i = 0; i < index; ++i)
        {
            sf::Uint32 curChar = m_string[i];

            // Apply the kerning offset
            position.x += m_font->getKerning(prevChar, curChar, m_characterSize);
            prevChar = curChar;

            // Handle special characters
            switch (curChar)
            {
                case ' ':
                    position.x += whitespaceWidth;
                    continue;
                case '\t':
                    position.x += whitespaceWidth * 4;
                    continue;
                case '\n':
                    position.y += lineSpacing;
                    position.x = 0;
                    continue;
            }

            // For regular characters, add the advance offset of the glyph
            position.x += m_font->getGlyph(curChar, m_characterSize, isBold).advance + letterSpacing;
        }

        // Transform the position to global coordinates
        position = getTransform().transformPoint(position);

        return position;
    }

    ////////////////////////////////////////////////////////////
    sf::FloatRect MulticolorText::getLocalBounds() const {
        ensureGeometryUpdate();

        return m_bounds;
    }

    ////////////////////////////////////////////////////////////
    sf::FloatRect MulticolorText::getGlobalBounds() const {
        return getTransform().transformRect(getLocalBounds());
    }

    ////////////////////////////////////////////////////////////
    void MulticolorText::draw(sf::RenderTarget& target,
                              sf::RenderStates  states) const {
        if (m_font) {
            ensureGeometryUpdate();

            states.transform *= getTransform();
            states.texture = &m_font->getTexture(m_characterSize);

            // Only draw the outline if there is something to draw
            if (!m_outlineThicknesses.empty()) {
                target.draw(m_outlineVertices, states);
            }

            target.draw(m_vertices, states);
        }
    }

    ////////////////////////////////////////////////////////////
    void MulticolorText::ensureGeometryUpdate() const {
        if (!m_font)
            return;

        // Do nothing, if geometry has not changed and the font texture has not changed
        if (!m_geometryNeedUpdate)
            return;

        // Mark geometry as updated
        m_geometryNeedUpdate = false;

        // Clear the previous geometry
        m_vertexIndeces.clear();
        m_vertices.clear();
        m_outlineVertices.clear();
        m_bounds = sf::FloatRect();

        // No text: nothing to draw
        if (m_string.isEmpty())
            return;

        // Compute values related to the text style
        bool  isBold             = m_style & Bold;
        bool  isUnderlined       = m_style & Underlined;
        bool  isStrikeThrough    = m_style & StrikeThrough;
        float italicShear        = (m_style & Italic) ? 0.209f : 0.f; // 12 degrees in radians
        float underlineOffset    = m_font->getUnderlinePosition(m_characterSize);
        float underlineThickness = m_font->getUnderlineThickness(m_characterSize);

        // Compute the location of the strike through dynamically
        // We use the center point of the lowercase 'x' glyph as the reference
        // We reuse the underline thickness as the thickness of the strike through as well
        sf::FloatRect xBounds = m_font->getGlyph(L'x', m_characterSize, isBold).bounds;
        float strikeThroughOffset = xBounds.top + xBounds.height / 2.f;

        // Precompute the variables needed by the algorithm
        float whitespaceWidth = m_font->getGlyph(L' ', m_characterSize, isBold).advance;
        float letterSpacing   = ( whitespaceWidth / 3.f ) * ( m_letterSpacingFactor - 1.f );
        whitespaceWidth      += letterSpacing;
        float lineSpacing     = m_font->getLineSpacing(m_characterSize) * m_lineSpacingFactor;
        float x               = 0.f;
        float y               = static_cast<float>(m_characterSize);

        // Create one quad for each character
        float minX = static_cast<float>(m_characterSize);
        float minY = static_cast<float>(m_characterSize);
        float maxX = 0.f;
        float maxY = 0.f;
        sf::Uint32 prevChar = 0;
        for (std::size_t i = 0; i < m_string.getSize(); ++i) {

            sf::Uint32 curChar = m_string[i];

            // Skip the \r char to avoid weird graphical issues
            if (curChar == '\r')
                continue;

            //////////////////////////////////////////////////////////////////////
            if (!m_fillColors.count(i)) {
                m_fillColors.insert(std::make_pair(i, sf::Color(255, 255, 255)));
            }

            if (!m_outlineColors.count(i)) {
                m_outlineColors.insert(std::make_pair(i, sf::Color(0, 0, 0)));
            }

            if (!m_outlineThicknesses.count(i)) {
                m_outlineThicknesses.insert(std::make_pair(i, 0));
            }

            sf::Color  curFillColor    = m_fillColors[i];
            sf::Color  curOutlineColor = m_outlineColors[i];
            float  curOutlineThickness = m_outlineThicknesses[i];
            //////////////////////////////////////////////////////////////////////

            // Apply the kerning offset
            x += m_font->getKerning(prevChar, curChar, m_characterSize);


            // If we're using the underlined style and there's a new line, draw a line
            if (isUnderlined && (curChar == L'\n' && prevChar != L'\n')) {
                addLine(m_vertices, x, y, m_styleFillColor, underlineOffset, underlineThickness);

                if (curOutlineThickness != 0)
                    addLine(m_outlineVertices, x, y, curOutlineColor, underlineOffset, underlineThickness, curOutlineThickness);
            }

            // If we're using the strike through style and there's a new line, draw a line across all characters
            if (isStrikeThrough && (curChar == L'\n' && prevChar != L'\n')) {
                addLine(m_vertices, x, y, m_styleFillColor, strikeThroughOffset, underlineThickness);

                if (curOutlineThickness != 0)
                    addLine(m_outlineVertices, x, y, curOutlineColor, strikeThroughOffset, underlineThickness, curOutlineThickness);
            }

            prevChar = curChar;

            // Handle special characters
            if ((curChar == L' ') || (curChar == L'\n') || (curChar == L'\t')) {
                // Update the current bounds (min coordinates)
                minX = std::min(minX, x);
                minY = std::min(minY, y);

                switch (curChar)
                {
                    case L' ':
                        x += whitespaceWidth;
                        break;
                    case L'\t':
                        x += whitespaceWidth * 4;
                        break;
                    case L'\n':
                        y += lineSpacing;
                        x = 0;
                        break;
                }

                // Update the current bounds (max coordinates)
                maxX = std::max(maxX, x);
                maxY = std::max(maxY, y);

                // Next glyph, no need to create a quad for whitespace
                continue;
            }

            // Apply the outline
            if (curOutlineThickness != 0) {
                const sf::Glyph& glyph = m_font->getGlyph(curChar, m_characterSize, isBold, curOutlineThickness);

                float left   = glyph.bounds.left;
                float top    = glyph.bounds.top;
                float right  = glyph.bounds.left + glyph.bounds.width;
                float bottom = glyph.bounds.top + glyph.bounds.height;

                // Add the outline glyph to the vertices
                addGlyphQuad(m_outlineVertices, sf::Vector2f(x, y), curOutlineColor, glyph, italicShear, curOutlineThickness);

                // Update the current bounds with the outlined glyph bounds
                minX = std::min(minX, x + left  - italicShear * bottom - curOutlineThickness);
                maxX = std::max(maxX, x + right - italicShear * top    - curOutlineThickness);
                minY = std::min(minY, y + top    - curOutlineThickness);
                maxY = std::max(maxY, y + bottom - curOutlineThickness);
            }

            // Extract the current glyph's description
            const sf::Glyph& glyph = m_font->getGlyph(curChar, m_characterSize, isBold);

            // Add the glyph to the vertices
            addGlyphQuad(m_vertices, sf::Vector2f(x, y), curFillColor, glyph, italicShear);

            m_vertexIndeces.insert(std::make_pair(i, m_vertices.getVertexCount() - 6));

            // Update the current bounds with the non outlined glyph bounds
            if (curOutlineThickness == 0) {
                float left   = glyph.bounds.left;
                float top    = glyph.bounds.top;
                float right  = glyph.bounds.left + glyph.bounds.width;
                float bottom = glyph.bounds.top + glyph.bounds.height;

                minX = std::min(minX, x + left  - italicShear * bottom);
                maxX = std::max(maxX, x + right - italicShear * top);
                minY = std::min(minY, y + top);
                maxY = std::max(maxY, y + bottom);
            }

            // Advance to the next character
            x += glyph.advance + letterSpacing;
        }

        // If we're using the underlined style, add the last line
        if (isUnderlined && (x > 0)) {
            addLine(m_vertices, x, y, m_styleFillColor, underlineOffset, underlineThickness);

            if (m_styleOutlineThickness != 0)
                addLine(m_outlineVertices, x, y, m_styleOutlineColor, underlineOffset, underlineThickness, m_styleOutlineThickness);
        }

        // If we're using the strike through style, add the last line across all characters
        if (isStrikeThrough && (x > 0)) {
            addLine(m_vertices, x, y, m_styleFillColor, strikeThroughOffset, underlineThickness);

            if (m_styleOutlineThickness != 0)
                addLine(m_outlineVertices, x, y, m_styleOutlineColor, strikeThroughOffset, underlineThickness, m_styleOutlineThickness);
        }

        // Update the bounding rectangle
        m_bounds.left   = minX;
        m_bounds.top    = minY;
        m_bounds.width  = maxX - minX;
        m_bounds.height = maxY - minY;
    }

} // namespace spss
