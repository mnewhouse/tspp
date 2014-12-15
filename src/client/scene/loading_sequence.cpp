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
#include "action/stage_interface.hpp"

ts::scene::Loading_sequence::Loading_sequence(const resources::Resource_store* resource_store, script::Engine* engine)
: resource_store_(resource_store),
  script_loader_(engine)
{
}

void ts::scene::Loading_sequence::poll()
{
    if (stage_loader_)
    {
        set_progress(stage_loader_->progress());
        set_max_progress(stage_loader_->max_progress());
    }

    else if (script_loader_.loading_interface())
    {
        set_progress(script_loader_.loading_interface()->progress());
        set_max_progress(script_loader_.loading_interface()->max_progress());
    }

    scene_loader_.poll();
    audio_loader_.poll();
    script_loader_.poll();
}

void ts::scene::Loading_sequence::async_load(const cup::Stage_data& stage_data, action::Stage_interface* stage_interface)
{
    set_loading(true);
    set_finished(false);

    auto stage_loader_callback = [this](const action::Stage* stage)
    {
        async_load(stage);

        stage_loader_ = nullptr;
    };

    stage_loader_ = stage_interface->async_load_stage(stage_data, stage_loader_callback);
}

void ts::scene::Loading_sequence::async_load(const action::Stage* stage)
{
    loaded_scene_ = Scene();
    exception_ptr_ = nullptr;
    stage_ = stage;

    set_finished(false);
    set_loading(true);

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

void ts::scene::Loading_sequence::load_scripts_if_ready()
{
    if (scene_loader_.finished() && audio_loader_.finished())
    {
        try
        {
            loaded_scene_.action_scene = scene_loader_.transfer_result();
            loaded_scene_.sound_controller = audio_loader_.transfer_result();

            auto callback = [this]()
            {
                handle_completion();
            };

            script_loader_.async_load_scripts(stage_->stage_data().script_resources, callback);
        }

        catch (...)
        {
            exception_ptr_ = std::current_exception();            
        }
    }
}

void ts::scene::Loading_sequence::handle_completion()
{
    if (completion_handler_)
    {
        set_finished(true);

        try
        {
            loaded_scene_.script_conductor = script_loader_.transfer_result();
        }

        catch (...)
        {
            exception_ptr_ = std::current_exception();
        }

        auto completion_handler = std::move(completion_handler_);
        completion_handler();
    }
}

void ts::scene::Loading_sequence::state_change(const utf8_string& state)
{
    if (state_change_handler_)
    {
        state_change_handler_(state);
    }
}

ts::scene::Scene ts::scene::Loading_sequence::transfer_result()
{
    set_loading(false);

    if (exception_ptr_)
    {
        std::rethrow_exception(exception_ptr_);
    }

    return std::move(loaded_scene_);
}

ts::utf8_string ts::scene::Loading_sequence::progress_string() const
{
    if (stage_loader_)
    {
        return stage_loader_->progress_string();
    }

    if (script_loader_.loading_interface())
    {
        return script_loader_.loading_interface()->progress_string();
    }

    return to_string(scene_loader_.state());
}