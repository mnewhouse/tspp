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

#include "stdinc.hpp"
#include "terrain_definition.hpp"

ts::resources::Terrain_definition::Terrain_definition()
: id(0),
  acceleration(1.0f),
  roughness(0.0f),  
  steering(1.0f),
  traction(1.0f),
  antislide(1.0f),  
  braking(1.0f),
  tire_mark(true),
  skid_mark(false),
  is_wall(false),
  color()
{
}

std::istream& ts::resources::operator>>(std::istream& stream, Terrain_definition& terrain_def)
{
    for (std::string line, directive; directive != "end" && std::getline(stream, line); )
    {
        boost::trim(line);
        std::istringstream line_stream(line);

        read_directive(line_stream, directive);

        if (directive == "id") 
        {
            std::uint32_t id;
            if (line_stream >> id) 
            {
                terrain_def.id = id;
            }
        }

        else if (directive == "steering") 
        {
            line_stream >> terrain_def.steering;
        }

        else if (directive == "antislide") 
        {
            line_stream >> terrain_def.antislide;
        }

        else if (directive == "acceleration") 
        {
            line_stream >> terrain_def.acceleration;
        }

        else if (directive == "roughness") 
        {
            line_stream >> terrain_def.roughness;
        }

        else if (directive == "grip") 
        {
            line_stream >> terrain_def.traction;
        }

        else if (directive == "braking") 
        {
            line_stream >> terrain_def.braking;
        }

        else if (directive == "bounciness") 
        {
            line_stream >> terrain_def.wall_definition.elasticity;
        }

        else if (directive == "red")
        {
            std::uint32_t value;
            if (line_stream >> value)
            {
                terrain_def.color.red = value;
            }
        }

        else if (directive == "green")
        {
            std::uint32_t value;
            if (line_stream >> value)
            {
                terrain_def.color.green = value;
            }     
        }

        else if (directive == "blue")
        {
            std::uint32_t value;
            if (line_stream >> value)
            {
                terrain_def.color.blue = value;
            }
        }

        else if (directive == "tiremark")
        {
            std::uint32_t value;
            if (line_stream >> value)
            {
                terrain_def.tire_mark = (value != 0);
            }
        }

        else if (directive == "skidmark")
        {
            std::uint32_t value;
            if (line_stream >> value)
            {
                terrain_def.skid_mark = (value != 0);
            }
        }

        else if (directive == "iswall") 
        {
            std::uint32_t value;
            if (line_stream >> value) 
            {
                terrain_def.is_wall = (value != 0);
            }
        }
    }

    return stream;
}

bool ts::resources::operator==(const Terrain_definition& first, const Terrain_definition& second)
{
    return first.id == second.id;
}

bool ts::resources::operator!=(const Terrain_definition& first, const Terrain_definition& second)
{
    return first.id != second.id;
}