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

#include "client/client_interface.hpp"

#include "cup/cup.hpp"

#include "resources/loading_interface.hpp"

namespace ts
{
    namespace states
    {
        ts::utf8_string to_string(cup::Cup_state cup_state);
    }
}


ts::states::Cup_state_base::Cup_state_base(const client::Client_interface* client_interface,
    state_machine_type* state_machine, gui::Context* context, resources::Resource_store* resource_store)
  : gui::State(state_machine, context, resource_store),
    cup::Scoped_cup_listener(client_interface->cup()),

    client_interface_(client_interface),
    cup_gui_(std::make_unique<Cup_GUI>(client_interface, context, resource_store))
{
    cup_gui_->set_cup_state_text(to_string(client_interface->cup()->cup_state()));
}

ts::states::Cup_state_base::~Cup_state_base()
{
}

void ts::states::Cup_state_base::show_gui()
{
    cup_gui_->show();

    context()->set_mouse_cursor_visible(true);
}

void ts::states::Cup_state_base::on_activate()
{
    show_gui();
}

void ts::states::Cup_state_base::update(std::size_t frame_duration)
{
    cup_gui_->update(frame_duration);

    if (loading_interface_)
    {
        cup_gui_->set_loading_progress(loading_interface_->progress() / loading_interface_->max_progress());
        cup_gui_->set_loading_progress_text(loading_interface_->progress_string());
    }

    else if (client_interface_->cup()->cup_state() == cup::Cup_state::Action)
    {
        // No loading interface anymore and cup state equals Action - let's go!
        launch_action(make_action_state());
    }
}

void ts::states::Cup_state_base::on_state_change(cup::Cup_state old_state, cup::Cup_state new_state)
{
    cup_gui_->set_cup_state_text(to_string(new_state));

    if (new_state == cup::Cup_state::Initializing)
    {
        cup_gui_->hide_progress_dialog();
        loading_interface_ = nullptr;
    }
}

void ts::states::Cup_state_base::show_stage_loading(const resources::Loading_interface* loading_interface)
{
    loading_interface_ = loading_interface;

    cup_gui_->show_progress_dialog();
    cup_gui_->set_loading_progress(loading_interface->progress());
    cup_gui_->set_loading_progress_text(loading_interface->progress_string());
}

void ts::states::Cup_state_base::ready_for_action()
{
    loading_interface_ = nullptr;
    cup_gui_->hide_progress_dialog();
}

/*
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
*/


void ts::states::Cup_state_base::return_to_main_menu()
{
    state_machine()->change_state();
}

void ts::states::Cup_state_base::launch_action(std::unique_ptr<Action_state_base> action_state)
{
    context()->hide_all();
    context()->set_mouse_cursor_visible(false);

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
