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

#include "stdinc.hpp"
#include "server_cup_script.hpp"

#include "cup/cup_controller.hpp"

#include "resources/resource_store.hpp"
#include "resources/script_manager.hpp"
#include "resources/settings/script_settings.hpp"

#include "script/interfaces/cup_script_interface.hpp"

struct ts::server::Cup_script_interface::Impl
{
    Impl(Message_center* message_center, Command_center* command_center, cup::Cup_controller* cup_controller);

    void load_script_modules(const cup::Cup_controller* cup_controller);
    void load_script_module(const resources::Script_handle& script_handle);

    script_api::Cup_interface script_interface_;
};

ts::server::Cup_script_interface::Impl::Impl(Message_center* message_center, Command_center* command_center, cup::Cup_controller* cup_controller)
: script_interface_(message_center, command_center)
{
    script_interface_.register_console(script::Stdout_console());

    load_script_modules(cup_controller);
}

void ts::server::Cup_script_interface::Impl::load_script_modules(const cup::Cup_controller* cup_controller)
{
    const auto& script_manager = cup_controller->resource_store().script_manager();
    const auto& script_settings = cup_controller->script_settings();

    for (const auto& script_name : script_settings.loaded_scripts())
    {
        if (auto script_handle = script_manager.get_script_by_name(script_name))
        {
            load_script_module(script_handle);            
        }
    }
}

void ts::server::Cup_script_interface::Impl::load_script_module(const resources::Script_handle& script_handle)
{
    auto module = script_interface_.create_module(script_handle->name());

    for (const auto& file_name : script_handle->cup_scripts())
    {
        module->do_file(file_name);
    }
}

ts::server::Cup_script_interface::Cup_script_interface(Message_center* message_center, Command_center* command_center, 
                                                       cup::Cup_controller* cup_controller)
: impl_(std::make_unique<Impl>(message_center, command_center, cup_controller))
{
}

ts::server::Cup_script_interface::~Cup_script_interface()
{
}