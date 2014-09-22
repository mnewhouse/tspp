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

#ifndef GUI_OPTION_SET_HPP
#define GUI_OPTION_SET_HPP

#include "../stylable_element.hpp"

#include "text_element.hpp"

namespace ts
{
    namespace gui
    {
        struct Option_set_style
        {
            Vector2<double> text_area_size;
            Vector2<double> arrow_area_size;

            gui::Text_style text_style;
            gui::Text_style text_hover_style;
        };

        namespace option_set
        {
            enum Wrap_mode
            {
                wrap_around,
                no_wrap
            };
        }

        namespace option_set_detail
        {
            template <typename KeyType, option_set::Wrap_mode WrapMode = option_set::wrap_around>
            struct on_change_t{};
        }

        
        template <typename KeyType, option_set::Wrap_mode WrapMode = option_set::wrap_around>
        class Option_set
            : public Stylable_element<Option_set_style>
        {
        public:
            Option_set(Option_set_style style);

            void increment();
            void decrement();

            void set_value(const KeyType& value);
            const KeyType& value() const;

            void add_option(KeyType key, utf8_string value);

            static const option_set_detail::on_change_t<KeyType, WrapMode> on_change;

        private:
            virtual void apply_style(const Option_set_style& style) override;
            void apply_style_impl(const Option_set_style& style);

            std::map<KeyType, utf8_string> option_map_;

            KeyType current_value_;

            gui::Text_element* option_text_;
            gui::Text_element* increment_arrow_;
            gui::Text_element* decrement_arrow_;
        };

        template <typename KeyType, option_set::Wrap_mode WrapMode>
        struct Event_traits<option_set_detail::on_change_t<KeyType, WrapMode>>
        {
            using function_type = void(class Option_set<KeyType, WrapMode>&, KeyType new_value);
        };
    }
}

template <typename KeyType, ts::gui::option_set::Wrap_mode WrapMode>
const typename ts::gui::option_set_detail::on_change_t<KeyType, WrapMode> ts::gui::Option_set<KeyType, WrapMode>::on_change;

template <typename KeyType, ts::gui::option_set::Wrap_mode WrapMode>
ts::gui::Option_set<KeyType, WrapMode>::Option_set(Option_set_style style)
: Stylable_element(style),
option_text_(create_child<gui::Text_element>("", style.text_style)),
increment_arrow_(create_child<gui::Text_element>(">", style.text_style)),
decrement_arrow_(create_child<gui::Text_element>("<", style.text_style)),
current_value_()
{
    increment_arrow_->add_event_handler(gui::events::on_click,
        [this](const gui::Element& element)
    {
        increment();
    });

    decrement_arrow_->add_event_handler(gui::events::on_click,
        [this](const gui::Element& element)
    {
        decrement();
    });

    apply_style_impl(style);
}

template <typename KeyType, ts::gui::option_set::Wrap_mode WrapMode>
void ts::gui::Option_set<KeyType, WrapMode>::increment()
{
    auto it = option_map_.lower_bound(value());
    if (it == option_map_.end() || ++it == option_map_.end() && WrapMode == option_set::wrap_around)
    {
        it = option_map_.begin();
    }

    if (it != option_map_.end())
    {
        set_value(it->first);
    }
}

template <typename KeyType, ts::gui::option_set::Wrap_mode WrapMode>
void ts::gui::Option_set<KeyType, WrapMode>::add_option(KeyType key, utf8_string display_text)
{
    bool empty = option_map_.empty();

    option_map_.emplace(key, std::move(display_text));

    if (empty)
    {
        set_value(key);
    }
}

template <typename KeyType, ts::gui::option_set::Wrap_mode WrapMode>
void ts::gui::Option_set<KeyType, WrapMode>::decrement()
{
    auto it = option_map_.lower_bound(value());
    if (it != option_map_.begin())
    {
        --it;
    }

    else if (WrapMode == option_set::wrap_around)
    {
        it = std::prev(option_map_.end());
    }

    if (it != option_map_.end())
    {
        set_value(it->first);
    }
}

template <typename KeyType, ts::gui::option_set::Wrap_mode WrapMode>
void ts::gui::Option_set<KeyType, WrapMode>::set_value(const KeyType& value)
{
    auto it = option_map_.find(value);
    if (it != option_map_.end())
    {
        option_text_->set_text(it->second);

        if (value != current_value_)
        {
            trigger_event(on_change, *this, value);
            current_value_ = value;
        }
    }
}
template <typename KeyType, ts::gui::option_set::Wrap_mode WrapMode>
const KeyType& ts::gui::Option_set<KeyType, WrapMode>::value() const
{
    return current_value_;
}

template <typename KeyType, ts::gui::option_set::Wrap_mode WrapMode>
void ts::gui::Option_set<KeyType, WrapMode>::apply_style(const Option_set_style& style)
{
    apply_style_impl(style);
}

template <typename KeyType, ts::gui::option_set::Wrap_mode WrapMode>
void ts::gui::Option_set<KeyType, WrapMode>::apply_style_impl(const Option_set_style& style)
{
    increment_arrow_->set_size(style.arrow_area_size);
    decrement_arrow_->set_size(style.arrow_area_size);

    option_text_->set_size(style.text_area_size);
    option_text_->set_position({ style.arrow_area_size.x, 0.0 });

    increment_arrow_->set_position({ style.text_area_size.x + style.arrow_area_size.x, 0.0 });

    option_text_->set_default_style(style.text_style);
    increment_arrow_->set_default_style(style.text_style);
    decrement_arrow_->set_default_style(style.text_style);

    increment_arrow_->register_style(gui::states::hover, style.text_hover_style);
    decrement_arrow_->register_style(gui::states::hover, style.text_hover_style);

    increment_arrow_->center_text_horizontal();
    decrement_arrow_->center_text_horizontal();
    option_text_->center_text_horizontal();

    auto height = std::max(style.text_area_size.y, style.arrow_area_size.y);
    auto width = style.text_area_size.x + style.arrow_area_size.x * 2.0;
    set_size({ width, height });
}

#endif