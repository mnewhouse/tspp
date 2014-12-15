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
#include "camera.hpp"

#include "world/entity.hpp"

ts::scene::Camera::Camera(Vector2u screen_size, Vector2u world_size)
: mode_(Mode::Fixed),
  camera_target_(nullptr),
  world_size_(world_size),
  screen_size_(screen_size)
{
    set_zoom_level(1.0);
}

void ts::scene::Camera::set_target(const world::Entity* target, bool follow)
{
    camera_target_ = target;
    follow_target_ = follow;

    update_position();
}

void ts::scene::Camera::follow_target(bool follow)
{
    follow_target_ = follow;
}

void ts::scene::Camera::constrict_to_screen(bool constrict)
{
    constrict_to_screen_ = true;
}

void ts::scene::Camera::set_zoom_level(double zoom_level)
{
    zoom_level_ = zoom_level;
}

void ts::scene::Camera::set_position(Vector2<double> position)
{
    position_ = position;
}

void ts::scene::Camera::set_rotation(Rotation<double> rotation)
{
    rotation_ = rotation;
}
                    

void ts::scene::Camera::set_mode(Mode mode)
{
    mode_ = mode;

    if (mode == Mode::Fixed)
    {
        set_rotation({});
    }
}

ts::scene::Camera::Mode ts::scene::Camera::mode() const
{
    return mode_;
}

ts::Vector2<double> ts::scene::Camera::position() const
{
    return position_;
}

ts::Rotation<double> ts::scene::Camera::rotation() const
{
    return rotation_;
}

double ts::scene::Camera::zoom_level() const
{
    return zoom_level_;
}

const sf::View& ts::scene::Camera::view() const
{
    return view_;
}

const ts::world::Entity* ts::scene::Camera::target() const
{
    return camera_target_;
}

bool ts::scene::Camera::is_target_followed() const
{
    return follow_target_;
}

bool ts::scene::Camera::is_constricted_to_screen() const
{
    return constrict_to_screen_;
}

bool ts::scene::Camera::is_area_visible(const Double_rect& area) const
{
    return intersects(area, visible_area_);
}

double ts::scene::Camera::fit_in_screen_zoom_level() const
{
    return std::min(screen_size_.x / world_size_.x, screen_size_.y / world_size_.y) * 0.5;
}

void ts::scene::Camera::update_view(Double_rect view_port, double frame_time)
{
    if (camera_target_ && follow_target_)
    {
        position_ = camera_target_->position();
    }

    if (mode_ == Mode::Rotational && follow_target_) 
    {
        rotation_ = camera_target_->rotation();
    }

    auto interpolated_position = interpolate_position(last_position_, position_, frame_time);
    
    view_.setRotation(static_cast<float>(rotation().degrees()));
    view_.setCenter(static_cast<float>(interpolated_position.x), static_cast<float>(interpolated_position.y));

    auto inverse_zoom = 0.5 / zoom_level_;
    
    view_.setSize(static_cast<float>((view_port.width * screen_size_.x) * inverse_zoom), 
                  static_cast<float>((view_port.height * screen_size_.y) * inverse_zoom));

    auto left = static_cast<float>(view_port.left);
    auto top = static_cast<float>(view_port.top);
    auto width = static_cast<float>(view_port.width);
    auto height = static_cast<float>(view_port.height);

    view_.setViewport(sf::FloatRect(left, top, width, height));

    if (mode_ == Mode::Fixed && constrict_to_screen_) 
    { 
        constrict_view(); 
    }

    update_visible_area();
}

void ts::scene::Camera::update_visible_area()
{
    const auto& inverse_transform = view_.getInverseTransform();
    const sf::Vector2f corners[] =
    {
        inverse_transform.transformPoint({ -1.0f, 1.0f }),
        inverse_transform.transformPoint({ -1.0f, -1.0f }),
        inverse_transform.transformPoint({ 1.0f, -1.0f }),
        inverse_transform.transformPoint({ 1.0f, 1.0f })
    };

    auto minmax_x = std::minmax_element(std::begin(corners), std::end(corners), 
        [](const sf::Vector2f& a, const sf::Vector2f& b)
    {
        return a.x < b.x;
    });

    auto minmax_y = std::minmax_element(std::begin(corners), std::end(corners),
        [](const sf::Vector2f& a, const sf::Vector2f& b)
    {
        return a.y < b.y;
    });

    visible_area_.left = minmax_x.first->x;
    visible_area_.top = minmax_y.first->y;

    visible_area_.width = minmax_x.second->x - visible_area_.left;
    visible_area_.height = minmax_y.second->y - visible_area_.top;
}

void ts::scene::Camera::constrict_view()
{
    const auto& inverse_transform = view_.getInverseTransform();

    const sf::Vector2f screen_corners[] = 
    {
        inverse_transform.transformPoint(-1.0f, 1.0f),
        inverse_transform.transformPoint(-1.0f, -1.0f),
        inverse_transform.transformPoint(1.0f, -1.0f),
        inverse_transform.transformPoint(1.0f, 1.0f)
    };

    auto clamp_horizontal = [this](float screen_edge, float track_edge)
    {
        auto center = view_.getCenter();
        center.x += track_edge - screen_edge;
        view_.setCenter(center);
    };

    auto clamp_vertical = [this](float screen_edge, float track_edge)
    {
        auto center = view_.getCenter();
        center.y += track_edge - screen_edge;
        view_.setCenter(center);
    };

    const auto& transform = view_.getTransform();

    auto world_width = static_cast<float>(world_size_.x);
    auto world_height = static_cast<float>(world_size_.y);

    auto visible_size = transform.transformPoint(0.0f, 0.0f) -
        transform.transformPoint(world_width, world_height);

    visible_size.x = std::abs(visible_size.x);
    visible_size.y = std::abs(visible_size.y);

    if (visible_size.x <= 2.0f) 
    {
        // Center horizontally
        auto screen_center = inverse_transform.transformPoint(0.0f, 0.0f);
        clamp_horizontal(screen_center.x, world_width * 0.5f);
    }

    else if (screen_corners[0].x < 0.0f) 
    {
        clamp_horizontal(screen_corners[0].x, 0.0f);
    }

    else if (screen_corners[3].x > world_width) 
    {
        clamp_horizontal(screen_corners[3].x, world_width);
    }

    if (visible_size.y <= 2.0f) 
    {
        auto screen_center = inverse_transform.transformPoint(0.0f, 0.0f);
        clamp_vertical(screen_center.y, world_height * 0.5f);
    }

    else if (screen_corners[0].y < 0.0f) 
    {
        clamp_vertical(screen_corners[0].y, 0.0f);
    }

    else if (screen_corners[1].y > world_height) 
    {
        clamp_vertical(screen_corners[1].y, world_height);
    }
}

void ts::scene::Camera::update_position()
{
    last_position_ = position_;
}
