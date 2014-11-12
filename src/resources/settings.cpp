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
#include "settings.hpp"

#include "car_store.hpp"

namespace ts
{
    namespace resources
    {
        void load_settings(const utf8_string& file_name, Settings& settings);
        void load_settings(std::istream& stream, Settings& settings);

        void save_settings(const utf8_string& file_name, const Settings& settings);

        std::istream& operator>>(std::istream& stream, Video_settings& video_settings);
        std::istream& operator>>(std::istream& stream, Input_settings& input_settings);
        std::istream& operator>>(std::istream& stream, Audio_settings& audio_settings);
        std::istream& operator>>(std::istream& stream, Track_settings& track_settings);
        std::istream& operator>>(std::istream& stream, Car_settings& car_settings);
        std::istream& operator>>(std::istream& stream, Player_settings& player_settings);
        std::istream& operator>>(std::istream& stream, Script_settings& script_settings);

        std::ostream& operator<<(std::ostream& stream, const Video_settings& video_settings);
        std::ostream& operator<<(std::ostream& stream, const Input_settings& input_settings);
        std::ostream& operator<<(std::ostream& stream, const Audio_settings& audio_settings);
        std::ostream& operator<<(std::ostream& stream, const Track_settings& track_settings);
        std::ostream& operator<<(std::ostream& stream, const Car_settings& car_settings);
        std::ostream& operator<<(std::ostream& stream, const Player_settings& player_settings);
        std::ostream& operator<<(std::ostream& stream, const Script_settings& script_settings);
    }
}

ts::resources::Settings::Settings(utf8_string file_name)
: file_name_(std::move(file_name))
{
    load_settings(file_name_, *this);
}

ts::resources::Settings::~Settings()
{
    save_settings(file_name_, *this);
}

void ts::resources::load_settings(const utf8_string& file_name, Settings& settings)
{
    boost::filesystem::ifstream stream(file_name.string());
    return load_settings(stream, settings);
}

void ts::resources::load_settings(std::istream& stream, Settings& settings)
{
    settings.input_settings.key_mapping = controls::default_key_mapping();

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
                    stream >> settings.input_settings;
                }

                else if (section == "video")
                {
                    stream >> settings.video_settings;
                }

                else if (section == "audio")
                {
                    stream >> settings.audio_settings;
                }

                else if (section == "tracks")
                {
                    stream >> settings.track_settings;
                }

                else if (section == "cars")
                {
                    stream >> settings.car_settings;
                }

                else if (section == "players")
                {
                    stream >> settings.player_settings;
                }

                else if (section == "scripts")
                {
                    stream >> settings.script_settings;
                }
            }
        }       
    }
}

void ts::resources::save_settings(const utf8_string& file_name, const Settings& settings)
{
    boost::filesystem::ofstream stream(file_name.string());

    stream << "Section Video\n";
    stream << settings.video_settings;
    stream << "End\n\n";

    stream << "Section Input\n";
    stream << settings.input_settings;
    stream << "End\n\n";

    stream << "Section Audio\n";
    stream << settings.audio_settings;
    stream << "End\n\n";

    stream << "Section Tracks\n";
    stream << settings.track_settings;
    stream << "End\n\n";

    stream << "Section Cars\n";
    stream << settings.car_settings;
    stream << "End\n\n";

    stream << "Section Players\n";
    stream << settings.player_settings;
    stream << "End\n\n";

    stream << "Section Scripts\n";
    stream << settings.script_settings;
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

        else if (directive == "verticalsync")
        {
            bool vsync;
            if (line_stream >> vsync)
            {
                video_settings.vertical_sync = vsync;
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


std::istream& ts::resources::operator>>(std::istream& stream, Track_settings& track_settings)
{
    for (std::string line, directive; directive != "end" && std::getline(stream, line);)
    {
        std::istringstream line_stream(line);
        if (!read_directive(line_stream, directive)) continue;

        else if (directive == "selectedtrack")
        {
            std::string track_path;
            if (line_stream >> track_path)
            {
                Track_handle track_handle(std::move(track_path));

                track_settings.add_track(track_handle);
            }
        }
    }

    return stream;
}

std::istream& ts::resources::operator>>(std::istream& stream, Car_settings& car_settings)
{
    Car_mode car_mode = Car_mode::Free;
    car_settings.set_car_mode(car_mode);

    for (std::string line, directive; directive != "end" && std::getline(stream, line);)
    {
        std::istringstream line_stream(line);
        if (!read_directive(line_stream, directive)) continue;

        if (directive == "carmode")
        {
            std::string car_mode_string;
            if (line_stream >> car_mode_string)
            {
                boost::to_lower(car_mode_string);

                if (car_mode_string == "fixed")
                {
                    car_mode = Car_mode::Fixed;
                }

                else if (car_mode_string == "random")
                {
                    car_mode = Car_mode::Random;
                }

                else
                {
                    car_mode = Car_mode::Free;
                }
            }
        }

        else if (directive == "selectedcar")
        {
            std::string car_name;
            if (line_stream >> car_name)
            {
                Car_identifier car_identifier;
                car_identifier.car_name = car_name;
                
                car_settings.select_car(car_identifier);
            }
        }
    }

    car_settings.set_car_mode(car_mode);
    return stream;
}

std::istream& ts::resources::operator>>(std::istream& stream, Player_settings& player_settings)
{
    for (std::string line, directive; directive != "end" && std::getline(stream, line);)
    {
        std::istringstream line_stream(line);
        if (!read_directive(line_stream, directive)) continue;

        if (directive == "selectedplayer")
        {
            int control_slot;
            Player_store::unique_id unique_id;
            if (line_stream >> control_slot >> unique_id)
            {
                player_settings.select_player(unique_id, control_slot);
            }
        }
    }
    return stream;
}

std::istream& ts::resources::operator>>(std::istream& stream, Script_settings& script_settings)
{    
    for (std::string line, directive, param; directive != "end" && std::getline(stream, line);)
    {
        std::istringstream line_stream(line);
        if (!read_directive(line_stream, directive)) continue;

        if (directive == "enabledscript")
        {
            line_stream >> std::ws;
            if (std::getline(line_stream, param))
            {
                script_settings.enable_script(param);
            }            
        }

        else if (directive == "defaultgamemode")
        {
            line_stream >> std::ws;
            if (std::getline(line_stream, param))
            {
                script_settings.set_default_gamemode(std::move(param));
            }
        }

        else if (directive == "forcegamemode")
        {
            std::int32_t force = 0;
            if (line_stream >> force)
            {
                script_settings.force_default_gamemode(force != 0);
            }
        }
    }

    return stream;
}

std::ostream& ts::resources::operator<<(std::ostream& stream, const Video_settings& video_settings)
{
    stream << "FullScreen " << +video_settings.full_screen << "\n";
    stream << "VerticalSync " << +video_settings.vertical_sync << "\n";
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

std::ostream& ts::resources::operator<<(std::ostream& stream, const Track_settings& track_settings)
{
    for (const auto& track_handle : track_settings.selected_tracks())
    {
        stream << "SelectedTrack " << track_handle.path() << "\n";
    }

    return stream;
}

std::ostream& ts::resources::operator<<(std::ostream& stream, const Car_settings& car_settings)
{
    auto car_mode_string = [](Car_mode car_mode) -> const char*
    {
        switch (car_mode)
        {
        case Car_mode::Fixed: return "Fixed";
        case Car_mode::Free: return "Free";
        case Car_mode::Random: return "Random";
        default: return nullptr;
        }
    }(car_settings.car_mode());

    if (car_mode_string)
    {
        stream << "CarMode " << car_mode_string << "\n";
    }    

    for (const auto& car_identifier : car_settings.selected_cars())
    {
        stream << "SelectedCar " << car_identifier.car_name << "\n";
    }

    return stream;
}

std::ostream& ts::resources::operator<<(std::ostream& stream, const Player_settings& player_settings)
{
    const auto& selected_players = player_settings.selected_players();

    for (auto player_slot = 0; player_slot != selected_players.size(); ++player_slot)
    {
        if (auto unique_id = selected_players[player_slot])
        {
            stream << "SelectedPlayer " << player_slot << " " << unique_id << std::endl;
        }
    }

    return stream;
}

std::ostream& ts::resources::operator<<(std::ostream& stream, const Script_settings& script_settings)
{
    stream << "DefaultGamemode " << script_settings.default_gamemode() << std::endl;
    stream << "ForceGamemode " << +script_settings.force_default_gamemode() << std::endl;

    for (const auto& script_name : script_settings.loaded_scripts())
    {
        stream << "EnabledScript " << script_name << std::endl;
    }

    return stream;
}