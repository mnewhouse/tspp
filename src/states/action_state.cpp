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
#include "action_state.hpp"
#include "action/stage.hpp"

#include "messages/message_center.hpp"

#include "game/action_scene.hpp"

#include "controls/control_center.hpp"
#include "controls/control_interface.hpp"

#include "audio/sound_controller.hpp"

#include "script/interfaces/client_interface.hpp"

#include "resources/settings/input_settings.hpp"

ts::states::Action_state_base::Action_state_base(game::Loaded_scene loaded_scene, state_machine_type* state_machine, 
                                                 gui::Context* context, resources::Resource_store* resource_store)
: gui::State(state_machine, context, resource_store),
  scene_(std::move(loaded_scene)),
  
  control_interface_(std::make_unique<controls::Control_interface>(resource_store->input_settings().key_mapping))
{
    add_render_scene(&*scene_.action_scene);
}

ts::states::Action_state_base::~Action_state_base()
{
}

void ts::states::Action_state_base::on_activate()
{
    scene_.stage->launch_game();

    scene_.sound_controller->engine_sounds.start();
}

void ts::states::Action_state_base::handle_event(const sf::Event& event)
{
    control_interface_->forward_input(event, scene_.stage->control_center());

    if (event.type == sf::Event::KeyReleased) 
    {
        if (event.key.code == sf::Keyboard::Escape) 
        {
            state_machine()->change_state();
        }
    }
}

void ts::states::Action_state_base::update(std::size_t frame_duration)
{
    scene_.sound_controller->update(frame_duration);

    scene_.action_scene->update_entities(frame_duration);

    scene_.stage->update(frame_duration);
    scene_.action_scene->update(frame_duration);
}

void ts::states::Action_state_base::on_car_create(world::Car* car)
{
    scene_.sound_controller->engine_sounds.register_car(car);
}

void ts::states::Action_state_base::on_car_destroy(world::Car* car)
{
    scene_.sound_controller->engine_sounds.unregister_car(car);
}

void ts::states::Action_state_base::on_collision(const world::Collision_result& collision)
{
    scene_.sound_controller->collision_sounds.play_collision_sound(collision);
}
