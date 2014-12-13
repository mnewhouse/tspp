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
#include "stage_module_manager.hpp"

#include "cup/cup_controller.hpp"
#include "cup/cup_listener.hpp"
#include "cup/stage_data.hpp"

#include "script/script_engine.hpp"
#include "script/module_loader.hpp"

struct ts::server::Stage_module_manager::Impl
{
    Impl(script::Engine* engine);
    ~Impl();

    void async_load_modules(const std::vector<resources::Script_handle>& scripts, Completion_callback callback);
    void unload_all_modules();
    void poll();

    using Module_list = std::vector<script::Unique_module_handle>;

    Module_list modules_;
    script::Module_loader module_loader_;
    script::Engine* engine_;
};

ts::server::Stage_module_manager::Impl::Impl(script::Engine* engine)
: engine_(engine),
  module_loader_(engine)
{
}

ts::server::Stage_module_manager::Impl::~Impl()
{
}

void ts::server::Stage_module_manager::Impl::async_load_modules(const std::vector<resources::Script_handle>& scripts, Completion_callback callback)
{
    // Create module definitions - name and files
    std::vector<script::Module_definition> module_definitions;
    for (const auto& script : scripts)
    {
        module_definitions.emplace_back();
        module_definitions.back().file_list = script->server_scripts();
        module_definitions.back().module_name = script->name();        
    }

    auto callback_hook = [this, callback](std::vector<script::Unique_module_handle> modules)
    {
        // Adopt the module handles
        std::move(modules.begin(), modules.end(), std::back_inserter(modules_));

        // And inform the caller.
        callback();
    };

    module_loader_.async_load_modules(std::move(module_definitions), callback_hook);
}

void ts::server::Stage_module_manager::Impl::unload_all_modules()
{
    modules_.clear();
}

void ts::server::Stage_module_manager::Impl::poll()
{
    module_loader_.poll();
}

ts::server::Stage_module_manager::Stage_module_manager(script::Engine* engine)
: impl_(std::make_unique<Impl>(engine))
{
}

ts::server::Stage_module_manager::~Stage_module_manager()
{
}

void ts::server::Stage_module_manager::poll()
{
    impl_->poll();
}

void ts::server::Stage_module_manager::async_load_modules(const std::vector<resources::Script_handle>& scripts, Completion_callback callback)
{
    impl_->async_load_modules(scripts, callback);
}

void ts::server::Stage_module_manager::unload_all_modules()
{
    impl_->unload_all_modules();
}