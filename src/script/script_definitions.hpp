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

#ifndef SCRIPT_DEFINITIONS_HPP
#define SCRIPT_DEFINITIONS_HPP

#include "script_engine.hpp"

namespace ts
{
    namespace world
    {
        class World;
    }

    namespace controls
    {
        class Control_center;
    }

    namespace script
    {
        class HUD_overlay;
        class World_interface;
        class Control_point_interface;

        namespace decl
        {
            const char* const startGameTimer = "void startGameTimer()";
            const char* const getGameTime = "uint64 getGameTime()";

            const char* const IWorldListener = "IWorldListener";
            const char* const addWorldListener = "void addEventListener(IWorldListener@)";
            const char* const removeWorldListener = "void removeEventListener(IWorldListener@)";
            const char* const IWorldListener_onStart = "void onStart()";
            const char* const IWorldListener_onTick = "void onTick(uint64)";
            const char* const IWorldListener_onUpdate = "void onUpdate()";

            const char* const CoreCode_WorldListener = "class WorldListener : IWorldListener { void onStart() {} void onUpdate() {} void onTick(uint64) {} }";

            const char* const ControlPoint = "ControlPoint";
            const char* const ControlPoint_getId = "int32 getId() const";

            const char* const IControlPointListener = "IControlPointListener";
            const char* const IControlPointListener_onControlPointHit = "void onControlPointHit(Entity@, ControlPoint@, double)";
            const char* const IControlPointListener_onControlAreaEnter = "void onControlAreaEnter(Entity@, ControlPoint@, double)";
            const char* const IControlPointListener_onControlAreaLeave = "void onControlAreaLeave(Entity@, ControlPoint@, double)";

            const char* const CoreCode_ControlPointListener = "class ControlPointListener : IControlPointListener { void onControlPointHit(Entity@, ControlPoint@, double) {} void onControlAreaEnter(Entity@, ControlPoint@, double) {} void onControlAreaLeave(Entity@, ControlPoint@, double) {} }";

            const char* const removeControlPointListener = "void removeEventListener(IControlPointListener@)";
            const char* const addControlPointListener = "void addEventListener(IControlPointListener@)";

            const char* const getControlPointById = "ControlPoint@ getControlPointById(int32)";
            const char* const getControlPointCount = "uint32 getControlPointCount()";

            const char* const Entity_setControlPoint = "void setControlPoint(ControlPoint@)";
            const char* const Entity_ignoreControlPoints = "void ignoreControlPoints()";
        }

        void register_utility_definitions(Engine* engine);

        void register_world_definitions(Engine* engine, World_interface* world_interface);
        void register_control_point_definitions(Engine* engine, Control_point_interface* cp_interface);

        void register_control_definitions(Engine* engine, controls::Control_center* control_center);

        void register_display_definitions(Engine* engine, HUD_overlay* hud);
    }
}

#endif