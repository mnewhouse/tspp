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

#include "terrain_definition.hpp"
#include "core/directive_reader.hpp"

#include <boost/algorithm/string.hpp>
#include <sstream>

ts::resources::Terrain_definition::Terrain_definition()
: id(0),
  acceleration(1.0f),
  viscosity(1.0f),  
  steering(1.0f),
  grip(1.0f),
  anti_slide(1.0f),  
  braking(1.0f),  
  is_wall(false)
{
}

std::istream& ts::resources::operator>>(std::istream& stream, Terrain_definition& terrain_def)
{
    for (std::string line, directive; directive != "end" && std::getline(stream, line); )
    {
        boost::trim(line);
        std::istringstream line_stream(line);

        read_directive(line_stream, directive);

        if (directive == "id") {
            int id;
            if (line_stream >> id) {
                terrain_def.id = id;
            }
        }

        else if (directive == "steering") {
            line_stream >> terrain_def.steering;
        }

        else if (directive == "antislide") {
            line_stream >> terrain_def.anti_slide;
        }

        else if (directive == "acceleration") {
            line_stream >> terrain_def.acceleration;
        }

        else if (directive == "viscosity") {
            line_stream >> terrain_def.viscosity;
        }

        else if (directive == "grip") {
            line_stream >> terrain_def.grip;
        }

        else if (directive == "braking") {
            line_stream >> terrain_def.braking;
        }

        else if (directive == "bounciness") {
            line_stream >> terrain_def.wall_definition.elasticity;
        }

        else if (directive == "iswall") {
            int value;
            if (line_stream >> value) {
                terrain_def.is_wall = (value != 0);
            }
        }
    }

    return stream;
}