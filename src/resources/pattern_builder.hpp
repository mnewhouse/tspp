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

#ifndef RESOURCES_PATTERN_BUILDER_HPP
#define RESOURCES_PATTERN_BUILDER_HPP

#include "pattern.hpp"
#include "track.hpp"
#include "pattern_loader.hpp"

namespace ts
{
    namespace resources
    {
        class Track;

        class Pattern_builder
        {
        public:
            Pattern_builder(const Track& track);

            Pattern operator()(std::function<void()> operation = nullptr);

            void preload_pattern(const utf8_string& pattern_path);

        private:
            const utf8_string& resolve_include_path(const utf8_string& path);

            const Track& track_;

            Pattern_loader pattern_loader_;
            std::map<utf8_string, utf8_string> precomputed_paths_;
        };
    }
}


#endif