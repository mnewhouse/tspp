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

#ifndef RESOURCES_SETTINGS_COPY_HPP
#define RESOURCES_SETTINGS_COPY_HPP

namespace ts
{
    namespace resources
    {
        template <typename SettingsType>
        struct Settings_copy
        {
            Settings_copy(SettingsType* settings);
            ~Settings_copy();

            Settings_copy(const Settings_copy<SettingsType>&) = delete;
            Settings_copy<SettingsType>& operator=(const Settings_copy<SettingsType>&) = delete;

            SettingsType& operator*();
            const SettingsType& operator*() const;

            SettingsType* operator->();
            const SettingsType* operator->() const;

            void save();
            void load();

        private:
            SettingsType settings_;
            SettingsType* settings_ptr_;
        };
    }
}

template <typename SettingsType>
ts::resources::Settings_copy<SettingsType>::Settings_copy(SettingsType* settings)
: settings_(*settings),
  settings_ptr_(settings)
{
}

template <typename SettingsType>
ts::resources::Settings_copy<SettingsType>::~Settings_copy()
{
    *settings_ptr_ = settings_;
}

template <typename SettingsType>
SettingsType& ts::resources::Settings_copy<SettingsType>::operator*()
{
    return settings_;
}

template <typename SettingsType>
const SettingsType& ts::resources::Settings_copy<SettingsType>::operator*() const
{
    return settings_;
}

template <typename SettingsType>
SettingsType* ts::resources::Settings_copy<SettingsType>::operator->()
{
    return &settings_;
}

template <typename SettingsType>
const SettingsType* ts::resources::Settings_copy<SettingsType>::operator->() const
{
    return &settings_;
}

template <typename SettingsType>
void ts::resources::Settings_copy<SettingsType>::save()
{
    *settings_ptr_ = settings_;
}

template <typename SettingsType>
void ts::resources::Settings_copy<SettingsType>::load()
{
    settings_ = *settings_ptr_;
}

#endif
