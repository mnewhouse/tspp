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
#include "action_scene.hpp"

#include "world/car.hpp"
#include "resources/player_color.hpp"

#include "graphics/sprite.hpp"

ts::game::Action_scene::Action_scene(Track_scene_ track_scene)
: track_scene_(std::move(track_scene))
{
    screens_.emplace_back(track_scene_.track_size, Double_rect(0.0, 0.0, 1.0, 1.0));

    calculate_vertex_bounds();
}

void ts::game::Action_scene::add_car(const world::Car* car, const resources::Player_color& car_color)
{
    const auto& car_definition = car->car_definition();
    auto car_image = car_image_generator_(car_definition, car_color);

    auto texture = std::make_shared<graphics::Texture>(car_image, car_definition.image_rect);

    drawable_entities_.emplace_back(car, texture, car_definition.image_type);
    drawable_entities_.back().set_scale({ car_definition.image_scale, car_definition.image_scale });

    particle_generator_.add_car(car);
}

void ts::game::Action_scene::remove_car(const world::Car* car)
{
    auto drawable_it = std::find_if(drawable_entities_.begin(), drawable_entities_.end(), 
        [car](const Drawable_entity& drawable_entity)
    {
        return drawable_entity.entity() == car;
    });

    if (drawable_it != drawable_entities_.end())
    {
        drawable_entities_.erase(drawable_it);
    }

    particle_generator_.remove_car(car);

    for (auto& screen : screens_)
    {
        if (screen.camera.target() == car)
        {
            screen.camera.set_target(nullptr);
        }
    }
}

void ts::game::Action_scene::reassign_screens()
{
    auto screen_count = 1;
    if (enable_split_screen_)
    {
        screen_count = followed_entities_.size() - std::count(followed_entities_.begin(), followed_entities_.end(), nullptr);
        screen_count = std::max(screen_count, 1);
    }

    screens_.clear();
    if (screen_count == 4)
    {
        screens_.emplace_back(track_scene_.track_size, Double_rect(0.0, 0.0, 0.5, 0.5));
        screens_.emplace_back(track_scene_.track_size, Double_rect(0.5, 0.0, 0.5, 0.5));
        screens_.emplace_back(track_scene_.track_size, Double_rect(0.0, 0.5, 0.5, 0.5));
        screens_.emplace_back(track_scene_.track_size, Double_rect(0.5, 0.5, 0.5, 0.5));
    }

    else if (screen_count == 3)
    {
        const auto one_third = (1.0/3.0);
        screens_.emplace_back(track_scene_.track_size, Double_rect(0.0, 0.0, one_third, 1.0));
        screens_.emplace_back(track_scene_.track_size, Double_rect(one_third, 0.0, one_third, 1.0));
        screens_.emplace_back(track_scene_.track_size, Double_rect(one_third * 2.0, 0.0, one_third, 1.0));
    }

    else if (screen_count == 2)
    {
        screens_.emplace_back(track_scene_.track_size, Double_rect(0.0, 0.0, 0.5, 1.0));
        screens_.emplace_back(track_scene_.track_size, Double_rect(0.5, 0.0, 0.5, 1.0));
    }

    else
    {
        screens_.emplace_back(track_scene_.track_size, Double_rect(0.0, 0.0, 1.0, 1.0));
    }

    auto screen_id = 0;
    for (auto entity : followed_entities_)
    {
        if (entity)
        {
            auto& camera = screens_[screen_id++].camera;
            camera.set_target(entity);
        }
    }
}

void ts::game::Action_scene::zoom_in()
{
    for (auto& screen : screens_)
    {
        auto& camera = screen.camera;

        camera.set_zoom_level(camera.zoom_level() * 1.05);
    }
}

void ts::game::Action_scene::zoom_out()
{
    for (auto& screen : screens_)
    {
        auto& camera = screen.camera;

        camera.set_zoom_level(camera.zoom_level() / 1.05);
    }
}

void ts::game::Action_scene::set_followed_entity(const world::Entity* entity, controls::Slot slot)
{
    slot = std::min(slot, 3);

    if (slot >= followed_entities_.size())
    {
        followed_entities_.resize(slot + 1);
    }

    followed_entities_[slot] = entity;

    if (enable_split_screen_)
    {
        reassign_screens();
    }
}

void ts::game::Action_scene::render_screen(Screen& screen, sf::RenderTarget& render_target, double frame_time)
{
    Vector2<double> screen_size(render_target.getSize().x, render_target.getSize().y);

    screen.camera.update_view(screen.view_port, screen_size, frame_time);
    render_target.setView(screen.camera.view());
    
    auto entity_it = drawable_entities_.begin(), entity_end = drawable_entities_.end();
    auto component_it = screen.component_cache.begin(), component_end = screen.component_cache.end();

    for (std::uint32_t current_level = 0; component_it != component_end; ++component_it)
    {
        if (current_level != component_it->level)
        {
            current_level = component_it->level;

            // Draw all the entities below the component's level
            for (; entity_it != entity_end && entity_it->entity()->z_level() < current_level; ++entity_it)
            {
                entity_it->draw(render_target, sf::RenderStates::Default, frame_time);
            }
        }

       sf::RenderStates render_states;
       render_states.texture = component_it->texture;
       render_target.draw(component_it->vertices.data(), component_it->vertices.size(), sf::Quads, render_states);
    }

    for (; entity_it != entity_end; ++entity_it)
    {
        entity_it->draw(render_target, sf::RenderStates::Default, frame_time);
    }

    particle_generator_.render(render_target, sf::RenderStates::Default);

    render_screen_border(screen, render_target, 4.0);
}

void ts::game::Action_scene::render_screen_border(Screen& screen, sf::RenderTarget& render_target, double border_size)
{
    Vector2<double> screen_size(render_target.getSize().x, render_target.getSize().y);
    
    const auto& view_port = screen.view_port;

    auto left = view_port.left * screen_size.x;
    auto top = view_port.top * screen_size.y;
    auto right = left + view_port.width * screen_size.x;
    auto bottom = top + view_port.height * screen_size.y;

    auto border_left = left - border_size;
    auto border_top = top - border_size;
    auto border_right = right + border_size;
    auto border_bottom = bottom + border_size;

    Vector2<double> vertex_positions[16] =
    {
        { border_left, border_top },
        { border_left, top },
        { border_right, top },
        { border_right, border_top },

        { border_left, border_top },
        { border_left, border_bottom },
        { left, border_bottom },
        { left, border_top },

        { border_left, bottom },
        { border_left, border_bottom },
        { border_right, border_bottom },
        { border_right, bottom },

        { right, border_top },
        { right, border_bottom },
        { border_right, border_bottom },
        { border_right, border_top }
    };

    sf::Vertex vertices[16];
    auto vertex_it = std::begin(vertices);
    for (const auto& position : vertex_positions)
    {
        vertex_it->position.x = static_cast<float>(position.x);
        vertex_it->position.y = static_cast<float>(position.y);
        vertex_it->color = sf::Color::Black;

        ++vertex_it;
    }
    
    render_target.setView(render_target.getDefaultView());
    render_target.draw(vertices, 16, sf::Quads);
}

void ts::game::Action_scene::render(sf::RenderTarget& render_target, double frame_time)
{
    for (auto& screen : screens_)
    {
        render_screen(screen, render_target, frame_time);
    }
}

void ts::game::Action_scene::update(std::size_t frame_duration)
{
    update_entity_positions();
    update_component_cache();
    sort_entities_by_level();

    particle_generator_.update(frame_duration);
}

void ts::game::Action_scene::update_entity_positions()
{
    for (auto& drawable_entity : drawable_entities_)
    {
        drawable_entity.update_position();
    }

    for (auto& screen : screens_)
    {
        screen.camera.update_position();
    }
}

void ts::game::Action_scene::calculate_vertex_bounds()
{
    vertex_bounds_.resize(track_scene_.components.size());
    auto out_it = vertex_bounds_.begin();

    for (const auto& component : track_scene_.components)
    {
        const auto& vertices = component.vertices;

        out_it->reserve(vertices.size() / 4);

        for (auto vertex_it = vertices.begin(); vertex_it < vertices.end(); vertex_it += 4)
        {
            const std::initializer_list<double> x_list = 
            { 
                vertex_it[0].position.x, vertex_it[1].position.x, vertex_it[2].position.x, vertex_it[3].position.x
            };

            const std::initializer_list<double> y_list =
            {
                vertex_it[0].position.y, vertex_it[1].position.y, vertex_it[2].position.y, vertex_it[3].position.y
            };

            auto x_range = std::minmax(x_list);
            auto y_range = std::minmax(y_list);

            auto left = x_range.first - 20.0;
            auto top = y_range.first - 20.0;
            auto right = x_range.second + 20.0;
            auto bottom = y_range.second + 20.0;

            out_it->emplace_back(left, top, right - left, bottom - top);
        }

        ++out_it;
    }
}

void ts::game::Action_scene::update_component_cache()
{
    for (auto& screen : screens_)
    {
        screen.component_cache.resize(track_scene_.components.size());
        auto cache_it = screen.component_cache.begin();

        std::size_t component_index = 0;
        for (const auto& component : track_scene_.components)
        {
            auto& cached_component = *cache_it;
            cached_component.vertices.clear();
            cached_component.texture = component.texture;
            cached_component.level = component.level;
            cached_component.bounding_box = component.bounding_box;

            const auto& vertex_bounds = vertex_bounds_[component_index];
            auto vertex_it = component.vertices.begin();

            for (auto area : vertex_bounds)
            {
                if (screen.camera.is_area_visible(area))
                {
                    cached_component.vertices.insert(cached_component.vertices.end(), vertex_it, vertex_it + 4);
                }

                vertex_it += 4;
            }

            ++cache_it, ++component_index;
        }
    }
}


void ts::game::Action_scene::sort_entities_by_level()
{
    auto level_comparison = [](const Drawable_entity& a, const Drawable_entity& b)
    {
        return a.entity()->z_position() < b.entity()->z_position();
    };

    std::stable_sort(drawable_entities_.begin(), drawable_entities_.end(), level_comparison);
}

ts::game::Action_scene::Screen::Screen(Vector2u track_size, Double_rect view_port_)
: view_port(view_port_),
  camera(track_size)
{
}