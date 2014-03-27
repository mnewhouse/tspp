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

#include "callback.hpp"

#include <algorithm>
#include <iostream>

ts::script::Callback_listener::Callback_listener(asIScriptObject* listener)
    : object_(listener)
{
}

ts::script::Callback_listener::~Callback_listener()
{
    if (object_) object_->Release();
}

ts::script::Callback_listener::Callback_listener(const Callback_listener& listener)
    : object_(listener.object_)
{
    if (object_) object_->AddRef();
}

ts::script::Callback_listener::Callback_listener(Callback_listener&& listener)
    : object_(listener.object_)
{
    listener.object_ = nullptr;
}

asIScriptObject* ts::script::Callback_listener::object() const
{
    return object_;
}

ts::script::Callback_listener& ts::script::Callback_listener::operator=(Callback_listener listener)
{
    if (object_ = listener.object_) object_->AddRef();

    return *this;
}

const std::vector<ts::script::Callback_listener>& ts::script::Callback_list::callbacks() const
{
    return callbacks_;
}

void ts::script::Callback_list::store(asIScriptObject* listener)
{
    callbacks_.emplace_back(listener);
}

void ts::script::Callback_list::remove(asIScriptObject* listener)
{
    callbacks_.erase(std::remove_if(callbacks_.begin(), callbacks_.end(), 
        [listener](const Callback_listener& other)
    {
        return other.object() == listener;
    }), callbacks_.end());
}