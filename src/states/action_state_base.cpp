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

#include "action/stage.hpp"

#include "game/action_scene.hpp"
#include "game/chatbox_display.hpp"

#include "controls/control_interface.hpp"
#include "controls/control_event_translator.hpp"

#include "audio/sound_controller.hpp"

#include "resources/settings/input_settings.hpp"

ts::states::Action_state_base::Action_state_base(game::Loaded_scene loaded_scene, std::unique_ptr<game::Chatbox_display> chatbox_display,
                                                 controls::Control_interface* control_interface, state_machine_type* state_machine, 
                                                 gui::Context* context, resources::Resource_store* resource_store)
: gui::State(state_machine, context, resource_store),
  scene_(std::move(loaded_scene)),
  control_event_translator_(std::make_unique<controls::Event_translator>(resource_store->input_settings().key_mapping)),
  control_interface_(control_interface),
  chatbox_display_(std::move(chatbox_display))
{
    add_render_scene(&*scene_.action_scene);
}

ts::states::Action_state_base::~Action_state_base()
{
}

void ts::states::Action_state_base::on_activate()
{
    scene_.sound_controller->start();

    chatbox_display_->show_chatbox();
}

void ts::states::Action_state_base::handle_event(const sf::Event& event)
{
    
    if (chatbox_display_->chatbox_input_active())
    {
        if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Return)
        {
            chatbox_display_->send_chat_message();
        }
    }

    else
    {
        // SFML event in       
        control_event_translator_->enqueue_event(event);

        // Control events out
        controls::Control_event control_event;
        while (control_event_translator_->poll_event(control_event))
        {
            control_interface_->handle_event(control_event);
        }
     
        if (event.type == sf::Event::KeyReleased)
        {
            if (event.key.code == sf::Keyboard::Escape)
            {
                state_machine()->change_state();
            }

            else if (event.key.code == sf::Keyboard::T)
            {
                chatbox_display_->toggle_chatbox_input(true);
            }
            
        }
    }
}

void ts::states::Action_state_base::update(std::size_t frame_duration)
{
    control_interface_->update(frame_duration);
    scene_.sound_controller->update(frame_duration);

    scene_.action_scene->update_entities(frame_duration);
    scene_.action_scene->update(frame_duration);

    chatbox_display_->update();
}

void ts::states::Action_state_base::on_state_change(cup::Cup_state old_state, cup::Cup_state new_state)
{
    if (old_state == cup::Cup_state::Action)
    {
        // Pop this state from the state stack
        state_machine()->change_state();
    }
}

void ts::states::Action_state_base::on_collision(world::Entity* subject, world::Entity* object, const world::Collision_info& collision_info)
{
    scene_.sound_controller->collision_sounds.play_collision_sound(subject, object, collision_info);
}