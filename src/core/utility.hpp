/*
 * Turbo Sliders++
 * Copyright (C) 2013 Martin Newhouse
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

#ifndef CORE_UTILITY_HPP
#define CORE_UTILITY_HPP

#include <cstddef>

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


    template <typename F>
    struct Scope_exit
    {
        explicit Scope_exit(F func)
        : func_(func)
        {
        }

        ~Scope_exit()
        {
            func_();
        }

    private:
        F func_;
    };

    template <typename F>
    Scope_exit<F> scope_exit(F func)
    {
        return Scope_exit<F>(func);
    }

}

#endif