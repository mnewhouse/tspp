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

#ifndef RESOURCES_PLAYER_STORE_HPP
#define RESOURCES_PLAYER_STORE_HPP

#include "player_definition.hpp"

namespace ts
{
    namespace resources
    {
        struct Player
            : public Player_definition
        {
            std::uint64_t internal_id;
        };

        static const std::size_t max_player_name_length = 20;

        using Player_handle = Pointer_handle<const Player>;

        class Player_store
        {
        public:
            Player_store(utf8_string players_file);
            ~Player_store();

            Player_handle create_player(utf8_string nickname);

            void delete_player(Player_handle player);

            using unique_id = std::uint64_t;
            static const unique_id invalid_id = 0;

            Player_handle get_player_by_id(unique_id id) const;

            void set_player_color(Player_handle player_handle, resources::Player_color new_color);

            struct iterator
                : public std::iterator<std::bidirectional_iterator_tag, Player_handle>
            {
                Player_handle operator*() const;
                Player_handle operator->() const;

                iterator& operator++();
                iterator& operator--();

                iterator operator++(int);
                iterator operator--(int);

                bool operator==(const iterator& rhs) const;
                bool operator!=(const iterator& rhs) const;

            private:
                using base_type = std::map<unique_id, Player>::const_iterator;
                explicit iterator(base_type it);
                friend Player_store;

                base_type it_;
            };

            iterator begin() const;
            iterator end() const;

        private:
            void load_from_file();
            void save_to_file();

            unique_id allocate_unique_id() const;
            std::uint64_t generate_player_id() const;
            Player_color generate_player_color() const;

            utf8_string file_name_;
            std::map<unique_id, Player> player_map_;
            mutable std::mt19937_64 random_engine_;
        };
    }
}


#endif