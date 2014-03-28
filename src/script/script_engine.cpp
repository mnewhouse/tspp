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

#include "script_engine.hpp"

#include "scriptstdstring/scriptstdstring.h"
#include "scriptarray/scriptarray.h"
#include "scriptmath/scriptmath.h"
#include "scriptbuilder/scriptbuilder.h"

#include <iostream>
#include <cstdint>

// Implement a simple message callback function
static void MessageCallback(const asSMessageInfo *msg, void *param)
{
    const char *type = "ERR";
    if (msg->type == asMSGTYPE_WARNING)
        type = "WARN";
    else if (msg->type == asMSGTYPE_INFORMATION)
        type = "INFO";

    std::cout << msg->section << "(" << msg->row << ", " << msg->col << ") : " << msg->type << ": " << msg->message << std::endl;
}

template <typename T>
static void print_line(T arg)
{
    std::cout << arg << std::endl;
}

ts::script::Module_handle::Module_handle(Engine* engine, std::string module_name)
: script_engine_(engine),
  module_name_(std::move(module_name)),
  module_handle_(engine->engine()->GetModule(module_name_.c_str()))
{
}

ts::script::Module_handle::operator bool() const
{
    return module_handle_ != nullptr;
}

void ts::script::Module_handle::run()
{
    auto func = module_handle_->GetFunctionByDecl("void main()");
    if (!func) throw No_main_error();

    script_engine_->call_function(func, nullptr);
}

ts::script::Engine::Engine()
   : script_engine_(asCreateScriptEngine(ANGELSCRIPT_VERSION))
{
    RegisterStdString(script_engine_);
    RegisterScriptArray(script_engine_, true);
    RegisterScriptMath(script_engine_);

    script_engine_->SetMessageCallback(asFUNCTION(MessageCallback), nullptr, asCALL_CDECL);

    script_engine_->RegisterGlobalFunction("void print(const string& in)", asFUNCTION(print_line<const std::string&>), asCALL_CDECL);
    script_engine_->RegisterGlobalFunction("void print(double)", asFUNCTION(print_line<double>), asCALL_CDECL);
    script_engine_->RegisterGlobalFunction("void print(int)", asFUNCTION(print_line<std::int32_t>), asCALL_CDECL);

    script_context_ = script_engine_->CreateContext();
}

void ts::script::Engine::append_core_code(const std::string& code)
{
    core_code_ += code;
    core_code_ += "\n";
}

ts::script::Module_handle ts::script::Engine::load_module(const std::string& module_name, const std::string& script_file)
{
    CScriptBuilder script_builder;
    auto r = script_builder.StartNewModule(script_engine_, module_name.c_str());
    if (r < 0) throw Unrecoverable_error();

    r = script_builder.AddSectionFromFile(script_file.c_str());
    if (r < 0) throw File_not_found_error();

    r = script_builder.AddSectionFromMemory("__core", core_code_.data(), core_code_.size());

    r = script_builder.BuildModule();
    if (r < 0) {
        throw Load_error();
    }

    return Module_handle(this, module_name);
}

asIScriptEngine* ts::script::Engine::engine() const
{
    return script_engine_;
}

asIScriptContext* ts::script::Engine::context() const
{
    return script_context_;
}


ts::script::Engine::~Engine()
{
    script_context_->Release();
    script_engine_->Release();
}
