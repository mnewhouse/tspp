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

#ifndef STAGE_LOADER_HPP
#define STAGE_LOADER_HPP

#include "generic_loader.hpp"

#include "action/stage.hpp"

namespace ts
{
    namespace cup
    {
        struct Stage_data;
    }

    namespace game
    {
        enum class Stage_loader_state
        {
            None,
            Preprocessing,
            Creating_world,
            Building_pattern,
            Creating_entities,
            Loading_scripts,
            Complete
        };

        class Stage_loader
            : public Generic_loader<Stage_loader_state, std::unique_ptr<action::Stage>>
        {
        public:
            Stage_loader();
            ~Stage_loader();

            Stage_loader(const Stage_loader&) = delete;
            Stage_loader& operator=(const Stage_loader&) = delete;

            using Script_loader_function = std::function<void(action::Stage*)>;
            void async_load(const action::Stage_data& stage_data, Script_loader_function script_loader);

            // Inherited function: poll()
            // Throws resources::Pattern_load_error or resources::Broken_track_exception.
            using State = Stage_loader_state;

        private:
            std::unique_ptr<action::Stage> load_stage(const action::Stage_data& stage_data, const Script_loader_function& script_loader);
        };

        utf8_string to_string(Stage_loader_state state);
    }
}

#endif