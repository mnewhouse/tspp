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

#ifndef UTF8_STRING_HPP
#define UTF8_STRING_HPP

#include <string>
#include "utf8.h"

namespace ts
{
    class utf8_string;
   
    class utf8_string_view
    {
    public:
        utf8_string_view();
        template <std::size_t N>
        utf8_string_view(const char(&data)[N]);

        utf8_string_view(const char* begin, const char* end);

        using const_iterator = utf8::iterator<const char*>;        

        const_iterator begin() const;
        const_iterator end() const;

        const char* data() const;
        std::size_t size() const;
        bool empty() const;        
        
    private:
        const char* begin_;
        const char* end_;
    };

    class utf8_string
    {
    public:
        utf8_string() = default;

        explicit utf8_string(utf8::uint32_t code_point);
        utf8_string(std::string data);
        utf8_string(const utf8_string_view& string_view);

        utf8_string(const utf8_string& other) = default;
        utf8_string(utf8_string&& other); // Stupid MSVC doesn't support default move operations yet

        template <std::size_t N>
        utf8_string(const char(&data)[N]);

        utf8_string(const char* data);

        utf8_string(const char* data, std::size_t data_size);

        template <typename OctetIt>
        utf8_string(OctetIt it, OctetIt end);

        template <typename OctetIt>
        utf8_string(utf8::iterator<OctetIt> it, utf8::iterator<OctetIt> end);

        template <std::size_t N>
        utf8_string& operator=(const char(&data)[N]);

        utf8_string& operator=(const utf8_string& rhs) = default;
        utf8_string& operator=(utf8_string&& rhs);

        utf8_string& operator=(std::string rhs);

        bool valid() const;

        using iterator = utf8::iterator<std::string::const_iterator>;
        using const_iterator = iterator;
        using value_type = iterator::value_type;

        iterator begin() const;
        iterator end() const;

        utf8_string& operator+=(const utf8_string& rhs);
        utf8_string& operator+=(utf8::uint32_t code_point);

        void append(utf8::uint32_t code_point);
        void append(const utf8_string& rhs);

        const std::string& string() const;
        const char* data() const;
        //std::size_t size() const;
        std::size_t count() const;
        std::size_t length() const;
        std::size_t size() const;

        void resize(std::size_t new_size);

        bool empty() const;
        void clear();



        void pop_back();
        void push_back(utf8::uint32_t code_point);

        iterator erase(iterator it, iterator end);
        iterator erase(iterator it);

        iterator insert(iterator where, utf8::uint32_t code_point);
        iterator insert(iterator where, const utf8_string& string);

    private:
        std::string string_data_;
    };

    // Concatenation
    utf8_string operator+(utf8_string lhs, const utf8_string& rhs);

    // Relational operators
    bool operator==(const utf8_string& lhs, const utf8_string& rhs);
    bool operator!=(const utf8_string& lhs, const utf8_string& rhs);
    bool operator<(const utf8_string& lhs, const utf8_string& rhs);
    bool operator>(const utf8_string& lhs, const utf8_string& rhs);
    bool operator<=(const utf8_string& lhs, const utf8_string& rhs);
    bool operator>=(const utf8_string& lhs, const utf8_string& rhs);

    bool operator==(const utf8_string_view& lhs, const utf8_string_view& rhs);
    bool operator!=(const utf8_string_view& lhs, const utf8_string_view& rhs);
    bool operator<(const utf8_string_view& lhs, const utf8_string_view& rhs);
    bool operator>(const utf8_string_view& lhs, const utf8_string_view& rhs);
    bool operator<=(const utf8_string_view& lhs, const utf8_string_view& rhs);
    bool operator>=(const utf8_string_view& lhs, const utf8_string_view& rhs);

    bool operator==(const utf8_string& lhs, const utf8_string_view& rhs);
    bool operator!=(const utf8_string& lhs, const utf8_string_view& rhs);
    bool operator<(const utf8_string& lhs, const utf8_string_view& rhs);
    bool operator>(const utf8_string& lhs, const utf8_string_view& rhs);
    bool operator<=(const utf8_string& lhs, const utf8_string_view& rhs);
    bool operator>=(const utf8_string& lhs, const utf8_string_view& rhs);

    bool operator==(const utf8_string_view& lhs, const utf8_string& rhs);
    bool operator!=(const utf8_string_view& lhs, const utf8_string& rhs);
    bool operator<(const utf8_string_view& lhs, const utf8_string& rhs);
    bool operator>(const utf8_string_view& lhs, const utf8_string& rhs);
    bool operator<=(const utf8_string_view& lhs, const utf8_string& rhs);
    bool operator>=(const utf8_string_view& lhs, const utf8_string& rhs);

    // Free begin/end overloads
    utf8_string::iterator begin(const utf8_string& string);
    utf8_string::iterator end(const utf8_string& string);

    std::ostream& operator<<(std::ostream& stream, const utf8_string& string);
    std::istream& operator>>(std::istream& stream, utf8_string& string);

    utf8_string to_lower(const utf8_string& string);
    utf8_string to_upper(const utf8_string& string);
}

namespace std
{
    // Hash specialization, for use in std::unordered_map and such

    template <>
    struct hash<ts::utf8_string>
    {
        using argument_type = ts::utf8_string;
        using result_type = std::size_t;

        result_type operator()(const argument_type& arg) const;
    };
}

template <std::size_t N>
ts::utf8_string_view::utf8_string_view(const char(&data)[N])
: begin_(data), end_(data + N - 1)
{
    if (*end_ != 0)
    {
        ++end_;
    }
}

template <std::size_t N>
ts::utf8_string::utf8_string(const char(&data)[N])
: string_data_(std::begin(data), std::end(data))
{
    if (string_data_.back() == 0)
    {
        // Pop a terminating null byte, if it's there
        string_data_.pop_back();
    }
}

template <typename OctetIt>
ts::utf8_string::utf8_string(OctetIt it, OctetIt end)
: string_data_(it, end)
{
}

template <typename OctetIt>
ts::utf8_string::utf8_string(utf8::iterator<OctetIt> it, utf8::iterator<OctetIt> end)
{
    auto out = std::back_inserter(string_data_);

    while (it != end)
    {
        utf8::append(*it++, out);
    }
}

template <std::size_t N>
ts::utf8_string& ts::utf8_string::operator=(const char(&data)[N])
{
    string_data_.assign(std::begin(data), std::end(data));
    if (string_data_.back() == 0)
    {
        // Pop a terminating null byte, if it's there
        string_data_.pop_back();
    }

    return *this;
}

#endif