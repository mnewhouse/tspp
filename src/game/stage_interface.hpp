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

#ifndef STAGE_INTERFACE_HPP
#define STAGE_INTERFACE_HPP

#include "stage_loader.hpp"

namespace ts
{
    namespace action
    {
        class Stage;
    }

    namespace cup
    {
        struct Stage_data;
    }

    namespace game
    {
        class Stage_interface
        {
        public:
            using Completion_callback = void(const action::Stage*);
            const Stage_loader* async_load_stage(const cup::Stage_data& stage_data, std::function<Completion_callback> completion_callback);
            void poll();

            void clean_stage();
            const action::Stage* stage() const;

            void update(std::size_t frame_duration);

        private:
            Stage_loader stage_loader_;
            std::unique_ptr<action::Stage> stage_;
        };
    }
}

#endif