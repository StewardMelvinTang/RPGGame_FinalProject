#include <allegro5/allegro_font.h>
#include <memory>
#include <sstream>
#include <vector>
#include "Engine/IObject.hpp"
#include "MultiLineLabel.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"

namespace Engine {
    MultiLineLabel::MultiLineLabel(const std::string& text, const std::string& font, int fontSize, float x, float y, unsigned char r, unsigned char g, unsigned char b, unsigned char a, float anchorX, float anchorY) :
        IObject(x, y, 0, 0, anchorX, anchorY), font(Resources::GetInstance().GetFont(font, fontSize)), Text(text), Color(al_map_rgba(r, g, b, a)) {
        SplitTextIntoLines();
    }

    void MultiLineLabel::SplitTextIntoLines() {
        std::istringstream stream(Text);
        std::string line;
        lines.clear();
        while (std::getline(stream, line)) {
            lines.push_back(line);
        }
    }

    void MultiLineLabel::Draw() const {
        float offsetY = 0;
        for (const auto& line : lines) {
            al_draw_text(font.get(), Color, Position.x - Anchor.x * GetTextWidth(line), Position.y - Anchor.y * GetTextHeight() + offsetY, 0, line.c_str());
            offsetY += al_get_font_line_height(font.get());
        }
    }

    int MultiLineLabel::GetTextWidth() const {
        int maxWidth = 0;
        for (const auto& line : lines) {
            int width = al_get_text_width(font.get(), line.c_str());
            if (width > maxWidth) {
                maxWidth = width;
            }
        }
        return maxWidth;
    }

    int MultiLineLabel::GetTextWidth(const std::string& line) const {
        return al_get_text_width(font.get(), line.c_str());
    }

    int MultiLineLabel::GetTextHeight() const {
        return al_get_font_line_height(font.get()) * lines.size();
    }

    void MultiLineLabel::SetText(const std::string& text) {
        Text = text;
        SplitTextIntoLines();
    }
}
