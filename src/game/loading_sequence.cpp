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

ts::game::Loading_sequence::Loading_sequence(const resources::Resource_store* resource_store)
: resource_store_(resource_store)
{
}

void ts::game::Loading_sequence::poll()
{
    stage_loader_.poll();
    action_loader_.poll();
    audio_loader_.poll();
}

void ts::game::Loading_sequence::async_load(const action::Stage_data& stage_data)
{
    loaded_scene_ = Loaded_scene();
    stage_data_ = stage_data;

    stage_loader_.set_completion_handler([this]()
    {
        loaded_scene_.stage = stage_loader_.transfer_result();
        load_scene();
    });

    stage_loader_.set_state_change_handler([this](Stage_loader_state new_state)
    {
        state_change(to_string(new_state));
    });

    stage_loader_.async_load(stage_data);
}

void ts::game::Loading_sequence::load_scene()
{
    action_loader_.set_completion_handler([this]()
    {
        loaded_scene_.action_scene = action_loader_.transfer_result();
        test_readiness();
    });

    action_loader_.set_state_change_handler([this](Action_loader_state new_state)
    {
        state_change(to_string(new_state));
    });

    audio_loader_.set_completion_handler([this]()
    {
        loaded_scene_.sound_controller = audio_loader_.transfer_result();
        test_readiness();
    });

    const auto* stage = loaded_scene_.stage.get();
    action_loader_.async_load(stage, resource_store_->video_settings());
    audio_loader_.async_load(stage, resource_store_->audio_settings());
}

void ts::game::Loading_sequence::test_readiness()
{
    if (loaded_scene_.sound_controller && loaded_scene_.stage && loaded_scene_.action_scene)
    {
        if (completion_handler_)
        {
            completion_handler_();
        }
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
    return std::move(loaded_scene_);
}

double ts::game::Loading_sequence::progress() const
{
    if (stage_loader_.is_loading())
    {
        return stage_loader_.progress();
    }

    return action_loader_.progress();
}

ts::utf8_string ts::game::Loading_sequence::progress_string() const
{
    if (stage_loader_.is_loading())
    {
        return to_string(stage_loader_.state());
    }

    return to_string(action_loader_.state());
}