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

#ifndef SCRIPT_CALLBACK_HPP
#define SCRIPT_CALLBACK_HPP

#include "script_engine.hpp"

#include <angelscript.h>

#include <utility>
#include <vector>

namespace ts
{
    namespace script
    {

        class Callback_listener
        {
        public:
            explicit Callback_listener(asIScriptObject* object);
            ~Callback_listener();

            Callback_listener(const Callback_listener& listener);
            Callback_listener(Callback_listener&& listener);

            Callback_listener& operator=(Callback_listener listener);

            asIScriptObject* object() const;

            template <typename... Args>
            void operator()(Engine* engine, const char* declaration, Args&&... args) const;

        private:
            asIScriptObject* object_;
        };

        class Callback_list
        {
        public:
            void store(asIScriptObject* listener);
            void remove(asIScriptObject* listener);

            const std::vector<Callback_listener>& callbacks() const;
        private:
            std::vector<Callback_listener> callbacks_;
        };
    }
}

template <typename... Args>
void ts::script::Callback_listener::operator()(Engine* engine, const char* declaration, Args&&... args) const
{
    auto as_engine = engine->engine();

    auto type = object_->GetObjectType();
    auto function = type->GetMethodByDecl(declaration);

    engine->call_function(function, object_, std::forward<Args>(args)...);
}


#endif