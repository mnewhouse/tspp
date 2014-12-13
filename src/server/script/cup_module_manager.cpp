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
#include "cup_module_manager.hpp"

#include "cup/cup_controller.hpp"
#include "cup/cup_controller_listener.hpp"

#include "script/script_engine.hpp"
#include "script/script_module.hpp"

struct ts::server::Cup_module_manager::Impl
    : public cup::Cup_controller_listener
{
    Impl(cup::Cup_controller* cup_controller, script::Engine* engine);
    ~Impl();

    void load_script_modules();

    void load_script_module(const resources::Script_handle& script_handle);
    void unload_script_module(const resources::Script_handle& script_handle);

    virtual void on_resource_start(resources::Script_handle resource) override;
    virtual void on_resource_stop(resources::Script_handle resource) override;

    cup::Cup_controller* cup_controller_;
    script::Engine* engine_;

    std::vector<script::Unique_module_handle> modules_;
};

ts::server::Cup_module_manager::Impl::Impl(cup::Cup_controller* cup_controller, script::Engine* engine)
: cup_controller_(cup_controller),
  engine_(engine)
{
    cup_controller_->add_cup_controller_listener(this);
}

ts::server::Cup_module_manager::Impl::~Impl()
{
    cup_controller_->remove_cup_controller_listener(this);
}

void ts::server::Cup_module_manager::Impl::on_resource_start(resources::Script_handle resource)
{
    load_script_module(resource);
}

void ts::server::Cup_module_manager::Impl::on_resource_stop(resources::Script_handle resource)
{
    unload_script_module(resource);
}

void ts::server::Cup_module_manager::Impl::load_script_modules()
{
    for (const auto& resource : cup_controller_->loaded_resources())
    {
        load_script_module(resource);
    }
}

void ts::server::Cup_module_manager::Impl::load_script_module(const resources::Script_handle& script_handle)
{
    auto module = engine_->create_module(script_handle->name(), script::take_ownership);
    module->register_interface(script_handle.get());

    for (const auto& file_name : script_handle->cup_scripts())
    {
        module->load_file(file_name);
    }

    module->execute(engine_->api_definitions());
    modules_.push_back(std::move(module));
}

void ts::server::Cup_module_manager::Impl::unload_script_module(const resources::Script_handle& script_handle)
{
    if (auto module = engine_->get_module_by_name(script_handle->name()))
    {
        engine_->unload_module(module);
    }    
}

ts::server::Cup_module_manager::Cup_module_manager(cup::Cup_controller* cup_controller, script::Engine* engine)
: impl_(std::make_unique<Impl>(cup_controller, engine))
{
}

ts::server::Cup_module_manager::~Cup_module_manager()
{
}

void ts::server::Cup_module_manager::load_script_modules()
{
    impl_->load_script_modules();
}