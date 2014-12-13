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

#ifndef SCRIPT_MODULE_HANDLE_HPP
#define SCRIPT_MODULE_HANDLE_HPP

namespace ts
{
    namespace script
    {
        class Engine;
        class Module;

        struct take_ownership_t {};
        static const take_ownership_t take_ownership;

        struct Module_handle
            : private Pointer_handle<Module>
        {
        public:
            Module_handle() = default;
            Module_handle(Module* module, const utf8_string* module_name);

            using Pointer_handle<Module>::operator->;
            using Pointer_handle<Module>::operator*;
            using Pointer_handle<Module>::operator bool;

            const utf8_string& module_name() const;
        private:
            const utf8_string* module_name_ = nullptr;
        };

        struct Unique_module_handle
            : private Module_handle
        {
        public:
            Unique_module_handle() = default;
            Unique_module_handle(Module_handle handle, Engine* engine);

            ~Unique_module_handle();

            Unique_module_handle(const Unique_module_handle&) = delete;
            Unique_module_handle(Unique_module_handle&& other);

            Unique_module_handle& operator=(const Unique_module_handle&) = delete;
            Unique_module_handle& operator=(Unique_module_handle&& other);

            using Module_handle::operator->;
            using Module_handle::operator*;
            using Module_handle::operator bool;
            using Module_handle::module_name;

        private:
            Engine* engine_ = nullptr;
        };
    }
}

#endif