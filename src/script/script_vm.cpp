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
#include "script_vm.hpp"

ts::script::Virtual_machine::Virtual_machine()
: vm_(sq_open(1024))
{
}

ts::script::Virtual_machine::~Virtual_machine()
{
    sq_close(vm_);
}

ts::script::Virtual_machine::Virtual_machine(Virtual_machine&& other)
: vm_(other.vm_)
{
    other.vm_ = nullptr;
}

ts::script::Virtual_machine& ts::script::Virtual_machine::operator=(Virtual_machine other)
{
    std::swap(vm_, other.vm_);

    return *this;
}

ts::script::Virtual_machine::operator HSQUIRRELVM() const
{
    return vm_;
}

