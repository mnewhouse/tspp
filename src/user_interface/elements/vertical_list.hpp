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

#ifndef GUI_VERTICAL_LIST_HPP
#define GUI_VERTICAL_LIST_HPP

#include "../stylable_element.hpp"

namespace ts
{
    namespace gui
    {
        struct Vertical_list_style
        {
            Vector2<double> row_size;
            double row_spacing = 0.0;

            Background_style item_style;
            Background_style alternate_style;
        };

        template <typename ItemType>
        class Vertical_list
            : public Stylable_element<Vertical_list_style>
        {
        public:
            Vertical_list(const Vertical_list_style& default_style);

            template <typename... Args>
            ItemType* create_row(Args&&... args);

            void delete_row(Element* row);
            void delete_row(std::size_t index);

            std::size_t row_count() const;

            ItemType* get_row(std::size_t index) const;

            void clear();

        private:
            virtual void apply_style(const Vertical_list_style& style) override;

            void apply_style_impl(const Vertical_list_style& style);
            
            std::vector<ItemType*> rows_;
        };
    }
}

template <typename ItemType>
ts::gui::Vertical_list<ItemType>::Vertical_list(const Vertical_list_style& style)
: Stylable_element(style)
{
    apply_style_impl(style);
}

template <typename ItemType>
template <typename... Args>
ItemType* ts::gui::Vertical_list<ItemType>::create_row(Args&&... args)
{
    auto child = create_child<ItemType>(std::forward<Args>(args)...);

    auto& style = current_style();
    child->set_auto_sizing(false);
    child->set_size(style.row_size);
    
    auto num_rows = rows_.size();
    Vector2<double> position;
    position.y = num_rows * (style.row_size.y + style.row_spacing);

    if (style.row_size.y == 0.0)
    {
        child->set_auto_sizing(true);
        if (num_rows != 0)
        {
            auto prev_row = rows_.back();
            position.y = prev_row->position().y + prev_row->size().y + style.row_spacing;
        }
    }

    child->set_position(position);

    auto& item_style = num_rows & 1 ? style.alternate_style : style.item_style;
    child->set_background_style(item_style);

    rows_.push_back(child);

    dirty_layout();

    return child;
}

template <typename ItemType>
void ts::gui::Vertical_list<ItemType>::delete_row(std::size_t index)
{
    auto row = rows_[index];
    auto it = rows_.erase(rows_.begin() + index);

    Vector2<double> position;

    auto& style = current_style();

    position.y = index * (style.row_size.y + style.row_spacing);
    if (index != 0 && style.row_size.y == 0.0)
    {
        auto prev_row = rows_[index - 1];
        position.y = prev_row->position().y + prev_row->size().y + style.row_spacing;
    }


    for (; it != rows_.end(); ++it, ++index)
    {
        Element* row_elem = *it;

        row_elem->set_position(position);
        row_elem->set_background_style(index & 1 ? style.alternate_style : style.item_style);

        position.y += style.row_size.y + style.row_spacing;
    }

    row->destroy();
}

template <typename ItemType>
void ts::gui::Vertical_list<ItemType>::delete_row(Element* row)
{
    auto it = std::find(rows_.begin(), rows_.end(), row);
    if (it != rows_.end())
    {
        delete_row(it - rows_.begin());
    }
}

template <typename ItemType>
ItemType* ts::gui::Vertical_list<ItemType>::get_row(std::size_t index) const
{
    if (index >= rows_.size()) return nullptr;

    return rows_[index];
}

template <typename ItemType>
void ts::gui::Vertical_list<ItemType>::clear()
{
    rows_.clear();
    
    clear_children();
}

template <typename ItemType>
std::size_t ts::gui::Vertical_list<ItemType>::row_count() const
{
    return rows_.size();
}

template <typename ItemType>
void ts::gui::Vertical_list<ItemType>::apply_style(const Vertical_list_style& style)
{
    apply_style_impl(style);
}

template <typename ItemType>
void ts::gui::Vertical_list<ItemType>::apply_style_impl(const Vertical_list_style& style)
{
    if (style.row_size.y == 0.0)
    {
        Vector2<double> position;

        for (auto row : rows_)
        {
            row->set_auto_sizing(true);
            row->set_position(position);

            position.y += row->size().y + style.row_spacing;
        }
    }

    else
    {
        for (auto row : rows_)
        {
            row->set_auto_sizing(false);
            row->set_size(style.row_size);
        }
    }
}

#endif