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

#ifndef SERVER_SCRIPT_ENGINE_HPP
#define SERVER_SCRIPT_ENGINE_HPP

#include "script/event_interface.hpp"

namespace ts
{
    namespace script
    {
        class Engine;
    }

    namespace server
    {
        class Script_engine
            : public script::Event_interface
        {
        public:
            Script_engine();
            ~Script_engine();

            virtual void trigger_event(const utf8_string& event, const script::Value& source, const std::vector<script::Value>& arguments) const override;

            script::Engine* cup_engine();
            const script::Engine* cup_engine() const;

            script::Engine* stage_engine();
            const script::Engine* stage_engine() const;

            template <typename... Args>
            void trigger_event_generic(const utf8_string& event, Args&&... args) const
            {
                cup_engine()->trigger_event(event, std::forward<Args>(args)...);
                stage_engine()->trigger_event(event, std::forward<Args>(args)...);
            }

        private:
            struct Impl;
            std::unique_ptr<Impl> impl_;
        };
    }
}

#endif