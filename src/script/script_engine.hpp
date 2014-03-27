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

#ifndef SCRIPT_SCRIPT_HPP
#define SCRIPT_SCRIPT_HPP

#include <utility>
#include <string>
#include <exception>

class asIScriptContext;
class asIScriptEngine;
class asIScriptModule;
class asIScriptFunction;
class asIScriptObject;

namespace ts
{
    namespace script
    {
        class Engine;

        class Module_handle
        {
        public:
            Module_handle(Engine* engine, std::string module_name);

            void run();

            operator bool() const;

        private:
            Engine* script_engine_;
            std::string module_name_;
            asIScriptModule* module_handle_;
        };

        class Engine
        {
        public:
            Engine();
            virtual ~Engine();

            Module_handle load_module(const std::string& module_name, const std::string& file_name);

            template <typename... Args>
            void call_function(asIScriptFunction* function, asIScriptObject* object, Args&&... args);

            asIScriptEngine* engine() const;
            asIScriptContext* context() const;

        private:
            asIScriptEngine* script_engine_;
            asIScriptContext* script_context_;
        };

        class Script_error
            : public std::exception
        {
        };

        class Unrecoverable_error
            : public Script_error
        {
        };

        class File_not_found_error
            : public Script_error
        {
        };

        class Load_error
            : public Script_error
        {
        };

        class No_main_error
            : public Script_error
        {

        };

        template <int Index>
        void push_arguments(asIScriptContext* context) {}

        template <int Index, typename... Args>
        void push_arguments(asIScriptContext* context, std::size_t value, Args&&... args);

        template <int Index, typename... Args>
        void push_arguments(asIScriptContext* context, double value, Args&&... args);
    }
}

template <typename... Args>
void ts::script::Engine::call_function(asIScriptFunction* function, asIScriptObject* object, Args&&... args)
{
    auto ctx = context();
    ctx->Prepare(function);
    if (object) ctx->SetObject(object);

    push_arguments<0>(ctx, std::forward<Args>(args)...);

    ctx->Execute();
}


template <int Index, typename... Args>
void ts::script::push_arguments(asIScriptContext* context, std::size_t value, Args&&... args)
{
    context->SetArgQWord(Index, value);
    push_arguments<Index + 1>(context, std::forward<Args>(args)...);
}

template <int Index, typename... Args>
void ts::script::push_arguments(asIScriptContext* context, double value, Args&&... args)
{
    context->SetArgDouble(Index, value);
    push_arguments<Index + 1>(context, std::forward<Args>(args)...);
}

#endif