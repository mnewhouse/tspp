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

#ifndef COMMON_API_REGISTRATION_HPP
#define COMMON_API_REGISTRATION_HPP

namespace ts
{
    namespace script
    {
        class Engine;
        class Event_interface;
    }

    namespace action
    {
        class Stage_interface;
    }

    namespace script_api
    {
        void register_core_api(script::Engine* engine);
        void register_event_api(script::Engine* engine, const script::Event_interface* event_interface);
        void register_stage_api(script::Engine* engine, const action::Stage_interface* stage_interface);

        void register_stdout_console(script::Engine* engine);
    }
}

#endif