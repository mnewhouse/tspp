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

#ifndef LOADING_INTERFACE_HPP
#define LOADING_INTERFACE_HPP

namespace ts
{
    namespace resources
    {
        class Loading_interface
        {
        public:
            Loading_interface(double max_progress = 1.0);

            double progress() const;
            double max_progress() const;

            bool finished() const;
            bool loading() const;

            virtual utf8_string progress_string() const = 0;

        protected:
            void set_progress(double progress);
            void set_max_progress(double max_progress);
            void set_finished(bool finished = true);
            void set_loading(bool loading = true);

        private:
            std::atomic<double> progress_ = 0.0;
            std::atomic<double> max_progress_ = 1.0;
            std::atomic<bool> finished_ = false;
            std::atomic<bool> loading_ = false;
        };
    }
}

#endif