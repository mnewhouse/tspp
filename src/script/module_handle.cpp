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
#include "module_handle.hpp"
#include "script_engine.hpp"

ts::script::Module_handle::Module_handle(Module* module, const utf8_string* module_name)
    : Pointer_handle<Module>(module),
    module_name_(module_name)
{
}

const ts::utf8_string& ts::script::Module_handle::module_name() const
{
    return *module_name_;
}

ts::script::Unique_module_handle::Unique_module_handle(Module_handle handle, Engine* engine)
    : Module_handle(handle),
    engine_(engine)
{
}

ts::script::Unique_module_handle::~Unique_module_handle()
{
    if (engine_)
    {
        engine_->unload_module(*this);
    }
}

ts::script::Unique_module_handle::Unique_module_handle(Unique_module_handle&& other)
    : Module_handle(other),
    engine_(other.engine_)
{
    static_cast<Module_handle&>(other) = Module_handle();
    other.engine_ = nullptr;
}

ts::script::Unique_module_handle& ts::script::Unique_module_handle::operator=(Unique_module_handle&& rhs)
{
    static_cast<Module_handle&>(*this) = rhs;
    static_cast<Module_handle&>(rhs) = Module_handle();

    engine_ = rhs.engine_;
    rhs.engine_ = nullptr;
    return *this;
}