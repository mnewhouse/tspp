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

#ifndef SCRIPT_CONSOLE_HPP
#define SCRIPT_CONSOLE_HPP

#include "error_level.hpp"

namespace ts
{
    namespace script
    {
        namespace impl
        {
            struct Console_base
            {
                virtual void write_line_(const utf8_string& string, Error_level error_level) = 0;
                virtual std::unique_ptr<Console_base> clone_() const = 0;
            };

            template <typename ConcreteType>
            struct Console_model
                : public Console_base
            {
                Console_model(ConcreteType console);

                virtual void write_line_(const utf8_string& string, Error_level error_level) override;
                virtual std::unique_ptr<Console_base> clone_() const override;

                ConcreteType console_;
            };
        }

        class Console
        {
        public:
            template <typename ConsoleType>
            Console(ConsoleType console);

            Console(const Console& other);
            Console(Console&& other);

            Console& operator=(const Console& rhs);
            Console& operator=(Console&& rhs);

            void write_line(const utf8_string& string, Error_level error_level = Error_level::none);

        private:
            std::unique_ptr<impl::Console_base> self_;
        };

        struct Stdout_console
        {
        };

        void write_line(const Stdout_console&, const utf8_string&, Error_level error_level);
    }
}

template <typename ConsoleType>
ts::script::impl::Console_model<ConsoleType>::Console_model(ConsoleType console)
: console_(std::move(console))
{
}

template <typename ConsoleType>
void ts::script::impl::Console_model<ConsoleType>::write_line_(const utf8_string& string, Error_level error_level)
{
    write_line(console_, string, error_level);
}

template <typename ConsoleType>
std::unique_ptr<ts::script::impl::Console_base> ts::script::impl::Console_model<ConsoleType>::clone_() const
{
    return std::make_unique<Console_model<ConsoleType>>(console_);
}


template <typename ConsoleType>
ts::script::Console::Console(ConsoleType console)
: self_(std::make_unique<impl::Console_model<ConsoleType>>(std::move(console)))
{
}

#endif