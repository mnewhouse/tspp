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
#include "client_script_loader.hpp"

#include "script/module_loader.hpp"

struct ts::scene::Script_loader::Impl
{
    Impl(script::Engine* engine);

    script::Module_loader module_loader_;
    Module_list module_list_;
    const resources::Loading_interface* loading_interface_ = nullptr;
};

ts::scene::Script_loader::Impl::Impl(script::Engine* engine)
    : module_loader_(engine)
{
}

ts::scene::Script_loader::Script_loader(script::Engine* engine)
    : impl_(std::make_unique<Impl>(engine))
{
}

ts::scene::Script_loader::~Script_loader()
{
}

void ts::scene::Script_loader::async_load_scripts(std::vector<resources::Script_handle> scripts, std::function<void()> completion_callback)
{
    std::vector<script::Module_definition> definitions;
    for (const auto& script : scripts)
    {
        definitions.emplace_back();
        definitions.back().module_name = script->name();
        definitions.back().file_list = script->client_scripts();
    }

    auto callback_hook = [this, completion_callback](Module_list module_list)
    {
        impl_->loading_interface_ = nullptr;
        impl_->module_list_ = std::move(module_list);

        completion_callback();
    };    

    impl_->loading_interface_ = impl_->module_loader_.async_load_modules(definitions, callback_hook);
}

void ts::scene::Script_loader::poll()
{
    impl_->module_loader_.poll();
}

const ts::resources::Loading_interface* ts::scene::Script_loader::loading_interface() const
{
    return impl_->loading_interface_;
}

ts::scene::Script_conductor ts::scene::Script_loader::transfer_result()
{
    return Script_conductor(std::move(impl_->module_list_));
}