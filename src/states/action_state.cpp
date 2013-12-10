/*
 * Turbo Sliders++
 * Copyright (C) 2013 Martin Newhouse
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

#include "action_state.hpp"
#include "core/config.hpp"
#include "world/car.hpp"
#include "cup/stage_data.hpp"
#include "graphics/drawable_entity.hpp"
#include "resources/track.hpp"
#include "graphics/texture.hpp"

#include <boost/filesystem.hpp>
#include <iostream>

ts::states::Action_scene::Action_scene(const resources::Track& track, const cup::Stage_data& stage_data)
: track_components_(graphics::build_track(track)), frame_duration_(0.0f), camera_(track.size())
{
}

void ts::states::Action_scene::zoom_in()
{
    auto scale = camera_.getScale();
    camera_.setScale(scale * 1.05f);
}

void ts::states::Action_scene::zoom_out()
{
    auto scale = camera_.getScale();
    camera_.setScale(scale / 1.05f);
}

void ts::states::Action_scene::on_car_create(const world::Car& car)
{
    const auto& car_definition = car.car_definition();
    boost::filesystem::path image_path = config::car_directory;
    image_path /= car_definition.image_file;

    sf::Image image;
    image.loadFromFile(image_path.string());

    graphics::Texture texture(image, car_definition.image_rect);
    drawable_entities_.emplace_back(&car, Handle<graphics::Texture>(std::move(texture)), car_definition.image_type);

    Vector2f scale(car_definition.image_scale, car_definition.image_scale);
    drawable_entities_.back().set_scale(scale);

    camera_.set_target(&car);
}

void ts::states::Action_scene::on_entity_destroy(const world::Entity& entity)
{
    auto it = std::remove_if(drawable_entities_.begin(), drawable_entities_.end(), 
        [&](const graphics::Drawable_entity& object)
        {
            return object.entity() == &entity;
        }
    );

    drawable_entities_.erase(it, drawable_entities_.end());
}

void ts::states::Action_scene::update(std::size_t frame_duration)
{
    frame_time_.restart();
    frame_duration_ = frame_duration * 0.001f;

    for (auto& drawable_entity : drawable_entities_)
    {
        drawable_entity.update_position();
    }

    using graphics::Drawable_entity;

    // Sort drawable_entities. This will usually be O(n) because there's not much
    // shifting in height levels.
    std::sort(drawable_entities_.begin(), drawable_entities_.end(), 
              [](const Drawable_entity& a, const Drawable_entity& b)
    {
        return a.entity()->z_position() < b.entity()->z_position();
    });

    camera_.update_position();
}

void ts::states::Action_scene::render(graphics::Render_target& render_target)
{
    auto frame_time = std::min(frame_time_.getElapsedTime().asSeconds() / frame_duration_, 1.0);

    camera_.update_view(render_target, frame_time);
    sf::RenderStates render_states;

    const auto& transform = render_target.getView().getTransform();
    auto transformed_bbox = [transform](const Float_rect& bbox)
    {
        return transform.transformRect(sf::FloatRect(bbox.left, bbox.top, bbox.width, bbox.height));
    };

    const sf::FloatRect screen_rect(-1.0f, -1.0f, 2.0f, 2.0f);

    auto entity_it = drawable_entities_.begin(), entity_end = drawable_entities_.end();

    for (const auto& component : track_components_.components)
    {
        if (!transformed_bbox(component.bounding_box).intersects(screen_rect)) continue;

        for (; entity_it != entity_end && int(entity_it->entity()->z_position()) < component.level; ++entity_it)
        {
            entity_it->draw(render_target, render_states, frame_time);
        }

        render_states.texture = component.texture;
        render_target.draw(&component.vertices[0], component.vertices.size(), sf::Quads, render_states);
    }

    for (; entity_it != entity_end; ++entity_it) {
        entity_it->draw(render_target, render_states, frame_time);
    }

    auto default_view = render_target.getDefaultView();
    default_view.setSize(float(render_target.getSize().x), float(render_target.getSize().y));
    render_target.setView(default_view);
}

ts::states::Action_state::Action_state(resources::Track&& track, const cup::Stage_data& stage_data, 
                                       const Handle<state_machine_type>& state_machine, const Handle<gui::Context>& context)
    : gui::State(state_machine, context), 
      action_scene_(track, stage_data), 
      world_(std::move(track)),
      key_mapping_(controls::default_key_mapping())
{
    world_.add_entity_listener(&action_scene_);

    for (const auto& player : stage_data.players) {
        auto car = world_.create_car(player.car);

        if (player.control_slot) {
            control_center_.assume_control(player.control_slot, car);
        }

        car = world_.create_car(player.car);
        car->set_position({100.0, 100.0});
        car->set_rotation(Rotation<double>::degrees(180));
    }


}

void ts::states::Action_state::handle_event(const sf::Event& event)
{
    auto update_state = [&](bool state) {
        auto range = key_mapping_.equal_range(event.key.code);
        for (auto it = range.first; it != range.second; ++it) {
            const auto& key_bind = it->second;

            control_center_.set_control_state(key_bind.slot, key_bind.control, state);
        }
    };

    if (event.type == sf::Event::KeyPressed) {
        update_state(true);
    }

    else if (event.type == sf::Event::KeyReleased) {
        update_state(false);

        if (event.key.code == sf::Keyboard::Escape) {
            state_machine()->change_state();
        }        

        else if (event.key.code == sf::Keyboard::Add) {
            action_scene_.zoom_in();
        }

        else if (event.key.code == sf::Keyboard::Subtract) {
            action_scene_.zoom_out();
        }
    }
}

void ts::states::Action_state::render(graphics::Render_target& render_target)
{
    render_target.clear();
    action_scene_.render_if_visible(render_target);
}

void ts::states::Action_state::update(std::size_t frame_duration)
{
    action_scene_.update(frame_duration);
    world_.update(frame_duration);
}