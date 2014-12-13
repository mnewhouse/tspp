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
#include "script_stage_modifier.hpp"
#include "server_script_engine.hpp"
#include "server_script_events.hpp"

#include "script/userdata_forwarder.hpp"
#include "script/script_engine.hpp"
#include "script/script_module.hpp"


ts::server::script_api::Stage_modifier::Stage_modifier(const Script_engine* script_engine)
: engine_(script_engine)
{
}

void ts::server::script_api::Stage_modifier::operator()(Stage_assembler& stage_assembler) const
{
    engine_->trigger_event_generic(script_events::onStagePreinitialize, nullptr, script::forward_as_userdata(&stage_assembler));
}