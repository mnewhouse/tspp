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
#include "common_api_registration.hpp"

#include "script/script_engine.hpp"

#include "common/utility_classes.hpp"
#include "common/clock_api.hpp"
#include "common/event_api.hpp"
#include "common/color_api.hpp"
#include "common/player_color_api.hpp"
#include "common/stage_api.hpp"
#include "common/entity_api.hpp"
#include "common/message_api.hpp"

void ts::script_api::register_core_api(script::Engine* engine)
{
    engine->register_api(script_api::utility_classes());
    engine->register_api(script_api::player_color_api());
    engine->register_api(script_api::color_api());
    engine->register_api(script_api::clock_api());
}

void ts::script_api::register_event_api(script::Engine* engine, const script::Event_interface* event_interface)
{
    engine->register_api(script_api::event_api(event_interface));
}

void ts::script_api::register_stage_api(script::Engine* engine, const action::Stage_interface* stage_interface)
{
    engine->register_api(script_api::stage_api(stage_interface));
    engine->register_api(script_api::entity_api());
}

void ts::script_api::register_stdout_console(script::Engine* engine)
{
    engine->register_console(script::Stdout_console());
}