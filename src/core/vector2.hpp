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

#ifndef VECTOR2_HPP
#define VECTOR2_HPP

#include <cmath>
#include <cstdint>
#include <istream>

namespace ts
{
    template <typename T>
    struct Vector2
    {
    public:
        Vector2()
            : x(), y()
        {}

        Vector2(T x, T y)
            : x(x), y(y)
        {}

        template <typename U>
        Vector2(Vector2<U> other)
            : x(static_cast<T>(other.x)), y(static_cast<T>(other.y))
        {}

        template <typename U>
        Vector2<T>& operator=(Vector2<U> other)
        {
            x = static_cast<T>(other.x);
            y = static_cast<T>(other.y);
            return *this;
        }


        Vector2<T>& operator+=(Vector2<T> other)
        {
            x += other.x;
            y += other.y;
            return *this;
        }

        Vector2<T>& operator+=(T num)
        {
            x += num;
            y += num;
            return *this;
        }

        Vector2<T>& operator-=(Vector2<T> other)
        {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        Vector2<T>& operator-=(T num)
        {
            x -= num;
            y -= num;
            return *this;
        }

        Vector2<T>& operator*=(Vector2<T> other)
        {
            x *= other.x;
            y *= other.y;
            return *this;
        }

        Vector2<T>& operator*=(T num)
        {
            x *= num;
            y *= num;
            return *this;
        }

        Vector2<T>& operator/=(Vector2<T> other)
        {
            x /= other.x;
            y /= other.y;
            return *this;
        }


        Vector2<T>& operator/=(T num)
        {
            x /= num;
            y /= num;
            return *this;
        }

        T x;
        T y;
    };

    template <typename T>
    Vector2<T> operator+(Vector2<T> a, Vector2<T> b)
    {
        return a += b;
    }

    template <typename T>
    Vector2<T> operator-(Vector2<T> a, Vector2<T> b)
    {
        return a -= b;
    }

    template <typename T>
    Vector2<T> operator*(Vector2<T> a, Vector2<T> b)
    {
        return a *= b;
    }

    template <typename T>
    Vector2<T> operator/(Vector2<T> a, Vector2<T> b)
    {
        return a /= b;
    }

    template <typename T>
    Vector2<T> operator+(Vector2<T> vec, T num)
    {
        return vec += num;
    }

    template <typename T>
    Vector2<T> operator+(T num, Vector2<T> vec)
    {
        return vec += num;
    }

    template <typename T>
    Vector2<T> operator-(Vector2<T> vec, T num)
    {
        return vec -= num;
    }

    template <typename T>
    Vector2<T> operator-(T num, Vector2<T> vec)
    {
        return vec -= num;
    }

    template <typename T>
    Vector2<T> operator*(Vector2<T> vec, T num)
    {
        return vec *= num;
    }

    template <typename T>
    Vector2<T> operator*(T num, Vector2<T> vec)
    {
        return vec *= num;
    }

    template <typename T>
    Vector2<T> operator/(Vector2<T> vec, T num)
    {
        return vec /= num;
    }

    template <typename T>
    Vector2<T> operator/(T num, Vector2<T> vec)
    {
        return vec /= num;
    }

    template <typename T>
    Vector2<T> operator-(Vector2<T> vec)
    {
        vec.x = -vec.x;
        vec.y = -vec.y;
        return vec;
    }

    template <typename T, typename U>
    bool operator==(Vector2<T> a, Vector2<U> b)
    {
        return a.x == b.x && a.y == b.y;
    }

    template <typename T, typename U>
    bool operator!=(Vector2<T> a, Vector2<U> b)
    {
        return !(a == b);
    }

    template <typename T>
    T magnitude(Vector2<T> vec)
    {
        if (vec.x == 0.0f) return std::abs(vec.y);
        if (vec.y == 0.0f) return std::abs(vec.x);

        return std::hypot(vec.x, vec.y);
    }

    template <typename T, typename U>
    decltype(std::declval<T>() * std::declval<U>()) dot_product(Vector2<T> a, Vector2<U> b)
    {
        return a.x * b.x + a.y * b.y;
    }

    using Vector2u = Vector2<std::uint32_t>;
    using Vector2i = Vector2<std::int32_t>;
    using Vector2f = Vector2<float>;
    using Vector2d = Vector2<double>;

    template <typename T>
    Vector2<T> normalize(Vector2<T> vec)
    {
        auto mag = magnitude(vec);
        if (mag == 0) return vec;

        return vec /= mag;
    }

    template <typename T>
    std::istream& operator>>(std::istream& stream, Vector2<T>& vec)
    {
        return stream >> vec.x >> vec.y;
    }

    template <typename T>
    std::ostream& operator<<(std::ostream& stream, Vector2<T> vec)
    {
        return stream << vec.x << ", " << vec.y;
    }

}

#endif