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

#ifndef GRAPHICS_FPS_DISPLAY_HPP
#define GRAPHICS_FPS_DISPLAY_HPP

#include "fps_counter.hpp"
#include "render_scene.hpp"

namespace ts
{

    namespace graphics
    {

        class FPS_Display
            : public Render_scene
        {
        public:
            virtual void render(Render_target& render_target) override;

        private:
            mutable FPS_Counter fps_counter_;
        };

    }

}

#endif