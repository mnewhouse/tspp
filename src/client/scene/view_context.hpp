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

#ifndef SCENE_VIEW_CONTEXT_HPP
#define SCENE_VIEW_CONTEXT_HPP

#include "camera.hpp"

namespace ts
{
    namespace scene
    {
        struct View
        {
            View(Vector2u screen_size, Vector2u world_size);

            Camera camera;
            Double_rect view_port;
        };

        
        class View_context
        {
        public:
            static const std::size_t max_views = 50;

            View_context(Vector2u screen_size, Vector2u track_size);

            void clear();
            
            void remove_view(std::size_t index);
            View* add_view();

            // Gets a view by index. Index _must_ be < view_count.
            const View& view(std::size_t index) const;
            View& view(std::size_t index);

            std::size_t view_count() const;

            const View* begin() const;
            View* begin();

            const View* end() const;
            View* end();

            void arrange_evenly();
            Vector2u screen_size() const;

        private:
            Vector2u screen_size_;
            Vector2u world_size_;

            std::vector<View> views_;
        };
    }
}

#endif