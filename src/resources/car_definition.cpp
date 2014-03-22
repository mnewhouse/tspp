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

#include "car_definition.hpp"

#include "core/directive_reader.hpp"
#include "core/config.hpp"

#include "pattern.hpp"

#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

ts::resources::Car_definition ts::resources::load_car_definition(std::istream& stream, const std::string& directory)
{
    Car_definition car_def;

    for (std::string line, directive; directive != "end" && std::getline(stream, line); ) {
        boost::trim(line);
        std::istringstream line_stream(line);
        if (!read_directive(line_stream, directive)) continue;

        if (directive == "image" || directive == "rotimage") {
            std::string file;
            Int_rect rect;
            float scale;            

            if (line_stream >> file >> rect >> scale)
            {
                car_def.image_file = find_include_path(file, { directory });
                car_def.image_scale = scale;
                car_def.image_rect = rect;                
                car_def.image_type = Image_type::Default;

                if (directive == "rotimage") car_def.image_type = Image_type::Rotational;
            }
        }

        else if (directive == "handling")
        {
            stream >> car_def.handling;
        }

        else if (directive == "mask") {
            std::string pattern_file;
            Int_rect pattern_rect;
            if (line_stream >> pattern_file >> pattern_rect) {
                car_def.pattern_file = find_include_path(pattern_file, { directory });
                car_def.pattern_rect = pattern_rect;

                car_def.pattern = std::make_shared<Pattern>(car_def.pattern_file);
            }
        }

        else if (directive == "friction") {
            line_stream >> car_def.wall_definition.friction;
        }

        else if (directive == "elasticity") {
            line_stream >> car_def.wall_definition.elasticity;
        }

        else if (directive == "enginesample") {
            std::string file_name;
            if (line_stream >> file_name)
            {
                car_def.engine_sample = find_include_path(file_name, { directory, config::sound_directory });
            }
        }
    }

    return car_def;
}

std::vector<ts::resources::Car_definition> ts::resources::load_car_definitions(std::istream& stream, const std::string& directory)
{
    std::vector<Car_definition> result;

    for (std::string line, directive; std::getline(stream, line); )
    {
        std::istringstream line_stream(line);
        if (!read_directive(line_stream, directive)) continue;

        if (directive == "car" && std::getline(stream, line)) 
        {
            Car_definition car_def;

            boost::trim(line);
            const auto& car_name = line;

            try {
                result.push_back(load_car_definition(stream, directory));
                result.back().car_name = car_name;
            }

            catch (...) {
                // Failed to load car data
            }
        }
    }

    return result;
}