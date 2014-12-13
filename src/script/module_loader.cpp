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
#include "module_loader.hpp"
#include "script_engine.hpp"
#include "script_module.hpp"

ts::script::Module_loader::Module_loader(Engine* engine)
    : engine_(engine)
{
}

ts::script::Module_loader::~Module_loader()
{
}

void ts::script::Module_loader::async_load_modules(std::vector<Module_definition> definitions, Completion_callback callback)
{
    std::vector<Unique_module_handle> module_list;

    // First, create the modules synchronously, because this is not really a potential bottleneck.
    for (const auto& module_def : definitions)
    {
        auto module = engine_->create_module(module_def.module_name, script::take_ownership);
        module_list.push_back(std::move(module));
    }

    auto loader_func = [this](const std::vector<Unique_module_handle>& modules, const std::vector<Module_definition>& definitions)
    {
        for (std::size_t index = 0; index != modules.size() && index != definitions.size(); ++index)
        {
            for (const auto& file : definitions[index].file_list)
            {
                modules[index]->load_file(file);
            }
        }
    };

    auto completion_hook = [this, callback](std::vector<Unique_module_handle> modules)
    {
        // Finally, when the modules are loaded, execute them synchronously again.
        for (const auto& module : modules)
        {
            module->execute(engine_->api_definitions());
        }

        // And inform the caller, transfering the ownership to them.
        // callback(std::move(modules));
    };
    
    loading_states_.emplace_back();
    loading_states_.back().modules = std::move(module_list);
    loading_states_.back().definitions = std::move(definitions);
    loading_states_.back().completion_callback = std::move(callback);

    try
    {
        // Load and compile the script code asynchronously, because this is a potentially slow operation.
        loading_states_.back().future = std::async(std::launch::async, loader_func,
            std::cref(loading_states_.back().modules), std::cref(loading_states_.back().definitions));
    }

    catch (...)
    {
        loading_states_.pop_back();
        throw;
    }
}

void ts::script::Module_loader::poll()
{
    for (auto it = loading_states_.begin(); it != loading_states_.end();  )
    {
        auto& future = it->future;

        // Test if future is ready
        if (future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
        {
            it->completion_callback(std::move(it->modules));
            future.get();
        }

        // Now test if it's still valid - if not, erase this entry
        if (!future.valid())
        {
            it = loading_states_.erase(it);
        }

        else
        {
            // Otherwise, just increment the iterator
            ++it;
        }
    }
}