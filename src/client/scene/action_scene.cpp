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
#include "view_context.hpp"
#include "particle_generator.hpp"
#include "drawable_entity.hpp"
#include "track_builder.hpp"

#include "resources/car_image_generator.hpp"
#include "resources/settings/video_settings.hpp"

#include "world/car.hpp"

#include "graphics/sprite.hpp"

#include <SFML/Audio/Listener.hpp>

ts::scene::Drawable_entity_definition::Drawable_entity_definition(std::shared_ptr<graphics::Texture> texture_)
: texture(std::move(texture_)),
  texture_rect(0, 0, texture->size().x, texture->size().y),
  scale(1.0, 1.0),
  image_type(resources::Image_type::Default)
{
}

ts::scene::Action_scene::Action_scene(Track_scene track_scene, const resources::Video_settings& video_settings)
: track_scene_(std::move(track_scene)),
  view_context_(std::make_unique<View_context>(video_settings.current_screen_resolution, track_scene_.track_size)),
  particle_generator_(std::make_unique<Particle_generator>()),
  car_image_generator_(std::make_unique<resources::Car_image_generator>())
{
    // Add a view, just in case there won't be any players registered
    auto view = view_context_->add_view();
    view_context_->arrange_evenly();

    enable_split_screen_ = view->camera.fit_in_screen_zoom_level() < video_settings.split_screen_threshold;

    calculate_vertex_bounds();
}

ts::scene::Action_scene::~Action_scene()
{
}

void ts::scene::Action_scene::add_car(const world::Car* car, const Drawable_entity_definition& definition)
{
    Drawable_entity drawable_entity(car, definition.texture, definition.image_type);
    drawable_entity.set_scale(definition.scale);
    drawable_entity.set_texture_rect(definition.texture_rect);
    drawable_entities_.push_back(drawable_entity);

    particle_generator_->add_car(car);
}

void ts::scene::Action_scene::remove_car(const world::Car* car)
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

    particle_generator_->remove_car(car);

    for (auto& view : *view_context_)
    {
        if (view.camera.target() == car)
        {
            view.camera.set_target(nullptr);
        }
    }
}

void ts::scene::Action_scene::reassign_screens()
{
    view_context_->clear();
    for (auto entity : followed_entities_)
    {
        if (auto view = view_context_->add_view())
        {
            view->camera.set_target(entity);
        } 
    }

    view_context_->arrange_evenly();

    dirty_component_cache_ = true;
}

ts::scene::View_context* ts::scene::Action_scene::view_context()
{
    return view_context_.get();
}

void ts::scene::Action_scene::add_followed_entity(const world::Entity* entity)
{
    followed_entities_.push_back(entity);

    if (enable_split_screen_)
    {
        reassign_screens();
    }

    else if (followed_entities_.size() >= 1)
    {
        view_context_->clear();
        auto view = view_context_->add_view();
        view->camera.set_zoom_level(view->camera.fit_in_screen_zoom_level());

        if (followed_entities_.size() == 1)
        {
            view->camera.set_target(followed_entities_.front());
        }
    }
}

void ts::scene::Action_scene::update_cameras(double frame_time)
{
    for (auto& view : *view_context_)
    {
        view.camera.update_view(view.view_port, frame_time);
    }
}

void ts::scene::Action_scene::render_view(std::size_t view_index, sf::RenderTarget& render_target, double frame_time)
{
    const auto& view = view_context_->view(view_index);
    const auto& component_cache = component_cache_[view_index];        

    Vector2<double> screen_size(render_target.getSize().x, render_target.getSize().y);

    render_target.setView(view.camera.view());
    
    auto entity_it = drawable_entities_.begin(), entity_end = drawable_entities_.end();
    auto component_it = component_cache.begin(), component_end = component_cache.end();

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

    particle_generator_->render(render_target, sf::RenderStates::Default);

    render_view_border(view, render_target, 4.0);
}

void ts::scene::Action_scene::render_view_border(const View& view, sf::RenderTarget& render_target, double border_size)
{
    Vector2<double> screen_size(render_target.getSize().x, render_target.getSize().y);
    
    const auto& view_port = view.view_port;

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

void ts::scene::Action_scene::render(sf::RenderTarget& render_target, double frame_time)
{
    update_cameras(frame_time);

    if (dirty_component_cache_)
    {
        update_component_cache();
        dirty_component_cache_ = false;
    }

    for (std::size_t view_index = 0; view_index != view_context_->view_count(); ++view_index)
    {
        render_view(view_index, render_target, frame_time);
    }
}

void ts::scene::Action_scene::update(std::size_t frame_duration)
{
    update_entities(frame_duration);

    sort_entities_by_level();
    update_component_cache();    
}

void ts::scene::Action_scene::update_entities(std::size_t frame_duration)
{
    update_entity_positions();

    particle_generator_->update(frame_duration);
}

void ts::scene::Action_scene::update_entity_positions()
{
    for (auto& drawable_entity : drawable_entities_)
    {
        drawable_entity.update_position();
    }

    for (auto& view : *view_context_)
    {
        view.camera.update_position();

        auto camera_position = view.camera.position();
        sf::Listener::setPosition(static_cast<float>(camera_position.x), static_cast<float>(camera_position.y), 0.0f);
    }
}

void ts::scene::Action_scene::calculate_vertex_bounds()
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

void ts::scene::Action_scene::update_component_cache()
{
    component_cache_.resize(view_context_->view_count());

    for (std::size_t viewport_index = 0; viewport_index != view_context_->view_count(); ++viewport_index)
    {
        const auto& view = view_context_->view(viewport_index);

        auto& component_cache = component_cache_[viewport_index];
        component_cache.resize(track_scene_.components.size());
        auto cache_it = component_cache.begin();

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
                if (view.camera.is_area_visible(area))
                {
                    cached_component.vertices.insert(cached_component.vertices.end(), vertex_it, vertex_it + 4);
                }

                vertex_it += 4;
            }

            ++cache_it, ++component_index;
        }
    }
}


void ts::scene::Action_scene::sort_entities_by_level()
{
    auto level_comparison = [](const Drawable_entity& a, const Drawable_entity& b)
    {
        return a.entity()->z_position() < b.entity()->z_position();
    };

    std::stable_sort(drawable_entities_.begin(), drawable_entities_.end(), level_comparison);
}
