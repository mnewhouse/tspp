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


#pragma once

#ifndef GUI_ELEMENTS_GRID_LIST_HPP
#define GUI_ELEMENTS_GRID_LIST_HPP

#include "../gui_element.hpp"

#include <vector>

namespace ts
{
    namespace gui
    {
        namespace elements
        {
            class Grid_list_field
                : public Element
            {
            public:
                Grid_list_field(Vector2i size)
                {
                    set_size(size);
                    enable_clipping(true);
                }
            };

            class Grid_list
                : public Element
            {
            public:
                Grid_list(Vector2i field_size, std::int32_t num_columns)
                    : field_size_(field_size),
                    num_columns_(num_columns)
                {
                }

                void clear()
                {
                    for (auto field : fields_)
                    {
                        field->destroy();
                    }

                    fields_.clear();
                }

                template <typename ElementType, typename... Args>
                ElementType* create_field(Args&&... args);

            private:
                Vector2i field_size_;
                std::int32_t num_columns_;

                std::vector<Grid_list_field*> fields_;
            };
        }
    }
}

template <typename ElementType, typename... Args>
ElementType* ts::gui::elements::Grid_list::create_field(Args&&... args)
{
    auto row_id = fields_.size() / num_columns_;
    auto column_id = fields_.size() % num_columns_;

    Vector2i position(field_size_.x * column_id, field_size_.y * row_id);

    auto field = create_child<Grid_list_field>(field_size_);
    field->set_position(position);

    fields_.push_back(field);

    auto result = field->create_child<ElementType>(std::forward<Args>(args)...);
    return result;
}

#endif