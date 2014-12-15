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

#ifndef CLIENT_SCRIPT_CONDUCTOR_HPP
#define CLIENT_SCRIPT_CONDUCTOR_HPP

namespace ts
{
    namespace script
    {
        struct Unique_module_handle;
    }

    namespace scene
    {
        using Module_list = std::vector<script::Unique_module_handle>;

        class Script_conductor
        {
        public:
            Script_conductor();
            explicit Script_conductor(Module_list list);
            ~Script_conductor();

            Script_conductor(Script_conductor&& other);
            Script_conductor& operator=(Script_conductor&& other);

            void execute();

        private:
            Module_list module_list_;
        };
    }
}

#endif