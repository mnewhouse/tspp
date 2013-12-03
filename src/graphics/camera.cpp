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

#include "camera.hpp"
#include "world/entity.hpp"

#include "core/rotation.hpp"

ts::graphics::Camera::Camera(Vector2u world_size)
: mode_(Mode::Fixed),
  camera_target_(nullptr),
  last_position_(),
  world_size_(world_size)
{
}

void ts::graphics::Camera::set_target(const world::Entity* target)
{
    camera_target_ = target;
    last_position_ = target->position();
}

void ts::graphics::Camera::update_view(graphics::Render_target& render_target, double frame_time)
{
    update_transform(frame_time);

    auto screen_size = render_target.getSize();

    auto screen_width = float(screen_size.x);
    auto screen_height = float(screen_size.y);

    auto scale = getScale();

    sf::View view;
    view.setCenter(getPosition());
    if (mode_ == Mode::Rotational) {
        view.setRotation(getRotation());
    }
    
    view.setSize(screen_width / scale.x, screen_height / scale.y);

    if (mode_ == Mode::Fixed) clamp_view(view);

    render_target.setView(view);
}

void ts::graphics::Camera::clamp_view(sf::View& view)
{
    const auto& inverse_transform = view.getInverseTransform();

    sf::Vector2f screen_corners[] = {
        inverse_transform.transformPoint(-1.0f, 1.0f),
        inverse_transform.transformPoint(-1.0f, -1.0f),
        inverse_transform.transformPoint(1.0f, -1.0f),
        inverse_transform.transformPoint(1.0f, 1.0f)
    };

    auto clamp_horizontal = [&](float screen_edge, float track_edge)
    {
        auto center = view.getCenter();
        center.x += track_edge - screen_edge;
        view.setCenter(center);
    };

    auto clamp_vertical = [&](float screen_edge, float track_edge)
    {
        auto center = view.getCenter();
        center.y += track_edge - screen_edge;
        view.setCenter(center);
    };

    const auto& transform = view.getTransform();

    auto visible_size = transform.transformPoint(0.0f, 0.0f) -
        transform.transformPoint(world_size_.x, world_size_.y);

    visible_size.x = std::abs(visible_size.x);
    visible_size.y = std::abs(visible_size.y);

    if (visible_size.x <= 2.0f) {
        // Center horizontally
        auto screen_center = inverse_transform.transformPoint(0.0f, 0.0f);
        clamp_horizontal(screen_center.x, world_size_.x * 0.5f);
    }

    else if (screen_corners[0].x < 0.0f) {
        clamp_horizontal(screen_corners[0].x, 0.0f);
    }

    else if (screen_corners[3].x > world_size_.x) {
        clamp_horizontal(screen_corners[3].x, world_size_.x);
    }

    if (visible_size.y <= 2.0f) {
        auto screen_center = inverse_transform.transformPoint(0.0f, 0.0f);
        clamp_vertical(screen_center.y, world_size_.y * 0.5f);
    }

    else if (screen_corners[0].y < 0.0f) {
        clamp_vertical(screen_corners[0].y, 0.0f);
    }

    else if (screen_corners[1].y > world_size_.y) {
        clamp_vertical(screen_corners[1].y, world_size_.y);
    }
}

void ts::graphics::Camera::update_position()
{
    last_position_ = camera_target_->position();
}

void ts::graphics::Camera::update_transform(double frame_time)
{
    if (camera_target_) {
        // Show the whole track
        auto position = last_position_ + (camera_target_->position() - last_position_) * frame_time;

        setPosition(float(position.x), float(position.y));

        auto rotation = camera_target_->rotation();
        setRotation(float(rotation.degrees()));
    }
}