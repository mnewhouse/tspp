/*
 * Turbo Sliders++
 * Copyright (C) 2013 Martin Newhouse
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

#ifndef WORLD_CAR_HPP
#define WORLD_CAR_HPP

#include "controls/controllable.hpp"

#include "resources/car_definition.hpp"

#include "entity.hpp"
#include "handling.hpp"

namespace ts
{

    namespace world
    {

        class World;

        class Car
            : public Entity, public controls::Controllable
        {
        public:
            Car(World* world, const resources::Car_definition& car_definition);

            const resources::Car_definition& car_definition() const;

            virtual void update(double frame_duration) override;

        private:
            resources::Car_definition car_definition_;
            Handling handling_;
        };

    }

}

#endif