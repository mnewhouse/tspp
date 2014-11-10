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
#include "loading_sequence.hpp"

#include "resources/resource_store.hpp"
#include "resources/track_handle.hpp"

#include "action/stage.hpp"

ts::game::Loading_sequence::Loading_sequence(const resources::Resource_store* resource_store)
: resource_store_(resource_store)
{
}

void ts::game::Loading_sequence::poll()
{
    scene_loader_.poll();
    audio_loader_.poll();
    script_loader_.poll();
}

void ts::game::Loading_sequence::async_load(const action::Stage* stage)
{
    loaded_scene_ = Loaded_scene();
    exception_ptr_ = nullptr;
    stage_ = stage;

    scene_loader_.set_completion_handler([this]()
    {
        load_scripts_if_ready();
    });

    scene_loader_.set_state_change_handler([this](Scene_loader_state new_state)
    {
        state_change(to_string(new_state));
    });

    audio_loader_.set_completion_handler([this]()
    {
        load_scripts_if_ready();
    });

    scene_loader_.async_load(stage, resource_store_->video_settings());
    audio_loader_.async_load(stage, resource_store_->audio_settings());
}

bool ts::game::Loading_sequence::is_complete() const
{
    return loaded_scene_.action_scene && loaded_scene_.sound_controller && loaded_scene_.script_interface;
}

bool ts::game::Loading_sequence::is_loading() const
{
    return is_complete() || scene_loader_.is_loading() || audio_loader_.is_loading() || script_loader_.is_loading();
}

void ts::game::Loading_sequence::load_scripts_if_ready()
{
    if (scene_loader_.is_completed() && audio_loader_.is_completed())
    {
        try
        {
            loaded_scene_.action_scene = scene_loader_.transfer_result();
            loaded_scene_.sound_controller = audio_loader_.transfer_result();

            script_loader_.set_completion_handler([this]()
            {
                handle_completion();
            });

            script_loader_.set_state_change_handler([this](Script_loader_state new_state)
            {
                state_change(to_string(new_state));
            });

            script_loader_.async_load(stage_->stage_data().script_resources, stage_, loaded_scene_.action_scene.get());
        }

        catch (...)
        {
            exception_ptr_ = std::current_exception();
        }
    }
}

void ts::game::Loading_sequence::handle_completion()
{
    if (completion_handler_)
    {
        try
        {
            loaded_scene_.script_interface = script_loader_.transfer_result();
        }

        catch (...)
        {
            exception_ptr_ = std::current_exception();
        }

        auto completion_handler = std::move(completion_handler_);
        completion_handler();
    }
}

void ts::game::Loading_sequence::state_change(const utf8_string& state)
{
    if (state_change_handler_)
    {
        state_change_handler_(state);
    }
}

ts::game::Loaded_scene ts::game::Loading_sequence::transfer_result()
{
    if (exception_ptr_)
    {
        std::rethrow_exception(exception_ptr_);
    }

    return std::move(loaded_scene_);
}

double ts::game::Loading_sequence::progress() const
{
    return scene_loader_.progress();
}

ts::utf8_string ts::game::Loading_sequence::progress_string() const
{
    return to_string(scene_loader_.state());
}