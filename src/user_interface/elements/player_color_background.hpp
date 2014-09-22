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

#include "../background.hpp"

#include "resources/player_color.hpp"

namespace ts
{
    namespace gui
    {
        class Player_color_background
            : public Background_base
        {
        public:
            Player_color_background(resources::Player_color player_color = resources::Player_color());

            virtual void draw(const gui::Element& element, sf::RenderTarget& render_target, sf::RenderStates render_states) const override;

            void set_color(resources::Player_color player_color);
            void set_base_color(resources::Color_base base);
            void add_extra_color(resources::Color_extra extra);

        private:
            void update_vertices();

            resources::Player_color player_color_;
            std::vector<sf::Vertex> vertices_;
        };
    }
}