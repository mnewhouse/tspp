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

#ifndef GUI_ELEMENT_STYLE_HPP
#define GUI_ELEMENT_STYLE_HPP

namespace ts
{
    namespace gui
    {
        template <typename StateType, typename StyleType>
        class Element_style
        {
        public:
            Element_style(StyleType default_style = {});

            void register_style(StateType state, StyleType style);

            template <typename ApplyFunc>
            void state_change(const StateType& active_state, ApplyFunc apply_style);

            template <typename ApplyFunc>
            void state_change(ApplyFunc apply_style);

            void set_default_style(const StyleType& style);
            const StyleType& default_style() const;

            const StyleType& current_style() const;

        private:
            std::vector<StateType> states_;
            std::unordered_map<StateType, StyleType> style_map_;
            StyleType default_style_;
            const StyleType* current_style_;
        };
    }
}

template <typename StateType, typename StyleType>
ts::gui::Element_style<StateType, StyleType>::Element_style(StyleType default_style)
: default_style_(std::move(default_style)),
  current_style_(&default_style_)
{
}

template <typename StateType, typename StyleType>
void ts::gui::Element_style<StateType, StyleType>::set_default_style(const StyleType& style)
{
    default_style_ = style;
}

template <typename StateType, typename StyleType>
const StyleType& ts::gui::Element_style<StateType, StyleType>::default_style() const
{
    return default_style_;
}

template <typename StateType, typename StyleType>
const StyleType& ts::gui::Element_style<StateType, StyleType>::current_style() const
{
    return *current_style_;
}

template <typename StateType, typename StyleType>
void ts::gui::Element_style<StateType, StyleType>::register_style(StateType state, StyleType style)
{
    auto it = style_map_.find(state);
    if (it != style_map_.end())
    {
        it->second = style;
    }

    else
    {
        style_map_.emplace(std::move(state), std::move(style));
    }
}

template <typename StateType, typename StyleType>
template <typename ApplyFunc>
void ts::gui::Element_style<StateType, StyleType>::state_change(const StateType& new_state, ApplyFunc apply_style)
{
    auto it = style_map_.find(new_state);
    if (it != style_map_.end())
    {
        apply_style(it->second);
        current_style_ = &it->second;
    }
};

template <typename StateType, typename StyleType>
template <typename ApplyFunc>
void ts::gui::Element_style<StateType, StyleType>::state_change(ApplyFunc apply_style)
{
    apply_style(default_style_);
    current_style_ = &default_style_;
};


#endif
