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
#include "action_state_base.hpp"
#include "cup_gui.hpp"

#include "cup/cup.hpp"

#include "game/loading_sequence.hpp"

namespace ts
{
    namespace states
    {
        ts::utf8_string to_string(cup::Cup_state cup_state);
    }
}


ts::states::Cup_state_base::Cup_state_base(const client::Client_interface* client_interface, state_machine_type* state_machine, gui::Context* context,
                                 resources::Resource_store* resource_store)
  : gui::State(state_machine, context, resource_store),
    client_interface_(client_interface),
    cup_gui_(std::make_unique<Cup_GUI>(client_interface, context, resource_store)),
    loading_sequence_(std::make_unique<game::Loading_sequence>(resource_store))    
{
    cup_gui_->set_cup_state_text(to_string(client_interface->cup()->cup_state()));
}

ts::states::Cup_state_base::~Cup_state_base()
{
}

void ts::states::Cup_state_base::show_gui()
{
    cup_gui_->show();
}

void ts::states::Cup_state_base::on_activate()
{
    show_gui();
}

void ts::states::Cup_state_base::update(std::size_t frame_duration)
{
    cup_gui_->update(frame_duration);

    if (stage_loader_)
    {
        cup_gui_->set_loading_progress(stage_loader_->progress());
        cup_gui_->set_loading_progress_text(to_string(stage_loader_->state()));
    }

    else
    {
        loading_sequence_->poll();
        
        cup_gui_->set_loading_progress(loading_sequence_->progress());
        cup_gui_->set_loading_progress_text(loading_sequence_->progress_string());
    }

    if (client_interface_->cup()->cup_state() == cup::Cup_state::Action)
    {
        if (loading_sequence_->is_complete())
        {
            try
            {
                launch_action(make_action_state(loading_sequence_->transfer_result()));
            }

            catch (const std::exception& error)
            {
                client_interface_->load_error(error.what());
            }            
        }
    }

    if (cup_gui_->quit_event_pending())
    {
        state_machine()->change_state();
    }
}

void ts::states::Cup_state_base::on_state_change(cup::Cup_state old_state, cup::Cup_state new_state)
{
    cup_gui_->set_cup_state_text(to_string(new_state));
}

void ts::states::Cup_state_base::show_stage_loading(const game::Stage_loader* stage_loader)
{
    stage_loader_ = stage_loader;

    cup_gui_->show_progress_dialog();
    cup_gui_->set_loading_progress(stage_loader->progress());
    cup_gui_->set_loading_progress_text(to_string(stage_loader->state()));
}

void ts::states::Cup_state_base::load_scene(const action::Stage* stage)
{
    stage_loader_ = nullptr;

    loading_sequence_->set_completion_handler([this]()
    {
        cup_gui_->hide_progress_dialog();
        client_interface_->signal_ready();
    });

    loading_sequence_->set_state_change_handler([this](const utf8_string& state)
    {
        cup_gui_->set_loading_progress_text(state);
    });

    loading_sequence_->async_load(stage);
}


void ts::states::Cup_state_base::return_to_main_menu()
{
    state_machine()->change_state();
}

void ts::states::Cup_state_base::launch_action(std::unique_ptr<Action_state_base> action_state)
{
    context()->hide_all();

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
