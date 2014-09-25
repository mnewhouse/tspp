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
#include "cup_state.hpp"
#include "action_state.hpp"

#include "resources/track.hpp"


#include "gui_definitions/window_template.hpp"
#include "gui_definitions/background_document.hpp"

#include "user_interface/document.hpp"
#include "user_interface/elements/elements.hpp"

namespace ts
{
    namespace states
    {
        ts::utf8_string to_string(game::Cup_state cup_state);
        ts::utf8_string to_string(game::Loading_phase loading_phase);
    }
}


ts::states::Cup_state::Cup_state(game::Cup_type cup_type, state_machine_type* state_machine, gui::Context* context,
                      resources::Resource_store* resource_store)
    : gui::State(state_machine, context, resource_store),
      cup_(cup_type, resource_store),
      cup_gui_(&cup_, context, resource_store)
{
    cup_gui_.show();

    cup_.add_cup_listener(this);

    if (cup_type != game::Cup_type::Remote)
    {
        add_selected_local_players();

        if (cup_type != game::Cup_type::Server)
        {
            // Skip the registration state
            cup_.advance();
        }
    }
}

void ts::states::Cup_state::add_selected_local_players()
{
    auto& selected_players = resource_store()->settings.player_settings.selected_players;
    auto& player_store = resource_store()->players;

    for (auto slot = 0; slot != selected_players.size(); ++slot)
    {
        auto unique_id = selected_players[slot];

        if (auto player_handle = player_store.get_player_by_id(unique_id))
        {
            game::Player player;
            player.nickname = player_handle->name;
            player.id = player_handle->id;
            player.color = player_handle->color;

            cup_.add_local_player(player, slot);
        }
    }
}

void ts::states::Cup_state::on_state_change(game::Cup_state old_state, game::Cup_state new_state)
{
    if (new_state == game::Cup_state::Action)
    {
        launch_action();
    }

    else if (new_state == game::Cup_state::Initializing)
    {
        action_loader_.async_load(cup_.make_stage_data(), *resource_store());

        cup_gui_.progress_dialog()->show();
        cup_gui_.progress_dialog()->set_loading_state(to_string(action_loader_.phase()));
        cup_gui_.progress_dialog()->set_progress(action_loader_.progress());
        cup_gui_.progress_dialog()->set_max_progress(1.0);
    }

    else if (new_state == game::Cup_state::Car_selection)
    {
        cup_gui_.show_car_selection_dialog();
    }

    cup_gui_.set_cup_state_text(to_string(new_state));
}

void ts::states::Cup_state::on_restart()
{
}

void ts::states::Cup_state::on_end()
{
    return_to_main_menu();
}

void ts::states::Cup_state::update(std::size_t frame_duration)
{
    cup_gui_.update(frame_duration);

    if (action_loader_.is_loading())
    {
        cup_gui_.progress_dialog()->set_progress(action_loader_.progress());

        if (loading_phase_ != action_loader_.phase())
        {
            loading_phase_ = action_loader_.phase();

            cup_gui_.progress_dialog()->set_loading_state(to_string(loading_phase_));

            if (action_loader_.complete())
            {
                completion_time_ = std::chrono::high_resolution_clock::now();
            }
        }

        if (action_loader_.complete())
        {
            auto now = std::chrono::high_resolution_clock::now();
            if (now - completion_time_ >= std::chrono::milliseconds(200))
            {
                cup_.signal_ready();
            }
        }
    }
}

void ts::states::Cup_state::return_to_main_menu()
{
    state_machine()->change_state();
}


void ts::states::Cup_state::on_activate()
{
}

void ts::states::Cup_state::launch_action()
{
    context()->hide_all();

    auto action_state = std::make_unique<Action_state>(action_loader_.transfer_loaded_scene(),
        state_machine(), context(), resource_store());

    state_machine()->change_state(std::move(action_state));
}


ts::utf8_string ts::states::to_string(game::Cup_state cup_state)
{
    using State = game::Cup_state;

    switch (cup_state)
    {
    case State::Action:
        return "Action";
    case State::Cup:
        return "Cup";
    case State::Registering:
        return "Registering";
    case State::Car_selection:
        return "Awaiting car selection";
    case State::Initializing:
        return "Initializing";
    case State::End:
        return "Cup over";
    default:
        return "";
    }
}

ts::utf8_string ts::states::to_string(game::Loading_phase phase)
{
    using game::Loading_phase;

    switch (phase)
    {
    case Loading_phase::Initializing:
        return "Initializing...";

    case Loading_phase::Preprocessing:
        return "Preprocessing...";

    case Loading_phase::Building_pattern:
        return "Building track pattern...";

    case Loading_phase::Loading_track_textures:
        return "Loading track textures...";

    case Loading_phase::Building_track_scene:
        return "Building track scene...";

    case Loading_phase::Creating_world:
        return "Creating world...";

    case Loading_phase::Creating_entities:
        return "Creating entities...";

    case Loading_phase::Loading_audio:
        return "Loading audio...";

    case Loading_phase::Complete:
        return "Complete!";

    default:
        return "";
    }
}