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

#ifndef RESOURCES_PATTERN_LOADER_HPP
#define RESOURCES_PATTERN_LOADER_HPP

#include "core/handle.hpp"

#include <string>
#include <unordered_map>

namespace ts
{
    namespace resources
    {
        class Pattern;

        class Pattern_loader
        {
        public:
            Handle<Pattern> load_from_file(const std::string& file_name);

        private:
            std::unordered_map<std::string, Handle<Pattern>> loaded_patterns_;
        };
    }
}

#endif