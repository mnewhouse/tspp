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

#ifndef GUI_STYLABLE_ELEMENT_HPP
#define GUI_STYLABLE_ELEMENT_HPP

#include "gui_element.hpp"
#include "element_style.hpp"

namespace ts
{
    namespace gui
    {
        template <typename StyleType>
        class Stylable_element
            : public Element
        {
        public:
            using Styler_type = Element_style<State_name, StyleType>;

            template <typename... Args>
            Stylable_element(StyleType default_style, Args&&... args);    

            template <typename... Args>
            Stylable_element(Styler_type styler, Args&&... args);

            void register_style(State_name state_name, StyleType style);
            const StyleType& default_style() const;
            void set_default_style(StyleType style);

            const StyleType& current_style() const;

            virtual void on_state_change(const State_list& states) override;

        private:
            virtual void apply_style(const StyleType& style) = 0;

            Styler_type styler_;
        };
    }
}


template <typename StyleType>
template <typename... Args>
ts::gui::Stylable_element<StyleType>::Stylable_element(StyleType default_style, Args&&... args)
: Element(std::forward<Args>(args)...),
  styler_(std::move(default_style))
{
}

template <typename StyleType>
template <typename... Args>
ts::gui::Stylable_element<StyleType>::Stylable_element(Styler_type styler, Args&&... args)
: Element(std::forward<Args>(args)...),
  styler_(std::move(styler))
{
}

template <typename StyleType>
void ts::gui::Stylable_element<StyleType>::register_style(State_name state_name, StyleType style)
{
    styler_.register_style(std::move(state_name), std::move(style));
};

template <typename StyleType>
void ts::gui::Stylable_element<StyleType>::set_default_style(StyleType style)
{
    styler_.set_default_style(std::move(style));
}

template <typename StyleType>
const StyleType& ts::gui::Stylable_element<StyleType>::default_style() const
{
    return styler_.default_style();
}

template <typename StyleType>
const StyleType& ts::gui::Stylable_element<StyleType>::current_style() const
{
    return styler_.current_style();
}

template <typename StyleType>
void ts::gui::Stylable_element<StyleType>::on_state_change(const State_list& states)
{
    auto apply = [this](const StyleType& style)
    {
        apply_style(style); 
    };

    if (states.empty())
    {
        styler_.state_change(apply);
    }

    else
    {
        styler_.state_change(states.back(), apply);
    }
}

#endif

