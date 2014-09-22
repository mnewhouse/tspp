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

#ifndef GUI_GRID_LIST_HPP
#define GUI_GRID_LIST_HPP

#include "../stylable_element.hpp"

namespace ts
{
    namespace gui
    {
        struct Grid_list_style
        {
            Vector2<double> cell_size;
        };

        template <typename ElementType>
        class Grid_list
            : public Stylable_element<Grid_list_style>
        {
        public:
            Grid_list(Grid_list_style default_style, std::size_t max_columns);

            template <typename... Args>
            ElementType* create_cell(Args&&... args);

            void delete_cell(Element* cell);
            void clear();

        private:
            Vector2<double> cell_position(std::size_t index, Vector2<double> cell_size) const;
            void update_cells(Vector2<double> cell_size, std::size_t start_index = 0);

            virtual void apply_style(const Grid_list_style& style) override;

            std::size_t max_columns_;
            std::vector<ElementType*> cells_;
        };
    }
}

template <typename ElementType>
ts::gui::Grid_list<ElementType>::Grid_list(Grid_list_style default_style, std::size_t max_columns)
: Stylable_element(default_style), 
  max_columns_(max_columns)
{
}

template <typename ElementType>
template <typename... Args>
ElementType* ts::gui::Grid_list<ElementType>::create_cell(Args&&... args)
{
    auto cell = create_child<ElementType>(std::forward<Args>(args)...);
    cell->set_auto_sizing(false);

    auto& style = current_style();

    auto index = cells_.size();

    const auto& cell_size = style.cell_size;

    cell->set_size(cell_size);
    cell->set_position(cell_position(index, cell_size));

    cells_.push_back(cell);

    dirty_layout();

    return cell;
}

template <typename ElementType>
void ts::gui::Grid_list<ElementType>::delete_cell(Element* cell)
{
    auto found_cell = std::find(cells_.begin(), cells_.end(), element);

    if (found_cell != cells_.end())
    {
        auto it = cells_.erase(found_cell);

        update_cells(it - cells_.begin());
    }
}

template <typename ElementType>
void ts::gui::Grid_list<ElementType>::clear()
{
    cells_.clear();

    clear_children();
}

template <typename ElementType>
ts::Vector2<double> ts::gui::Grid_list<ElementType>::cell_position(std::size_t index, Vector2<double> cell_size) const
{
    auto row_index = index / max_columns_;
    auto column_index = index % max_columns_;

    auto position = cell_size;
    position.x *= column_index;
    position.y *= row_index;

    return position;
}

template <typename ElementType>
void ts::gui::Grid_list<ElementType>::update_cells(Vector2<double> cell_size, std::size_t start_index)
{
    auto index = start_index;

    auto position = cell_position(index, cell_size);

    auto row_index = index / max_columns_;

    for (auto cell_it = cells_.begin() + index; cell_it != cells_.end(); ++cell_it, ++row_index)
    {
        if (row_index == max_columns_)
        {
            row_index = 0;
            position.x = 0.0;
            position.y += cell_size.y;
        }
        
        auto cell = *cell_it;
        cell->set_position(position);
        cell->set_size(cell_size);

        position.x += cell_size.x;
    }
}

template <typename ElementType>
void ts::gui::Grid_list<ElementType>::apply_style(const Grid_list_style& style)
{
    update_cells(style.cell_size);
}



#endif