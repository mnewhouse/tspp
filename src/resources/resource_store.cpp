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
#include "resource_store.hpp"

#include "player_store.hpp"
#include "car_store.hpp"
#include "track_store.hpp"
#include "settings.hpp"
#include "script_manager.hpp"
#include "font_library.hpp"

ts::resources::Resource_store::Resource_store()
: car_store_(std::make_unique<Car_store>(config::car_directory)),
  track_store_(std::make_unique<Track_store>(config::track_root_directory)),
  player_store_(std::make_unique<Player_store>(config::players_file)),
  script_manager_(std::make_unique<Script_manager>(config::script_root_directory)),
  settings_(std::make_unique<Settings>(config::settings_file)),
  font_library_(std::make_unique<Font_library>())
{
    settings_->car_settings.load_car_definitions(&car_store());
}

ts::resources::Resource_store::~Resource_store()
{
}

ts::resources::Car_store& ts::resources::Resource_store::car_store()
{
    return *car_store_;
}

ts::resources::Track_store& ts::resources::Resource_store::track_store()
{
    return *track_store_;
}

ts::resources::Player_store& ts::resources::Resource_store::player_store()
{
    return *player_store_;
}

ts::resources::Script_manager& ts::resources::Resource_store::script_manager()
{
    return *script_manager_;
}

ts::resources::Settings& ts::resources::Resource_store::settings()
{
    return *settings_;
}

ts::resources::Font_library& ts::resources::Resource_store::font_library()
{
    return *font_library_;
}




const ts::resources::Car_store& ts::resources::Resource_store::car_store() const
{
    return *car_store_;
}

const ts::resources::Track_store& ts::resources::Resource_store::track_store() const
{
    return *track_store_;
}

const ts::resources::Player_store& ts::resources::Resource_store::player_store() const
{
    return *player_store_;
}

const ts::resources::Script_manager& ts::resources::Resource_store::script_manager() const
{
    return *script_manager_;
}

const ts::resources::Settings& ts::resources::Resource_store::settings() const
{
    return *settings_;
}

const ts::resources::Font_library& ts::resources::Resource_store::font_library() const
{
    return *font_library_;
}

const ts::resources::Video_settings& ts::resources::Resource_store::video_settings() const
{
    return settings_->video_settings;
}

ts::resources::Video_settings& ts::resources::Resource_store::video_settings()
{
    return settings_->video_settings;
}

const ts::resources::Car_settings& ts::resources::Resource_store::car_settings() const
{
    return settings_->car_settings;
}

ts::resources::Car_settings& ts::resources::Resource_store::car_settings()
{
    return settings_->car_settings;
}

const ts::resources::Track_settings& ts::resources::Resource_store::track_settings() const
{
    return settings_->track_settings;
}

ts::resources::Track_settings& ts::resources::Resource_store::track_settings()
{
    return settings_->track_settings;
}


const ts::resources::Script_settings& ts::resources::Resource_store::script_settings() const
{
    return settings_->script_settings;
}

ts::resources::Script_settings& ts::resources::Resource_store::script_settings()
{
    return settings_->script_settings;
}

const ts::resources::Input_settings& ts::resources::Resource_store::input_settings() const
{
    return settings_->input_settings;
}

ts::resources::Input_settings& ts::resources::Resource_store::input_settings()
{
    return settings_->input_settings;
}

const ts::resources::Player_settings& ts::resources::Resource_store::player_settings() const
{
    return settings_->player_settings;
}

ts::resources::Player_settings& ts::resources::Resource_store::player_settings()
{
    return settings_->player_settings;
}

const ts::resources::Network_settings& ts::resources::Resource_store::network_settings() const
{
    return settings_->network_settings;
}

ts::resources::Network_settings& ts::resources::Resource_store::network_settings()
{
    return settings_->network_settings;
}

const ts::resources::Audio_settings& ts::resources::Resource_store::audio_settings() const
{
    return settings_->audio_settings;
}
    
ts::resources::Audio_settings& ts::resources::Resource_store::audio_settings()
{
    return settings_->audio_settings;
}