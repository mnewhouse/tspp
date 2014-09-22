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

#ifndef CORE_SETTINGS_DICTIONARY_HPP
#define CORE_SETTINGS_DICTIONARY_HPP

namespace ts
{
    namespace core
    {
        class Settings_value
        {
        public:
            enum Type
            {
                Empty,
                String,
                Integer,
                Floating_point
            };

            Settings_value()
                : type_(Empty)
            {
                numeric_value_.int_val = 0;
            }

            explicit Settings_value(std::string string_value)
                : type_(String), string_value_(std::move(string_value))
            {
            }

            template <typename T>
            explicit Settings_value(T value, typename std::enable_if<std::is_integral<T>::value || std::is_enum<T>::value, void*>::type = nullptr)
                : type_(Integer), string_value_(std::to_string(static_cast<integer_type>(value))), 
            {
                numeric_value_.int_val = static_cast<integer_type>(value);
            }
             
            template <typename T>
            explicit Settings_value(T value, typename std::enable_if<std::is_floating_point<T>::value, void*>::type = nullptr)
                : type_(Floating_point), string_value_(std::to_string(value))
            {
                numeric_value_.float_val = static_cast<float_type>(value);
            }

            Settings_value& operator=(std::string value)
            {
                type_ = String;
                string_value_ = std::move(value);
                return *this;
            }

            template <typename T>
            typename std::enable_if<std::is_integral<T>::value || std::is_enum<T>::value, Settings_value&>::type operator=(T value)
            {
                type_ = Integer;
                numeric_value_.int_val = static_cast<integer_type>(value);
                string_value_ = std::to_string(numeric_value_.int_val);
                return *this;
            }

            template <typename T>
            typename std::enable_if<std::is_floating_point<T>::value, Settings_value&>::type operator=(T value)
            {
                type_ = Floating_point;
                numeric_value_.float_val = static_cast<float_type>(value);
                string_value_ = std::to_string(value);
                return *this;
            }


            template <typename T>
            typename std::enable_if<std::is_same<typename std::decay<T>::type, std::string>::value, const std::string&>::type get() const
            {
                return string_value_;
            }

            template <typename T>
            typename std::enable_if<std::is_integral<T>::value || std::is_enum<T>::value, T>::type get() const
            {
                return static_cast<T>(numeric_value_.int_val);
            }

            template <typename T>
            typename std::enable_if<std::is_floating_point<T>::value, T>::type get() const
            {
                return static_cast<T>(numeric_.float_val);
            }
            
            Type type() const
            {
                return type_;
            }

        private:
            Type type_;
            std::string string_value_;

            using integer_type = std::int64_t;
            using float_type = double;

            union
            {
                integer_type int_val;
                float_type float_val;
            } numeric_value_;            
        };

        class Settings_dictionary;

        class Settings_proxy
        {
        public:
            Settings_proxy(Settings_dictionary* settings, std::string key)
                : settings_(settings), key_(std::move(key))
            {
            }

            template <typename T>
            Settings_proxy& operator=(const T& value);

            inline Settings_value& operator[](std::size_t index);

            template <typename T>
            auto get() -> decltype(std::declval<Settings_value>().get<T>());

        private:
            inline Settings_value& get_reference(std::size_t index);

            Settings_dictionary* settings_;
            std::string key_;
        };

        class Settings_dictionary
        {
        public:
            Settings_proxy operator[](std::string key)
            {
                return Settings_proxy(this, std::move(key));
            }

        private:
            friend Settings_proxy;
            std::map<std::pair<std::string, std::size_t>, Settings_value> settings_;
        };
    }
}

ts::core::Settings_value& ts::core::Settings_proxy::operator[](std::size_t index)
{
    return get_reference(index);
}

ts::core::Settings_value& ts::core::Settings_proxy::get_reference(std::size_t index)
{
    return settings_->settings_[std::make_pair(key_, index)];
}

template <typename T>
ts::core::Settings_proxy& ts::core::Settings_proxy::operator=(const T& value)
{
    get_reference(0) = value;
    return *this;
}

template <typename T>
auto ts::core::Settings_proxy::get() -> decltype(std::declval<Settings_value>().get<T>())
{
    return get_reference(0).get<T>();
}


#endif