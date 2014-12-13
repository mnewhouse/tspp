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
#include "action_state_base.hpp"

#include "client/client_interface.hpp"
#include "cup/cup.hpp"

#include "game/event_dispatcher.hpp"

#include "controls/control_interface.hpp"
#include "controls/control_event_processor.hpp"

#include "client/scene/scene.hpp"
#include "client/scene/sound_controller.hpp"
#include "client/scene/action_scene.hpp"
#include "client/hud/hud_chatbox.hpp"

#include "resources/settings/input_settings.hpp"

struct ts::states::Action_state_base::Members
{
    Members(std::unique_ptr<controls::Control_interface> control_interface, const client::Client_interface* client_interface,
        const resources::Resource_store* resource_store, gui::Context* context)
        : event_dispatcher_(),
          control_event_processor_(std::move(control_interface), resource_store->input_settings().key_mapping),
          chatbox_(resource_store->input_settings(), client_interface, context)
    {
    }

    game::Event_dispatcher event_dispatcher_;
    controls::Event_processor control_event_processor_;
    hud::Chatbox chatbox_;
    scene::Scene scene_;
};

ts::states::Action_state_base::Action_state_base(std::unique_ptr<controls::Control_interface> control_interface, const client::Client_interface* client_interface,
    state_machine_type* state_machine, gui::Context* context, resources::Resource_store* resource_store)
: gui::State(state_machine, context, resource_store),
  members_(std::make_unique<Members>(std::move(control_interface), client_interface, resource_store, context)),
  cup::Scoped_cup_listener(client_interface->cup())
{
}

ts::states::Action_state_base::~Action_state_base()
{

}

void ts::states::Action_state_base::handle_event(const sf::Event& event)
{
    auto& event_dispatcher = members_->event_dispatcher_;
    event_dispatcher.handle_event(event);

    if (!event_dispatcher.hogged())
    {
        if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape)
        {
            state_machine()->change_state();
        }
    }
}

void ts::states::Action_state_base::on_state_change(cup::Cup_state old_state, cup::Cup_state new_state)
{
    if (old_state == cup::Cup_state::Action && new_state != old_state)
    {
        // Pop this state from the state stack
        state_machine()->change_state();
    }
}

void ts::states::Action_state_base::on_activate()
{
    auto event_dispatcher = &members_->event_dispatcher_;

    members_->chatbox_.start_listening(event_dispatcher);
    members_->control_event_processor_.start_listening(event_dispatcher);

    members_->chatbox_.show();

    members_->scene_ = acquire_scene();
    members_->scene_.start();

    add_render_scene(members_->scene_.action_scene.get());
}

void ts::states::Action_state_base::update(std::size_t frame_duration)
{
    members_->scene_.update(frame_duration);
    members_->chatbox_.update(frame_duration);    
}