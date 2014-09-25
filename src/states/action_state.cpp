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
#include "graphics/texture.hpp"


ts::states::Action_state::Action_state(game::Loaded_scene loaded_scene, state_machine_type* state_machine, 
                                       gui::Context* context, resources::Resource_store* resource_store)
: gui::State(state_machine, context, resource_store),
  action_scene_(std::move(loaded_scene.action_scene)),
  world_(std::move(loaded_scene.world)),
  control_center_(std::move(loaded_scene.control_center)),
  sound_controller_(std::move(loaded_scene.sound_controller)),
  client_script_interface_(std::move(loaded_scene.client_script_interface)),
  
  control_interface_(resource_store->settings.input_settings.key_mapping)
{
    add_render_scene(&*action_scene_);

    world_->add_world_listener(this);
}

void ts::states::Action_state::on_activate()
{
    world_->launch_game();

    sound_controller_->engine_sounds.start();
}

void ts::states::Action_state::handle_event(const sf::Event& event)
{
    control_interface_.forward_input(event, *control_center_);

    if (event.type == sf::Event::KeyReleased) 
    {
        if (event.key.code == sf::Keyboard::Escape) 
        {
            state_machine()->change_state();
        }
    }
}

void ts::states::Action_state::update(std::size_t frame_duration)
{
    sound_controller_->update(frame_duration);

    action_scene_->update_entities(frame_duration);

    world_->update(frame_duration);
    action_scene_->update(frame_duration);
}

void ts::states::Action_state::on_car_create(world::Car* car)
{
    sound_controller_->engine_sounds.register_car(car);
}

void ts::states::Action_state::on_car_destroy(world::Car* car)
{
    sound_controller_->engine_sounds.unregister_car(car);
}

void ts::states::Action_state::on_collision(const world::Collision_result& collision)
{
    sound_controller_->collision_sounds.play_collision_sound(collision);
}