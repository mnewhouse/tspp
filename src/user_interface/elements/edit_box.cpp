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
#include "edit_box.hpp"

ts::gui::Edit_box::Edit_box(Text_style style)
: Text_element("", style)
{
    register_event_listeners();
}

void ts::gui::Edit_box::reset()
{
    set_text("");

    caret_index_ = 0;

    cursor_.clock.restart();
    cursor_.show = true;
}

void ts::gui::Edit_box::set_max_length(std::size_t max_length)
{
    max_length_ = max_length;

    if (max_length != 0 && text().count() > max_length)
    {
        auto string = text();
        string.resize(max_length);
        set_text(std::move(string));
    }
}

void ts::gui::Edit_box::register_event_listeners()
{
    add_event_handler(events::on_text_input, [this](Element& element, utf8::uint32_t code_point)
    {
        if (code_point >= 32)
        {
            auto string = text();
            if (max_length_ == 0 || string.count() < max_length_)
            {
                auto it = std::next(string.begin(), caret_index_);
                string.insert(it, code_point);
                set_text(std::move(string));

                ++caret_index_;
            }
        }
    });

    add_event_handler(events::on_key_down, [this](Element& element, sf::Keyboard::Key key, unsigned int modifiers)
    {
        if (key == sf::Keyboard::Left)
        {
            if (caret_index_ > 0)
            {
                --caret_index_;
            }
        }

        else if (key == sf::Keyboard::Right)
        {
            if (caret_index_ < text().count())
            {
                ++caret_index_;
            }
        }

        else if (key == sf::Keyboard::Home)
        {
            caret_index_ = 0;
        }

        else if (key == sf::Keyboard::End)
        {
            caret_index_ = text().count();
        }

        else if (key == sf::Keyboard::BackSpace)
        {           

            if (caret_index_ != 0)
            {
                --caret_index_;

                auto string = text();                
                
                auto it = std::next(string.begin(), caret_index_);
                string.erase(it);

                set_text(std::move(string));
            }

        }

        else if (key == sf::Keyboard::Delete)
        {
            auto string = text();

            if (caret_index_ < string.count())
            {
                auto it = std::next(string.begin(), caret_index_);
                string.erase(it);

                set_text(std::move(string));
            }
        }
    });
}

void ts::gui::Edit_box::draw(sf::RenderTarget& render_target, sf::RenderStates render_states) const
{
    auto char_position = text_item().character_position(caret_index_);

    if (cursor_.clock.getElapsedTime().asSeconds() > 0.5f)
    {
        cursor_.show = !cursor_.show;
        cursor_.clock.restart();
    }

    Text_element::draw(render_target, render_states);

    if (active() && cursor_.show)
    {
        sf::Vertex vertices[4];
        vertices[0].position.x = static_cast<float>(char_position.x);
        vertices[0].position.y = static_cast<float>(char_position.y);
        vertices[1].position.x = vertices[0].position.x - 1.0f;
        vertices[1].position.y = vertices[0].position.y;
        vertices[2].position.x = vertices[1].position.x;
        vertices[2].position.y = vertices[0].position.y - text_item().character_size();
        vertices[3].position.x = vertices[0].position.x;
        vertices[3].position.y = vertices[2].position.y;

        for (auto& vertex : vertices)
        {
            vertex.color = text_item().color();
        }

        render_target.draw(vertices, 4, sf::Quads, render_states);
    }
}