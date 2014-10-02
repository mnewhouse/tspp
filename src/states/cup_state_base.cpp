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
#include "cup_state_base.hpp"
#include "action_state.hpp"

#include "gui_definitions/window_template.hpp"
#include "gui_definitions/background_document.hpp"
#include "gui_definitions/cup_gui.hpp"

#include "user_interface/document.hpp"
#include "user_interface/elements/elements.hpp"

#include "cup/cup_interface.hpp"

namespace ts
{
    namespace states
    {
        ts::utf8_string to_string(cup::Cup_state cup_state);
        ts::utf8_string to_string(game::Loading_phase loading_phase);
    }
}


ts::states::Cup_state_base::Cup_state_base(cup::Cup_interface* cup_interface, state_machine_type* state_machine, gui::Context* context,
                                 resources::Resource_store* resource_store)
  : gui::State(state_machine, context, resource_store),
    cup_interface_(cup_interface),
    cup_gui_(cup_interface, context, resource_store)
{
}

ts::states::Cup_state_base::~Cup_state_base()
{
}

void ts::states::Cup_state_base::show_gui()
{
    cup_gui_.show();
}

void ts::states::Cup_state_base::on_activate()
{
    show_gui();
}

void ts::states::Cup_state_base::on_state_change(cup::Cup_state old_state, cup::Cup_state new_state)
{
    if (new_state == cup::Cup_state::Action)
    {
        launch_action();
    }

    if (new_state == cup::Cup_state::Initializing)
    {
        action_loader_.async_load(cup_interface_->cup()->make_stage_data(), *resource_store());

        auto progress_dialog = cup_gui_.progress_dialog();
        progress_dialog->show();
        progress_dialog->set_loading_state(to_string(action_loader_.phase()));
        progress_dialog->set_progress(action_loader_.progress());
        progress_dialog->set_max_progress(1.0);
    }

    else if (new_state == cup::Cup_state::Car_selection)
    {
        cup_gui_.show_car_selection_dialog();
    }

    cup_gui_.set_cup_state_text(to_string(new_state));
}

void ts::states::Cup_state_base::on_restart()
{
}

void ts::states::Cup_state_base::on_end()
{
    return_to_main_menu();
}

void ts::states::Cup_state_base::update(std::size_t frame_duration)
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
                cup_gui_.progress_dialog()->hide();
                cup_interface_->signal_ready();
            }
        }
    }
}

void ts::states::Cup_state_base::return_to_main_menu()
{
    state_machine()->change_state();
}

void ts::states::Cup_state_base::launch_action()
{
    context()->hide_all();

    // TODO: Delayed launching, for when loading isn't finished yet
    auto action_state = create_action_state(action_loader_.transfer_loaded_scene());

    state_machine()->change_state(std::move(action_state));
}

ts::utf8_string ts::states::to_string(cup::Cup_state cup_state)
{
    using State = cup::Cup_state;

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