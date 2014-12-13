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
#include "server_script_engine.hpp"

#include "script/script_engine.hpp"
#include "script/script_module.hpp"

struct ts::server::Script_engine::Impl
{
    script::Engine cup_engine;
    script::Engine stage_engine;
};

ts::server::Script_engine::Script_engine()
: impl_(std::make_unique<Impl>())
{
}

ts::server::Script_engine::~Script_engine()
{
}

void ts::server::Script_engine::trigger_event(const utf8_string& event, const script::Value& source, const std::vector<script::Value>& arguments) const
{
    cup_engine()->trigger_event(event, source, arguments);
    stage_engine()->trigger_event(event, source, arguments);
}

ts::script::Engine* ts::server::Script_engine::cup_engine()
{
    return &impl_->cup_engine;
}

const ts::script::Engine* ts::server::Script_engine::cup_engine() const
{
    return &impl_->cup_engine;
}

ts::script::Engine* ts::server::Script_engine::stage_engine()
{
    return &impl_->stage_engine;
}

const ts::script::Engine* ts::server::Script_engine::stage_engine() const
{
    return &impl_->stage_engine;
}