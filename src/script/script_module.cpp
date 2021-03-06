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
#include "script_module.hpp"
#include "script_api.hpp"
#include "script_engine.hpp"
#include "script_utility.hpp"
#include "error_level.hpp"

#include <cstdarg>

namespace ts
{
    namespace script
    {
        template <Error_level ErrorLevel>
        void printf_console(HSQUIRRELVM vm, const SQChar* format, ...);
    }
}

template <ts::script::Error_level ErrorLevel>
void ts::script::printf_console(HSQUIRRELVM vm, const SQChar* format, ...)
{
    std::va_list args;
    va_start(args, format);

    if (auto engine = get_engine_by_vm(vm))
    {

#ifndef _MSC_VER
        char dummy{};
        int buffer_size = std::vsnprintf(&dummy, 0, format, args);
#else
        int buffer_size = _vscprintf(format, args);
#endif

        if (buffer_size >= 0)
        {
            std::string buffer(buffer_size + 1, 0);

#ifdef _MSC_VER
            vsnprintf_s(&buffer[0], buffer.size(), buffer.size(), format, args);
#else
            std::vsnprintf(&buffer[0], buffer.size(), format, args);
#endif
            buffer.pop_back(); // Pop terminating null byte

            engine->write_console_line(std::move(buffer), ErrorLevel);
        }
    }

    va_end(args);
}

ts::script::Module::Module(Engine* engine)
: engine_(engine)
{
    Stack_guard stack_guard(vm_);

    sq_setforeignptr(vm_, this);
    sq_setprintfunc(vm_, printf_console<Error_level::none>, printf_console<Error_level::runtime_error>);

    sq_newclosure(vm_, error_handler, 0);
    sq_seterrorhandler(vm_);
    sq_setcompilererrorhandler(vm_, compile_error_handler);

    sq_pushroottable(vm_);
    sqstd_register_mathlib(vm_);
    sqstd_register_stringlib(vm_);
}

void ts::script::Module::register_api(const API_definition& api_definition)
{
    Stack_guard stack_guard(vm_);

    sq_pushroottable(vm_);
    for (const auto& func_def : api_definition.static_functions)
    {
        sq_pushstring(vm_, func_def.name, -1);
        sq_newclosure(vm_, func_def.function, 0);
        
        sq_newslot(vm_, -3, SQTrue);
    }

    for (const auto& delegate : api_definition.delegates)
    {
        create_delegate_table(vm_, delegate);
    }

    for (auto interface : api_definition.interfaces)
    {
        script::register_interface(vm_, interface);
    }

    for (const auto& class_def : api_definition.classes)
    {
        register_class_definition(vm_, class_def);
    }

    for (const auto& callback : api_definition.launch_callbacks)
    {
        callback(vm_);
    }
}

void ts::script::Module::report_error(const utf8_string& error_message, Error_level error_level)
{
    engine_->write_console_line(error_message, error_level);
}

ts::script::Engine* ts::script::Module::engine()
{
    return engine_;
}

void ts::script::Module::load_file(const utf8_string& file_name)
{
    boost::filesystem::ifstream stream(file_name.string(), std::ios::in);
    if (stream)
    {
        auto file_contents = core::read_stream_contents(stream);
        Stack_guard stack_guard(vm_);

        auto result = sq_compilebuffer(vm_, file_contents.data(), file_contents.size(), file_name.string().c_str(), SQTrue);
        if (SQ_SUCCEEDED(result))
        {
            Function script(vm_, -1);
            execution_list_.push_back(std::move(script));
        }

        else
        {
            // Compilation failed.
        }
    }

    else
    {
        // Reading from the file failed.
    }
}

void ts::script::Module::execute(const std::vector<API_definition>& api_definitions)
{
    auto scripts = std::move(execution_list_);

    for (const auto& api : api_definitions)
    {
        register_api(api);
    }

    for (const auto& script : scripts)
    {
        script(get_root_table(vm_));
    }
}

ts::script::Event_handler ts::script::Module::add_event_handler(const utf8_string& event_name, Function function)
{
    return event_manager_.register_event_handler(event_name, function);
}

void ts::script::Module::remove_event_handler(const Event_handler& event_handler)
{
    event_manager_.remove_event_handler(event_handler);
}

SQInteger ts::script::error_handler(HSQUIRRELVM vm)
{
    const SQChar* error_message;
    sq_getstring(vm, 2, &error_message);
    
    auto engine = get_engine_by_vm(vm);
    engine->write_console_line(error_message, Error_level::runtime_error);

    return 0;
}

void ts::script::compile_error_handler(HSQUIRRELVM vm, const SQChar* desc, const SQChar* source, SQInteger line, SQInteger column)
{
    printf_console<Error_level::syntax_error>(vm, "%s in %s (line %d, column %d)", desc, source, line, column);
}