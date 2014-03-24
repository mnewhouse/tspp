#ifndef AUDIO_ENGINE_SOUNDS_HPP
#define AUDIO_ENGINE_SOUNDS_HPP

#include "audio_store.hpp"
#include "world/entity_listener.hpp"

#include <vector>
#include <memory>
#include <deque>

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


namespace ts
{
    namespace audio
    {
        class Car_sound_controller
            : public world::Entity_listener
        {
        public:
            virtual void on_car_create(world::Car* car) override;
            virtual void on_entity_destroy(world::Entity* entity) override;

            void update(std::size_t ticks);

        private:
            // pair hack - I would use a struct, but MSVC still doesn't have 
            // compiler-generated move operations. 
            using Engine_sound = std::pair<const world::Car*, std::unique_ptr<sf::Sound>>;

            std::vector<Engine_sound> engine_sounds_;

            Audio_store audio_store_;
        };
    }
}

#endif