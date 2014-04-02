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

#ifndef CONTROLS_KEY_MAPPING_HPP
#define CONTROLS_KEY_MAPPING_HPP

#include "control.hpp"

#include <SFML/Window.hpp>

#include <map>
#include <cstdint>

namespace ts
{
    namespace controls
    {
        using Key = sf::Keyboard::Key;

        struct Key_bind
        {
            Key key;
            Slot slot;
        };

        inline bool operator<(const Key_bind& a, const Key_bind& b)
        {
            if (a.key < b.key) return true;
            if (b.key < a.key) return false;
            
            return a.key < b.key;
        }

        class Key_mapping
        {
        public:
            using Map_type = std::map<Key_bind, Control>;

            void bind_key_to_control(Key key, Slot slot, Control control);
            Control get_control_bound_to_key(Key key, Slot slot) const;
            
            Map_type::const_iterator begin() const;
            Map_type::const_iterator end() const;

            std::pair<Map_type::const_iterator, Map_type::const_iterator> get_all_binds_by_key(Key key) const;

        private:
            std::map<Key_bind, Control> key_mapping_;
        };

        Key_mapping default_key_mapping();
        sf::Keyboard::Key key_from_code(std::int32_t code);
        const char* key_to_string(sf::Keyboard::Key key);
    }
}

#endif