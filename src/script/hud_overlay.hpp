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

#pragma once

#ifndef SCRIPT_HUD_HPP
#define SCRIPT_HUD_HPP

#include "graphics/render_scene.hpp"

#include "core/vector2.hpp"

#include <memory>
#include <cstdint>
#include <vector>


namespace ts
{
    namespace script
    {
        enum class Font_type
        {
            Sans,
            Monospace
        };

        class HUD_overlay;

        struct Text_display
        {
            sf::Text text;
            std::size_t ref_count = 0;
            HUD_overlay* hud;
        };

        class HUD_overlay
            : public graphics::Render_scene
        {
        public:
            virtual void render(graphics::Render_target& render_target) override;

            Text_display* create_text_display(const std::string& text, std::uint32_t character_size, Font_type font);
            
            void destroy_text_display(Text_display* text_display);

        private:
            std::vector<std::unique_ptr<Text_display>> text_displays_;
        };
    }
}

#endif