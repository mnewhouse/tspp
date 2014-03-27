/*
 * Turbo Sliders++
 * Copyright (C) 2013-2014 Martin Newhouse
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "hud_overlay.hpp"
#include "graphics/fonts/font.hpp"
#include "graphics/fonts/sans.hpp"
#include "graphics/fonts/monospace.hpp"

ts::script::Text_display* ts::script::HUD_overlay::create_text_display(const std::string& text, std::uint32_t character_size, Font_type font_type)
{
    const auto* font = &ts::fonts::get_font_by_type<ts::fonts::Sans>();
    if (font_type == Font_type::Monospace) font = &ts::fonts::get_font_by_type<ts::fonts::Monospace>();

    auto text_display = std::make_unique<Text_display>();
    text_display->text.setString(text);
    text_display->text.setFont(*font);
    text_display->text.setCharacterSize(character_size);
    text_display->ref_count = 1;
    text_display->hud = this;

    text_displays_.push_back(std::move(text_display));
    return &*text_displays_.back();
}

void ts::script::HUD_overlay::destroy_text_display(Text_display* text_display)
{
    text_displays_.erase(std::remove_if(text_displays_.begin(), text_displays_.end(), 
                   [text_display](const std::unique_ptr<Text_display>& ptr)
    {
        return ptr.get() == text_display;
    }
    ), text_displays_.end());
}

void ts::script::HUD_overlay::render(graphics::Render_target& render_target)
{
    for (const auto& text_display : text_displays_)
    {
        render_target.draw(text_display->text);
    }
}