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
 * GNU General Public License for more details
. *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef CORE_ROTATION_HPP
#define CORE_ROTATION_HPP

#include <iostream>
#include <cmath>
 
namespace ts
{

    template <typename T>
    class Rotation
    {
    public:
        Rotation();

        static Rotation<T> degrees(T degrees);
        static Rotation<T> radians(T radians);

        template <typename U>
        Rotation<T>& operator+=(Rotation<U> right);

        template <typename U>
        Rotation<T>& operator-=(Rotation<U> right);

        Rotation<T> operator-() const;

        T degrees() const;
        T radians() const;

    private:
        void normalize();

        Rotation(T radians)
            : radians_(radians)
        {
            normalize();
        }

        T radians_;
        static const long double pi;
    };

    template <typename T>
    const long double Rotation<T>::pi = 3.141592653589793238462643383279502884197169399375105820974944;

    template <typename T>
    Rotation<T>::Rotation()
        : radians_(0)
    {
    }

    
    template <typename T>
    void Rotation<T>::normalize()
    {
        const auto range = T(pi * 2.0);

        while (radians_ >= pi) radians_ -= range;
        while (radians_ < -pi) radians_ += range;
    }

    template <typename T>
    Rotation<T> operator+(Rotation<T> left, Rotation<T> right)
    {
        return left += right;
    }

    template <typename T>
    Rotation<T> operator-(Rotation<T> left, Rotation<T> right)
    {
        return left -= right;
    }

    template <typename T>
    Rotation<T> Rotation<T>::operator-() const
    {
        return Rotation<T>(radians() - pi);
    }


    template <typename T>
    T Rotation<T>::degrees() const
    {
        return radians_ * T(57.295779513082);
    }

    template <typename T>
    T Rotation<T>::radians() const
    {
        return radians_;
    }

    template <typename T>
    Rotation<T> Rotation<T>::degrees(T value)
    {
        return Rotation<T>(value / T(57.295779513082));
    }

    template <typename T>
    Rotation<T> Rotation<T>::radians(T value)
    {
        return Rotation<T>(value);
    }

    template <typename T>
    template <typename U>
    Rotation<T>& Rotation<T>::operator+=(Rotation<U> rotation)
    {
        radians_ += rotation.radians();
        normalize();

        return *this;
    }

    template <typename T>
    template <typename U>
    Rotation<T>& Rotation<T>::operator-=(Rotation<U> rotation)
    {
        radians_ -= rotation.radians();
        normalize();

        return *this;
    }


    template <typename T>
    inline std::istream& operator>>(std::istream& stream, Rotation<T>& rotation)
    {
        T degrees;
        if (stream >> degrees) {
            rotation = Rotation<T>::degrees(degrees);
        }

        else {
            stream.setstate(std::istream::failbit);
        }

        return stream;
    }

}

#endif