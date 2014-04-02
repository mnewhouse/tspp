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

#include "settings.hpp"

#include "core/directive_reader.hpp"

#include <fstream>
#include <sstream>

namespace ts
{
    namespace resources
    {
        std::istream& operator>>(std::istream& stream, Video_settings& video_settings);
        std::istream& operator>>(std::istream& stream, Input_settings& input_settings);
        std::istream& operator>>(std::istream& stream, Audio_settings& audio_settings);
        std::istream& operator>>(std::istream& stream, Game_settings& game_settings);

        std::ostream& operator<<(std::ostream& stream, const Video_settings& video_settings);
        std::ostream& operator<<(std::ostream& stream, const Input_settings& input_settings);
        std::ostream& operator<<(std::ostream& stream, const Audio_settings& audio_settings);
        std::ostream& operator<<(std::ostream& stream, const Game_settings& game_settings);
    }
}

ts::resources::Settings ts::resources::load_settings(const std::string& file_name)
{
    std::ifstream stream(file_name);
    return load_settings(stream);
}

ts::resources::Settings ts::resources::load_settings(std::istream& stream)
{
    Settings result;

    result.input_settings.key_mapping = controls::default_key_mapping();

    for (std::string line, directive; std::getline(stream, line);) {
        std::istringstream line_stream(line);
        if (!read_directive(line_stream, directive)) continue;

        if (directive == "section")
        {
            std::string section;
            if (read_directive(line_stream, section))
            {
                if (section == "input")
                {
                    stream >> result.input_settings;
                }

                else if (section == "video")
                {
                    stream >> result.video_settings;
                }

                else if (section == "audio")
                {
                    stream >> result.audio_settings;
                }

                else if (section == "game")
                {
                    stream >> result.game_settings;
                }
            }
        }       
    }

    return result;
}

void ts::resources::save_settings(const std::string& file_name, const Settings& settings)
{
    std::ofstream stream(file_name);
    stream << "Section Game\n";
    stream << settings.game_settings;
    stream << "End\n\n";

    stream << "Section Video\n";
    stream << settings.video_settings;
    stream << "End\n\n";

    stream << "Section Input\n";
    stream << settings.input_settings;
    stream << "End\n\n";

    stream << "Section Audio\n";
    stream << settings.audio_settings;
    stream << "End\n\n";
}

std::istream& ts::resources::operator>>(std::istream& stream, Video_settings& video_settings)
{
    for (std::string line, directive; directive != "end" && std::getline(stream, line);)
    {
        std::istringstream line_stream(line);
        if (!read_directive(line_stream, directive)) continue;

        if (directive == "fullscreen")
        {
            int full_screen = 0;
            if (line_stream >> full_screen) 
            {
                video_settings.full_screen = (full_screen != 0);
            }
        }

        else if (directive == "screenresolution")
        {
            Vector2u screen_resolution;
            if (line_stream >> screen_resolution)
            {
                video_settings.screen_resolution = screen_resolution;
            }
        }
    }

    return stream;
}

std::istream& ts::resources::operator>>(std::istream& stream, Input_settings& input_settings)
{
    auto& key_mapping = input_settings.key_mapping;
    key_mapping = controls::default_key_mapping();
    using controls::Control;

    for (std::string line, directive; directive != "end" && std::getline(stream, line);)
    {
        std::istringstream line_stream(line);
        if (!read_directive(line_stream, directive)) continue;

        auto read_key_bind = [&key_mapping](std::istream& stream, Control control)
        {
            controls::Slot slot;
            std::int32_t key_code;

            if (stream >> slot >> key_code)
            {
                auto key = controls::key_from_code(key_code);

                if (key != sf::Keyboard::Unknown)
                {
                    key_mapping.bind_key_to_control(key, slot, control);
                }
            }
        };

        if (directive == "acceleratekey")
        {
            read_key_bind(line_stream, Control::Accelerate);
        }

        else if (directive == "brakekey")
        {
            read_key_bind(line_stream, Control::Brake);
        }

        else if (directive == "leftkey")
        {
            read_key_bind(line_stream, Control::Left);
        }

        else if (directive == "rightkey")
        {
            read_key_bind(line_stream, Control::Right);
        }

        else if (directive == "firekey")
        {
            read_key_bind(line_stream, Control::Fire);
        }

        else if (directive == "altfirekey")
        {
            read_key_bind(line_stream, Control::Alt_fire);
        }
    }

    return stream;
}

std::istream& ts::resources::operator>>(std::istream& stream, Audio_settings& audio_settings)
{
    for (std::string line, directive; directive != "end" && std::getline(stream, line);)
    {
        std::istringstream line_stream(line);
        if (!read_directive(line_stream, directive)) continue;
    }

    return stream;
}

std::istream& ts::resources::operator>>(std::istream& stream, Game_settings& game_settings)
{
    for (std::string line, directive; directive != "end" && std::getline(stream, line);)
    {
        std::istringstream line_stream(line);
        if (!read_directive(line_stream, directive)) continue;
    }

    return stream;
}

std::ostream& ts::resources::operator<<(std::ostream& stream, const Video_settings& video_settings)
{
    stream << "FullScreen " << static_cast<int>(video_settings.full_screen) << "\n";
    stream << "ScreenResolution " << video_settings.screen_resolution.x << " " << video_settings.screen_resolution.y << "\n";
    return stream;
}

std::ostream& ts::resources::operator<<(std::ostream& stream, const Input_settings& input_settings)
{
    using controls::Control;

    auto control_to_setting_string = [](Control control) -> const char*
    {
        switch (control)
        {
        case Control::Accelerate: return "AccelerateKey";
        case Control::Brake: return "BrakeKey";
        case Control::Left: return "LeftKey";
        case Control::Right: return "RightKey";
        case Control::Fire: return "FireKey";
        case Control::Alt_fire: return "AltFireKey";
        default: return nullptr;
        }        
    };

    for (auto& bind : input_settings.key_mapping)
    {
        auto setting = control_to_setting_string(bind.second);
        if (setting)
        {
            stream << setting << " " << bind.first.slot << " " << static_cast<std::int32_t>(bind.first.key) << "\n";
        }
    }

    return stream;
}

std::ostream& ts::resources::operator<<(std::ostream& stream, const Audio_settings& audio_settings)
{
    return stream;
}

std::ostream& ts::resources::operator<<(std::ostream& stream, const Game_settings& game_settings)
{
    return stream;
}