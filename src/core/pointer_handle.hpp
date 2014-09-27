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

#ifndef CORE_POINTER_HANDLE_HPP
#define CORE_POINTER_HANDLE_HPP

namespace ts
{
    template <typename T>
    struct Pointer_handle
    {
    public:
        explicit Pointer_handle(T* ptr = nullptr);

        T& operator*() const;
        T* operator->() const;

        explicit operator bool() const;

        Pointer_handle<T>& operator=(T* ptr);

        T* get() const;
        void set(T*);

    private:
        T* ptr_;
    };

    template <typename T, typename U>
    bool operator==(Pointer_handle<T> lhs, Pointer_handle<U> rhs);

    template <typename T, typename U>
    bool operator!=(Pointer_handle<T> lhs, Pointer_handle<U> rhs);

    template <typename T>
    bool operator==(Pointer_handle<T> lhs, std::nullptr_t rhs);

    template <typename T>
    bool operator==(std::nullptr_t lhs, Pointer_handle<T> rhs);

    template <typename T>
    bool operator!=(Pointer_handle<T> lhs, std::nullptr_t rhs);

    template <typename T>
    bool operator!=(std::nullptr_t lhs, Pointer_handle<T> rhs);

    template <>
    struct Pointer_handle<void>
    {
        Pointer_handle(void* ptr = nullptr);

        explicit operator bool() const;
        Pointer_handle<void>& operator=(void* ptr);

        void* get() const;
        void set(void*);

    private:
        void* ptr_;
    };

}

template <typename T>
ts::Pointer_handle<T>::Pointer_handle(T* ptr)
: ptr_(ptr)
{
}

template <typename T>
T& ts::Pointer_handle<T>::operator*() const
{
    return *ptr_;
}

template <typename T>
T* ts::Pointer_handle<T>::operator->() const
{
    return ptr_;
}

template <typename T>
ts::Pointer_handle<T>::operator bool() const
{
    return ptr_ != nullptr;
}

template <typename T>
ts::Pointer_handle<T>& ts::Pointer_handle<T>::operator=(T* ptr)
{
    ptr_ = ptr;
    return *this;
}

template <typename T>
void ts::Pointer_handle<T>::set(T* ptr)
{
    ptr_ = ptr;
}

template <typename T>
T* ts::Pointer_handle<T>::get() const
{
    return ptr_;
}

inline ts::Pointer_handle<void>::Pointer_handle(void* ptr)
: ptr_(ptr)
{
}

inline ts::Pointer_handle<void>::operator bool() const
{
    return ptr_ != nullptr;
}

inline ts::Pointer_handle<void>& ts::Pointer_handle<void>::operator=(void* ptr)
{
    ptr_ = ptr;
    return *this;
}

inline void ts::Pointer_handle<void>::set(void* ptr)
{
    ptr_ = ptr;
}

inline void* ts::Pointer_handle<void>::get() const
{
    return ptr_;
}

template <typename T, typename U>
bool ts::operator==(Pointer_handle<T> lhs, Pointer_handle<U> rhs)
{
    return lhs.get() == rhs.get();
}

template <typename T, typename U>
bool ts::operator!=(Pointer_handle<T> lhs, Pointer_handle<U> rhs)
{
    return lhs.get() != rhs.get();
}

template <typename T>
bool ts::operator==(Pointer_handle<T> lhs, std::nullptr_t rhs)
{
    lhs.get() == rhs;
}

template <typename T>
bool ts::operator!=(Pointer_handle<T> lhs, std::nullptr_t rhs)
{
    lhs.get() != rhs;
}

template <typename T>
bool ts::operator==(std::nullptr_t lhs, Pointer_handle<T> rhs)
{
    lhs == rhs.get();
}

template <typename T>
bool ts::operator!=(std::nullptr_t rhs, Pointer_handle<T> lhs)
{
    lhs == rhs.get();
}

#endif