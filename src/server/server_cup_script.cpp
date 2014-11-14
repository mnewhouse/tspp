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
#include "cup/cup_controller_listener.hpp"

#include "resources/resource_store.hpp"
#include "resources/script_manager.hpp"
#include "resources/settings/script_settings.hpp"

#include "script/interfaces/cup_script_interface.hpp"
#include "script/script_module.hpp"

struct ts::server::Cup_script_interface::Impl
    : public cup::Cup_controller_listener
{
    Impl(Message_center* message_center, Command_center* command_center, 
         cup::Cup_controller* cup_controller, Client_map* client_map);

    ~Impl();

    void load_script_modules(const cup::Cup_controller* cup_controller);

    void load_script_module(const resources::Script_handle& script_handle);
    void unload_script_module(const resources::Script_handle& script_handle);

    virtual void on_resource_start(resources::Script_handle resource) override;
    virtual void on_resource_stop(resources::Script_handle resource) override;

    script_api::Cup_interface script_interface_;
    cup::Cup_controller* cup_controller_;

    resources::Script_handle current_gamemode_;
};

ts::server::Cup_script_interface::Impl::Impl(Message_center* message_center, Command_center* command_center, 
                                             cup::Cup_controller* cup_controller, Client_map* client_map)
: script_interface_(message_center, command_center, client_map),
  cup_controller_(cup_controller)
{
    cup_controller->add_cup_controller_listener(this);

    script_interface_.register_console(script::Stdout_console());

    load_script_modules(cup_controller);
}

ts::server::Cup_script_interface::Impl::~Impl()
{
    cup_controller_->remove_cup_controller_listener(this);
}

void ts::server::Cup_script_interface::Impl::on_resource_start(resources::Script_handle resource)
{
    load_script_module(resource);
}

void ts::server::Cup_script_interface::Impl::on_resource_stop(resources::Script_handle resource)
{
    unload_script_module(resource);
}

void ts::server::Cup_script_interface::Impl::load_script_modules(const cup::Cup_controller* cup_controller)
{
    for (const auto& resource : cup_controller_->loaded_resources())
    {
        load_script_module(resource);
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

void ts::server::Cup_script_interface::Impl::unload_script_module(const resources::Script_handle& script_handle)
{
    if (auto module = script_interface_.get_module_by_name(script_handle->name()))
    {
        script_interface_.unload_module(module);
    }    
}

ts::server::Cup_script_interface::Cup_script_interface(Message_center* message_center, Command_center* command_center, 
                                                       cup::Cup_controller* cup_controller, Client_map* client_map)
: impl_(std::make_unique<Impl>(message_center, command_center, cup_controller, client_map))
{
}

ts::server::Cup_script_interface::~Cup_script_interface()
{
}

void ts::server::Cup_script_interface::handle_client_connect(const Generic_client& client)
{
    impl_->script_interface_.trigger_event("onClientConnect", nullptr, script::forward_as_userdata(client));
}

void ts::server::Cup_script_interface::handle_client_disconnect(const Generic_client& client)
{
    impl_->script_interface_.trigger_event("onClientDisconnect", nullptr, script::forward_as_userdata(client));
}

void ts::server::Cup_script_interface::handle_pre_initialization(Stage_assembler* stage_assembler)
{
    impl_->script_interface_.trigger_event("onStagePreInitialize", nullptr, script::forward_as_userdata(stage_assembler));
}