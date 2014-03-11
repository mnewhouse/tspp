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

#include "pattern_loader.hpp"
#include "pattern.hpp"

ts::Handle<ts::resources::Pattern> ts::resources::Pattern_loader::load_from_file(const std::string& file_name)
{
    auto it = loaded_patterns_.find(file_name);
    if (it != loaded_patterns_.end()) {
        return it->second;
    }

    
    Handle<Pattern> pattern_handle(Pattern{ file_name });
    loaded_patterns_.insert(std::make_pair(file_name, pattern_handle));

    return pattern_handle;
}