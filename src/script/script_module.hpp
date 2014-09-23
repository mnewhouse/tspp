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

#ifndef SCRIPT_MODULE_HPP
#define SCRIPT_MODULE_HPP

#include "script_vm.hpp"
#include "event_manager.hpp"
#include "error_level.hpp"
#include "execution_context.hpp"
#include "script_userdata.hpp"

namespace ts
{
    namespace script
    {
        struct API_definition;
        class Engine;

        class Module
        {
        public:
            Module(Engine* engine);

            void register_api(const API_definition& api_definition);
            void do_file(const utf8_string& file_name);

            Event_handler add_event_handler(const utf8_string& event_name, Function function);
            void remove_event_handler(const Event_handler& event_handler);

            template <typename... Args>
            void trigger_event(const utf8_string& event_name, Args&&... args);

            void report_error(const utf8_string& error_string, Error_level error_level);

            template <typename... Args>
            void call_function(const Function& function, Args&&... args) const;

            Engine* engine();

        private:
            template <typename T>
            T&& forward_argument(std::remove_reference_t<T>& argument, ...) const;

            template <typename T, typename U>
            Userdata<U> forward_argument(const Userdata_forwarder<U>& udata_forwarder) const;

            Engine* engine_;
            Virtual_machine vm_;
            Event_manager event_manager_;
        };

        Engine* get_engine_by_vm(HSQUIRRELVM vm);
        Module* get_module_by_vm(HSQUIRRELVM vm);

        SQInteger error_handler(HSQUIRRELVM vm);
        SQInteger compile_error_handler(HSQUIRRELVM vm);
    }
}

template <typename T>
T&& ts::script::Module::forward_argument(std::remove_reference_t<T>& argument, ...) const
{
    return std::forward<T>(argument);
}

template <typename T, typename U>
ts::script::Userdata<U> ts::script::Module::forward_argument(const Userdata_forwarder<U>& udata_forwarder) const
{
    return make_userdata(vm_, udata_forwarder.value);
}

template <typename... Args>
void ts::script::Module::trigger_event(const utf8_string& event_name, Args&&... args)
{
    event_manager_.trigger_event(event_name, forward_argument<Args>(args)...);
}

template <typename... Args>
void ts::script::Module::call_function(const Function& function, Args&&... args) const
{
    Execution_context context(vm_);
    context(function, forward_argument<Args>(args)...);
}

#endif