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

#ifndef CORE_HANDLE_HPP
#define CORE_HANDLE_HPP

#include <memory>
#include <utility>

namespace ts
{
    // Handle
    // This class is what its name says, a handle to something. Under the hood it's
    // just a shared pointer, meaning ownership is implied.
    template <typename T>
    class Handle
    {
    public:
        Handle()
            : handle_(std::make_shared<T>())
        {
        }

        explicit Handle(typename std::enable_if<std::is_rvalue_reference<T&&>::value, T&&>::type obj)
            : handle_(std::make_shared<T>(std::move(obj)))
        {
        }              

        bool operator==(const Handle<T>& other) const { return handle_ == other.handle_; }
        bool operator!=(const Handle<T>& other) const { return handle_ != other.handle_; }

        bool operator<(const Handle<T>& other) const { return handle_ < other.handle_; }
        bool operator>(const Handle<T>& other) const { return handle_ > other.handle_; }

        bool operator>=(const Handle<T>& other) const { return handle_ >= other.handle_; }
        bool operator<=(const Handle<T>& other) const { return handle_ <= other.handle_; }


        operator bool() const { return handle_; }
        T* operator->() const { return &*handle_; }
        T& operator*() const { return *handle_; }

    private:
        std::shared_ptr<T> handle_;
    };

}

#endif