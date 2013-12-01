/*
 * Turbo Sliders++
 * Copyright (C) 2013 Martin Newhouse
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

#ifndef GRAPHICS_TEXTURE_LOADER_HPP
#define GRAPHICS_TEXTURE_LOADER_HPP

#include "core/handle.hpp"

#include <unordered_map>

namespace ts
{

    namespace graphics
    {
        class Texture;

        class Texture_loader
        {
        public:
            Handle<Texture> load_from_file(const std::string& file_name);

        private:
            std::unordered_map<std::string, Handle<Texture>> loaded_textures_;
        };


    }

}

#endif