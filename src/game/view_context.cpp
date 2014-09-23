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
#include "view_context.hpp"

ts::game::View::View(Vector2u world_size)
: camera(world_size)
{
}

ts::game::View_context::View_context(Vector2u screen_size, Vector2u world_size)
: screen_size_(screen_size),
  world_size_(world_size)
{
}

ts::Vector2u ts::game::View_context::screen_size() const
{
    return screen_size_;
}

ts::game::View* ts::game::View_context::add_view()
{
    if (view_count() < max_views)
    {
        views_.emplace_back(world_size_);
        return &views_.back();
    }

    return nullptr;
}

std::size_t ts::game::View_context::view_count() const
{
    return views_.size();
}

void ts::game::View_context::clear()
{
    views_.clear();
}

const ts::game::View& ts::game::View_context::view(std::size_t view_index) const
{
    return views_[view_index];
}

ts::game::View& ts::game::View_context::view(std::size_t view_index)
{
    return views_[view_index];
}

const ts::game::View* ts::game::View_context::begin() const
{
    return views_.data();
}

ts::game::View* ts::game::View_context::begin()
{
    return views_.data();
}

const ts::game::View* ts::game::View_context::end() const
{
    return views_.data() + views_.size();
}

ts::game::View* ts::game::View_context::end()
{
    return views_.data() + views_.size();
}

void ts::game::View_context::arrange_evenly()
{
    double ratio = static_cast<double>(screen_size_.x) / screen_size_.y;

    double num_columns_temp = std::ceil(std::sqrt(view_count() * ratio));

    std::size_t num_columns = static_cast<std::size_t>(num_columns_temp);
    std::size_t num_rows = static_cast<std::size_t>(std::ceil(view_count() / num_columns_temp));

    std::size_t missing_cells = (num_rows * num_columns) - view_count();
    
    auto row_columns = num_columns - 1;

    double row_height = 1.0 / num_rows;
    double cell_width = 1.0 / row_columns;

    for (std::size_t row = 0, index = 0; row != num_rows; ++row)
    {
        if (row == missing_cells)
        {
            ++row_columns;
            cell_width = 1.0 / row_columns;
        }

        auto top = row * row_height;
        for (std::size_t column = 0; column != row_columns; ++column, ++index)
        {
            auto& view = views_[index];
            view.view_port.left = column * cell_width;
            view.view_port.top = top;
            view.view_port.width = cell_width;
            view.view_port.height = row_height;
        }
    }
}