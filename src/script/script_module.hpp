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
#include "script_interface.hpp"
#include "event_manager.hpp"
#include "execution_context.hpp"
#include "userdata_forwarder.hpp"
#include "error_level.hpp"

namespace ts
{
    namespace script
    {
        struct API_definition;
        class Engine;

        namespace impl
        {
            template <typename T>
            struct Argument_forward_helper
            {
                explicit Argument_forward_helper(HSQUIRRELVM vm) {}

                template <typename U>
                U& operator()(U& value) const
                {
                    return value;
                }

                template <typename U>
                U&& operator()(U&& value) const
                {
                    return value;
                }
            };

            template <typename T>
            struct Argument_forward_helper<Userdata_forwarder<T>>
            {
                explicit Argument_forward_helper(HSQUIRRELVM vm)
                    : vm_(vm)
                {}

                template <typename U>
                Userdata<std::decay_t<U>> operator()(const Userdata_forwarder<U>& forwarder) const
                {
                    return make_userdata(vm_, forwarder.value);
                }

                HSQUIRRELVM vm_;
            };
        }
        

        class Module
        {
        public:
            Module(Engine* engine);

            template <typename InterfaceType>
            void register_interface(InterfaceType* interface);

            void register_api(const API_definition& api_definition);

            void load_file(const utf8_string& file_name);
            void execute(const std::vector<API_definition>& api_definitions);

            Event_handler add_event_handler(const utf8_string& event_name, Function function);
            void remove_event_handler(const Event_handler& event_handler);

            template <typename... Args>
            void trigger_event(const utf8_string& event_name, Args&&... args) const;

            void report_error(const utf8_string& error_string, Error_level error_level);

            template <typename... Args>
            void call_function(const Function& function, Args&&... args) const;

            Engine* engine();

        private:
            template <typename T>
            auto forward_argument(std::remove_reference_t<T>& argument) const ->
                decltype(impl::Argument_forward_helper<std::decay_t<T>>(vm_)(argument))
            {
                return impl::Argument_forward_helper<std::decay_t<T>>(vm_)(argument);
            }

            Engine* engine_;
            Virtual_machine vm_;
            Event_manager event_manager_;

            std::vector<Function> execution_list_;
        };

        SQInteger error_handler(HSQUIRRELVM vm);
        void compile_error_handler(HSQUIRRELVM vm, const SQChar* desc, const SQChar* source, SQInteger line, SQInteger column);
    }
}

template <typename... Args>
void ts::script::Module::trigger_event(const utf8_string& event_name, Args&&... args) const
{
    event_manager_.trigger_event(event_name, forward_argument<Args>(args)...);
}

template <typename... Args>
void ts::script::Module::call_function(const Function& function, Args&&... args) const
{
    Execution_context context(vm_);
    context(function, forward_argument<Args>(args)...);
}

template <typename InterfaceType>
void ts::script::Module::register_interface(InterfaceType* interface)
{
    script::register_interface(vm_, make_interface(interface));
}


#endif