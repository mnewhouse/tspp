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

#ifndef CORE_DIRECTIVE_READER_HPP
#define CORE_DIRECTIVE_READER_HPP

#include <istream>
#include <string>

#include <boost/algorithm/string.hpp>

namespace ts
{

    inline bool read_directive(std::istream& stream, std::string& directive)
    {
        directive.clear();

        if (stream >> directive) {
            boost::to_lower(directive);
            return !directive.empty();
        }

        return false;
    }

}

#endif