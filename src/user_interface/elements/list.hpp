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

#ifndef GUI_ELEMENTS_LIST_HPP
#define GUI_ELEMENTS_LIST_HPP

#include "../gui_element.hpp"

#include "background_decorator.hpp"

namespace ts
{
    namespace gui
    {
        namespace elements
        {
            class List_row
                : public gui::Element
            {
            public:
                List_row(Vector2i size, sf::Color background_color)
                    : background_decorator_(create_decorator<gui::elements::Background_color>(background_color))
                {
                    set_size(size);                    
                }

                void set_background_color(sf::Color color)
                {
                    background_decorator_->set_color(color);
                }

            private:
                gui::elements::Background_color* background_decorator_;
            };

            class List
                : public Element
            {
            public:
                List(Vector2i item_size, sf::Color even_color, sf::Color odd_color)
                    : item_size_(item_size), even_color_(even_color), odd_color_(odd_color)
                {
                }

                template <typename ElemType, typename... Args>
                ElemType* create_header(Args&&... args)
                {

                }

                template <typename ElemType, typename... Args>
                ElemType* add_row(Args&&... args)
                {
                    auto color = ((row_count() & 1) == 0 ? odd_color_ : even_color_);
                    auto list_row = create_child<List_row>(item_size_, color);
                    
                    list_row->set_position(0, rows_.size() * item_size_.y);
                    rows_.push_back(list_row);                    

                    auto result = list_row->create_child<ElemType>(std::forward<Args>(args)...);
                    return result;
                }

                std::size_t row_count() const
                {
                    return rows_.size();
                }

                void clear()
                {
                    for (auto row : rows_)
                    {
                        row->destroy();
                    }

                    rows_.clear();
                }


            private:
                List_row* header_;
                std::vector<List_row*> rows_;

                Vector2i item_size_;
                sf::Color even_color_;
                sf::Color odd_color_;
            };
        }
    }
}

#endif