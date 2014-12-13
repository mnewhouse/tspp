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

#ifndef CORE_UTILITY_HPP
#define CORE_UTILITY_HPP

namespace ts
{

    typedef void* type_id_type;

    template <typename T>
    type_id_type type_id()
    {
        static int id;
        return type_id_type(&id);
    }

    template <std::size_t N, typename T>
    std::size_t array_size(const T(&)[N])
    {
        return N;
    }

    struct Generic_scope_exit
    {
        explicit Generic_scope_exit(std::function<void()> func = {})
        : func_(std::move(func))
        {
        }

        Generic_scope_exit(const Generic_scope_exit&) = delete;
        Generic_scope_exit& operator=(const Generic_scope_exit&) = delete;

        Generic_scope_exit(Generic_scope_exit&& other)
            : func_(std::move(other.func_))
        {
        }

        Generic_scope_exit& operator=(Generic_scope_exit&& rhs)
        {
            if (func_)
            {
                func_();
            }

            func_ = std::move(rhs.func_);
            rhs.func_ = nullptr;

            return *this;
        }        

        ~Generic_scope_exit()
        {
            if (func_)
            {
                func_();
            }            
        }

        explicit operator bool() const
        {
            return func_ != nullptr;
        }

    private:
        std::function<void()> func_;
    };

    template <typename F>
    Generic_scope_exit generic_scope_exit(F func)
    {
        return Generic_scope_exit(func);
    }

    template <typename T>
    T clamp(T value, T min, T max)
    {
        if (max < min) {
            std::swap(min, max);
        }

        return std::min(std::max(value, min), max);
    }

    template <typename IteratorType>
    struct Range
    {
        Range(IteratorType begin, IteratorType end)
            : begin_(begin), end_(end)
        {}

        IteratorType begin() const { return begin_; }
        IteratorType end() const { return end_; }

    private:
        IteratorType begin_, end_;
    };

    template <typename IteratorType>
    Range<IteratorType> make_range(IteratorType begin, IteratorType end)
    {
        return Range<IteratorType>(begin, end);
    }
}

#endif