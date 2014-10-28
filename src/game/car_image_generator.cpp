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
#include "car_image_generator.hpp"

#include "resources/car_definition.hpp"
#include "resources/player_color.hpp"

namespace ts
{
    namespace game
    {
        namespace impl
        {
            struct Color_map
            {
                Color_map(Vector2i frame_size_);

                std::vector<sf::Color> pixels;
                Vector2i frame_size;
            };

            sf::Color colorize(sf::Color source_color, sf::Color target_color);

            Color_map make_color_map(Vector2i frame_size, Double_rect car_bounds, const resources::Player_color& player_color);
            Double_rect find_image_bounds(const sf::Image& car_image, Int_rect rect);

            void apply_base_component(Color_map& color_map, Double_rect car_bounds, const resources::Color_base& base);
            void apply_extra_component(Color_map& color_map, Double_rect car_bounds, const resources::Color_extra& component);

            void apply_horizontal_gradient(Color_map& color_map, Double_rect area, sf::Color primary, sf::Color secondary);
            void apply_vertical_gradient(Color_map& color_map, Double_rect area, sf::Color primary, sf::Color secondary);

            sf::Color interpolate_color(sf::Color primary, sf::Color secondary, double value);

            void fill_with_color(Color_map& color_map, Double_rect area, sf::Color color);
            sf::Color blend_alpha(sf::Color source_color, sf::Color dest_color);

            Int_rect calculate_absolute_area(Double_rect area, Vector2i frame_size);
        }
    }
}

sf::Image ts::game::Car_image_generator::operator()(const resources::Car_definition& car_definition, const resources::Player_color& color_scheme, Frame_mode frame_mode) const
{
    const auto& source_image = image_loader_.load_from_file(car_definition.image_file);
    const auto& image_rect = car_definition.image_rect;

    Vector2i frame_size = { image_rect.width, image_rect.height };
    std::size_t frame_count = 1;

    if (car_definition.image_type == resources::Image_type::Rotational)
    {
        frame_count = frame_size.x / frame_size.y;
        frame_size.x = frame_size.y;
    }

    if (frame_mode == Single_frame)
    {
        frame_count = 1;
    }

    Int_rect frame_bounds(image_rect.left, image_rect.top, frame_size.x, frame_size.y);
    auto car_bounds = impl::find_image_bounds(source_image, frame_bounds);

    auto color_map = impl::make_color_map(frame_size, car_bounds, color_scheme);

    Vector2i center = frame_size / 2;

    sf::Image dest_image;
    dest_image.create(frame_count * frame_size.x, frame_size.y, sf::Color::Transparent);

    for (std::size_t frame_index = 0; frame_index != frame_count; ++frame_index)
    {
        auto frame_rotation = Rotation<double>::degrees(frame_index * (360.0 / frame_count));

        auto sin = std::sin(frame_rotation.radians());
        auto cos = std::cos(frame_rotation.radians());

        auto image_y = image_rect.top;
        
        for (std::int32_t y = 0, image_y = image_rect.top; y != frame_size.y; ++y, ++image_y)
        {
            auto image_x = image_rect.left + frame_index * frame_size.x;

            for (std::int32_t x = 0; x != frame_size.x; ++x, ++image_x)
            {
                auto source_color = source_image.getPixel(image_x, image_y);
                if (source_color.a == 0) continue;

                Vector2i point(x, y);
                point -= center;

                auto source_point = transform_point(point, -sin, cos);

                auto source_x = static_cast<std::int32_t>(source_point.x + center.x);
                auto source_y = static_cast<std::int32_t>(source_point.y + center.y);

                if (source_x >= 0 && source_x < frame_size.x && source_y >= 0 && source_y < frame_size.y)
                {
                    auto target_color = color_map.pixels[source_y * frame_size.x + source_x];

                    auto new_color = impl::colorize(source_color, target_color);
                    dest_image.setPixel(image_x, image_y, new_color);
                }
            }
        }
    }   

    return dest_image;
}

sf::Color ts::game::impl::colorize(sf::Color source_color, sf::Color target_color)
{
    if (source_color.a == 0) return source_color;

    auto min_value = std::min(source_color.r, std::min(source_color.g, source_color.b));
    auto max_value = std::max(source_color.r, std::max(source_color.g, source_color.b));

    auto red_amount = std::max(source_color.r - std::abs(source_color.g - source_color.b), 0) / static_cast<double>(max_value);

    target_color.r = static_cast<sf::Uint8>(std::round(source_color.r + (target_color.r - source_color.r) * red_amount));
    target_color.g = static_cast<sf::Uint8>(std::round(source_color.g + (target_color.g - source_color.g) * red_amount));
    target_color.b = static_cast<sf::Uint8>(std::round(source_color.b + (target_color.b - source_color.b) * red_amount));
    target_color.a = source_color.a;

    target_color.r = clamp(target_color.r, min_value, max_value);
    target_color.g = clamp(target_color.g, min_value, max_value);
    target_color.b = clamp(target_color.b, min_value, max_value);

    return target_color;
};

ts::game::impl::Color_map::Color_map(Vector2i frame_size_)
: pixels(frame_size_.x * frame_size_.y),
  frame_size(frame_size_)
{}

ts::game::impl::Color_map ts::game::impl::make_color_map(Vector2i frame_size, Double_rect car_bounds, const resources::Player_color& player_color)
{
    Color_map color_map(frame_size);

    apply_base_component(color_map, car_bounds, player_color.base_color);
    apply_extra_component(color_map, car_bounds, player_color.primary_extra);
    apply_extra_component(color_map, car_bounds, player_color.secondary_extra);

    return color_map;
}

ts::Int_rect ts::game::impl::calculate_absolute_area(Double_rect area, Vector2i frame_size)
{
    area.left *= frame_size.x;
    area.top *= frame_size.y;

    area.width *= frame_size.x;
    area.height *= frame_size.y;

    auto right = area.right();
    auto bottom = area.bottom();

    area.left = std::round(area.left);
    area.top = std::round(area.top);

    area.width = std::round(right - area.left);
    area.height = std::round(bottom - area.top);

    Int_rect result(static_cast<std::int32_t>(area.left), static_cast<std::int32_t>(area.top), 
                    static_cast<std::int32_t>(area.width), static_cast<std::int32_t>(area.height));
    
    if (result.right() < frame_size.x) ++result.width;
    if (result.bottom() < frame_size.y) ++result.height;

    return result;
}

void ts::game::impl::apply_base_component(Color_map& color_map, Double_rect car_bounds, const resources::Color_base& base)
{
    using resources::Color_base;

    auto& pixels = color_map.pixels;
    auto frame_size = color_map.frame_size;

    if (base.type == Color_base::Plain)
    {
        std::fill(pixels.begin(), pixels.end(), base.primary_color);
    }

    else if (base.type == Color_base::Horizontal_gradient)
    {
        fill_with_color(color_map, { 0.0, 0.0, car_bounds.width, 1.0 }, base.primary_color);
        apply_horizontal_gradient(color_map, { car_bounds.left, 0.0, car_bounds.width, 1.0 }, base.primary_color, base.secondary_color);
        fill_with_color(color_map, { car_bounds.right(), 0.0, 1.0 - car_bounds.right(), 1.0 }, base.secondary_color);
    }

    else if (base.type == Color_base::Vertical_gradient)
    {
        fill_with_color(color_map, { 0.0, 0.0, 1.0, car_bounds.height }, base.primary_color);
        apply_vertical_gradient(color_map, { 0.0, car_bounds.top, 1.0, car_bounds.height }, base.primary_color, base.secondary_color);
        fill_with_color(color_map, { 0.0, car_bounds.bottom(), 1.0, 1.0 - car_bounds.bottom() }, base.secondary_color);
    }
}

sf::Color ts::game::impl::blend_alpha(sf::Color source, sf::Color dest)
{
    if (source.a == 0) return dest;
    if (source.a == 255) return source;

    dest.r += ((source.r - dest.r) * source.a + 0xFF) >> 8;
    dest.g += ((source.g - dest.g) * source.a + 0xFF) >> 8;
    dest.b += ((source.b - dest.b) * source.a + 0xFF) >> 8;
    dest.a += ((source.a - dest.a) * source.a + 0xFF) >> 8;

    return dest;
}

void ts::game::impl::apply_extra_component(Color_map& color_map, Double_rect car_bounds, const resources::Color_extra& component)
{
    using resources::Color_extra;

    auto primary_color = component.primary_color;
    auto secondary_color = component.secondary_color;

    switch (component.type)
    {
    case Color_extra::Vertical_stripe:
        fill_with_color(color_map, { car_bounds.left + 0.35 * car_bounds.width, 0.0, car_bounds.width * 0.3, 1.0 }, primary_color);
        break;

    case Color_extra::Double_stripe:
        fill_with_color(color_map, { car_bounds.left + 0.30 * car_bounds.width, 0.0, car_bounds.width * 0.15, 1.0 }, primary_color);
        fill_with_color(color_map, { car_bounds.left + 0.55 * car_bounds.width, 0.0, car_bounds.width * 0.15, 1.0 }, secondary_color);
        break;

    case Color_extra::Left_half:
        fill_with_color(color_map, { 0.0, 0.0, car_bounds.left + 0.5 * car_bounds.width, 1.0 }, primary_color);
        break;
        
    case Color_extra::Right_half:
    {
        auto left = car_bounds.left + 0.5 * car_bounds.width;
        fill_with_color(color_map, { left, 0.0, 1.0 - left, 1.0 }, primary_color);
        break;
    }

    case Color_extra::Front_half:
        fill_with_color(color_map, { 0.0, 0.0, 1.0, car_bounds.top + car_bounds.height * 0.5 }, primary_color);
        break;

    case Color_extra::Rear_half:
    {
        auto top = car_bounds.top + 0.5 * car_bounds.height;
        fill_with_color(color_map, { 0.0, top, 1.0, 1.0 - top }, primary_color);
        break;
    }
        
    case Color_extra::Gradient_stripe:
    {
        auto bottom = car_bounds.bottom();
        auto left = car_bounds.left + car_bounds.width * 0.35;
        auto width = car_bounds.width * 0.3;

        fill_with_color(color_map, { left, 0.0, width, car_bounds.top }, primary_color);
        apply_vertical_gradient(color_map, { left, car_bounds.top, width, car_bounds.height }, primary_color, secondary_color);
        fill_with_color(color_map, { left, bottom, width, 1.0 - bottom }, secondary_color);
        break;
    }

    case Color_extra::Reflected_gradient:
    {
        auto halfway = car_bounds.top + car_bounds.width * 0.5;
        apply_vertical_gradient(color_map, { 0.0, 0.0, 1.0, halfway }, sf::Color::Transparent, primary_color);
        apply_vertical_gradient(color_map, { 0.0, halfway, 1.0, car_bounds.bottom() }, primary_color, sf::Color::Transparent);
        break;
    }

    case Color_extra::Front_stripe:
        fill_with_color(color_map, { 0.0, 0.0, 1.0, car_bounds.top + car_bounds.height * 0.3 }, primary_color);
        break;

    case Color_extra::Rear_stripe:
    {
        auto top = car_bounds.top + car_bounds.height * 0.7;
        fill_with_color(color_map, { 0.0, top, 1.0, 1.0 - top }, primary_color);
        break;
    }

    case Color_extra::Horizontal_stripe:
    {
        auto top = car_bounds.top + car_bounds.height * 0.3;
        auto height = car_bounds.height * 0.4;
        fill_with_color(color_map, { 0.0, top, 1.0, height }, primary_color);
        break;
    }

    default:
        break;

    }
}

sf::Color ts::game::impl::interpolate_color(sf::Color first, sf::Color second, double value)
{
    first.r += static_cast<sf::Uint8>((second.r - first.r) * value);
    first.g += static_cast<sf::Uint8>((second.g - first.g) * value);
    first.b += static_cast<sf::Uint8>((second.b - first.b) * value);
    first.a += static_cast<sf::Uint8>((second.a - first.a) * value);
    return first;
}

void ts::game::impl::fill_with_color(Color_map& color_map, Double_rect area, sf::Color color)
{
    auto& pixels = color_map.pixels;
    auto frame_size = color_map.frame_size;

    auto absolute_area = calculate_absolute_area(area, frame_size);

    auto right = absolute_area.right(), bottom = absolute_area.bottom();

    for (auto y = absolute_area.top; y != bottom; ++y)
    {
        auto ptr = &pixels[y * frame_size.x + absolute_area.left], end = ptr + absolute_area.width;

        while (ptr != end)
        {
            *ptr++ = color;
        }
    }
}

ts::Double_rect ts::game::impl::find_image_bounds(const sf::Image& image, Int_rect rect)
{
    std::int32_t min_x = rect.right();
    std::int32_t max_x = rect.top - 1;
    std::int32_t min_y = rect.bottom();
    std::int32_t max_y = rect.left - 1;

    auto image_size = image.getSize();
    
    auto right = rect.right(), bottom = rect.bottom();

    for (auto y = rect.top; y != bottom; ++y)
    {
        auto ptr = image.getPixelsPtr() + (image_size.x * y + rect.left) * 4, end = ptr + rect.width * 4;
        for (auto x = rect.left; ptr != end; ptr += 4, ++x)
        {
            if (auto alpha = ptr[3])
            {
                if (x < min_x) min_x = x;
                if (x > max_x) max_x = x;

                if (y < min_y) min_y = y;
                if (y > max_y) max_y = y;
            }
        }
    }

    Double_rect result;
    if (min_x > max_x || min_y > max_y) return result;

    result.left = static_cast<double>(min_x - rect.left) / rect.width;
    result.top = static_cast<double>(min_y - rect.top) / rect.height;
    result.width = static_cast<double>(max_x - min_x) / rect.width;
    result.height = static_cast<double>(max_y - min_y) / rect.width;

    return result;
}

void ts::game::impl::apply_horizontal_gradient(Color_map& color_map, Double_rect area, sf::Color primary, sf::Color secondary)
{
    auto& pixels = color_map.pixels;
    auto frame_size = color_map.frame_size;

    auto absolute_area = calculate_absolute_area(area, frame_size);

    double inverted_width = 1.0 / absolute_area.width;
    double offset = 0.0;

    auto right = absolute_area.right(), bottom = absolute_area.bottom();
    for (auto x = absolute_area.left; x != right; ++x, offset += 1.0)
    {
        auto color = interpolate_color(primary, secondary, offset * inverted_width);
        auto pixel_ptr = &pixels[absolute_area.top * frame_size.x + x];

        for (auto y = absolute_area.top; y != bottom; ++y, pixel_ptr += frame_size.x)
        {
            *pixel_ptr = blend_alpha(color, *pixel_ptr);
        }
    }
}


void ts::game::impl::apply_vertical_gradient(Color_map& color_map, Double_rect area, sf::Color primary, sf::Color secondary)
{
    auto& pixels = color_map.pixels;
    auto frame_size = color_map.frame_size;

    auto absolute_area = calculate_absolute_area(area, frame_size);

    double inverted_height = 1.0 / absolute_area.height;
    double offset = 0.0;

    auto right = absolute_area.right(), bottom = absolute_area.bottom();
    for (auto y = absolute_area.top; y != bottom; ++y, offset += 1.0)
    {
        auto color = interpolate_color(primary, secondary, offset * inverted_height);
        auto pixel_ptr = &pixels[y * frame_size.x + absolute_area.left];

        for (auto x = absolute_area.left; x != right; ++x, ++pixel_ptr)
        {
            *pixel_ptr = blend_alpha(color, *pixel_ptr);
        }
    }
}