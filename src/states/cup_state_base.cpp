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
#include "cup_gui.hpp"

#include "gui_definitions/window_template.hpp"
#include "gui_definitions/background_document.hpp"

#include "user_interface/document.hpp"
#include "user_interface/elements/elements.hpp"

#include "cup/cup.hpp"
#include "cup/cup_interface.hpp"

namespace ts
{
    namespace states
    {
        ts::utf8_string to_string(cup::Cup_state cup_state);
    }
}


ts::states::Cup_state_base::Cup_state_base(cup::Cup_interface* cup_interface, state_machine_type* state_machine, gui::Context* context,
                                 resources::Resource_store* resource_store)
  : Chatbox_listener(cup_interface->chatbox()),
    gui::State(state_machine, context, resource_store),
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
    cup_gui_.set_cup_state_text(to_string(new_state));
}

void ts::states::Cup_state_base::on_restart()
{
}

void ts::states::Cup_state_base::on_end()
{
    return_to_main_menu();
}

void ts::states::Cup_state_base::on_chat_message(const cup::Composite_message& message)
{
    cup_gui_.output_chat_message(message);
}

void ts::states::Cup_state_base::update(std::size_t frame_duration)
{
    cup_gui_.update(frame_duration);
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

void ts::states::Cup_state_base::begin_loading()
{
    cup_gui_.show_progress_dialog();
}

void ts::states::Cup_state_base::finish_loading()
{
    cup_gui_.hide_progress_dialog();
}

void ts::states::Cup_state_base::set_loading_progress(double progress)
{
    cup_gui_.set_loading_progress(progress);
}

void ts::states::Cup_state_base::set_loading_progress_text(const utf8_string& text)
{
    cup_gui_.set_loading_progress_text(text);
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
