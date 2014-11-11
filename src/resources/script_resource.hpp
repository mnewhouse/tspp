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

#ifndef RESOURCES_SCRIPT_RESOURCE_HPP
#define RESOURCES_SCRIPT_RESOURCE_HPP

namespace ts
{
    namespace resources
    {
        static const char* const config_file_name = "script.cfg";

        struct Resource_config_exception
            : public std::runtime_error
        {
            Resource_config_exception(const utf8_string& resource_name);
        };

        class Script_resource
        {
        public:
            Script_resource(utf8_string root_directory, utf8_string name);

            const std::vector<utf8_string>& client_scripts() const;
            const std::vector<utf8_string>& server_scripts() const;
            const std::vector<utf8_string>& cup_scripts() const;

            const utf8_string& name() const;

        private:
            void load_resource_config();
            void load_resource_config(std::istream& stream);


            utf8_string resource_name_;
            utf8_string root_directory_;

            std::vector<utf8_string> client_script_files_;
            std::vector<utf8_string> server_script_files_;
            std::vector<utf8_string> cup_script_files_;
        };

        using Script_handle = Pointer_handle<const Script_resource>;
    }
}

#endif
