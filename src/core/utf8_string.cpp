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

#include "stdinc.hpp"
#include "utf8_string.hpp"

ts::utf8_string::utf8_string(const utf8_string_view& string_view)
: string_data_(string_view.data(), string_view.size())
{
}

ts::utf8_string::utf8_string(utf8::uint32_t code_point)
{
    append(code_point);
}

ts::utf8_string::utf8_string(std::string data)
: string_data_(std::move(data))
{
}

ts::utf8_string::utf8_string(const char* data)
: string_data_(data)
{
}

ts::utf8_string::utf8_string(const char* data, std::size_t data_size)
: string_data_(data, data + data_size)
{
}

ts::utf8_string::utf8_string(utf8_string&& other)
: string_data_(std::move(other.string_data_))
{}

const std::string& ts::utf8_string::string() const
{
    return string_data_;
}

const char* ts::utf8_string::data() const
{
    return string_data_.data();
}

void ts::utf8_string::clear()
{
    string_data_.clear();
}

void ts::utf8_string::push_back(utf8::uint32_t code_point)
{
    append(code_point);
}

void ts::utf8_string::pop_back()
{
    auto it = string_data_.end();
    utf8::prior(it, string_data_.begin());

    string_data_.erase(it, string_data_.end());
}

void ts::utf8_string::resize(std::size_t new_size)
{
    auto current_size = count();
    if (new_size < current_size)
    {
        auto it = begin();
        std::advance(it, new_size);

        string_data_.erase(it.base(), string_data_.end());
    }

    else
    {
        auto size_in_bytes = string_data_.size();

        string_data_.resize(size_in_bytes + (new_size - current_size));
    }
}

std::size_t ts::utf8_string::count() const
{
    return utf8::distance(string_data_.begin(), string_data_.end());
}

ts::utf8_string::iterator ts::utf8_string::begin() const
{
    return iterator(string_data_.begin(), string_data_.begin(), string_data_.end());
}

ts::utf8_string::iterator ts::utf8_string::end() const
{
    return iterator(string_data_.end(), string_data_.begin(), string_data_.end());
}

ts::utf8_string::iterator ts::begin(const utf8_string& string)
{
    return string.begin();
}

ts::utf8_string::iterator ts::end(const utf8_string& string)
{
    return string.end();
}

bool ts::utf8_string::valid() const
{
    return utf8::is_valid(string_data_.begin(), string_data_.end());
}

std::size_t ts::utf8_string::size() const
{
    return string_data_.size();
}

bool ts::utf8_string::empty() const
{
    return string_data_.empty();
}

void ts::utf8_string::append(utf8::uint32_t code_point)
{
    utf8::append(code_point, std::back_inserter(string_data_));
}

void ts::utf8_string::append(const utf8_string& string)
{
    string_data_.insert(string_data_.end(), string.string_data_.begin(), string.string_data_.end());
}

ts::utf8_string::iterator ts::utf8_string::erase(iterator where)
{
    auto base = where.base(), next = base;

    utf8::next(next, string_data_.cend());

    auto result = string_data_.erase(base, next);

    return iterator(result, string_data_.begin(), string_data_.end());
}

ts::utf8_string::iterator ts::utf8_string::erase(iterator it, iterator end)
{
    auto result = string_data_.erase(it.base(), end.base());

    return iterator(result, string_data_.begin(), string_data_.end());
}


ts::utf8_string::iterator ts::utf8_string::insert(iterator where, utf8::uint32_t code_point)
{
    return insert(where, utf8_string(code_point));
}

ts::utf8_string::iterator ts::utf8_string::insert(iterator where, const utf8_string& string)
{
    auto it = string_data_.insert(where.base(), string.string().begin(), string.string().end());
    return iterator(it, string_data_.begin(), string_data_.end());
}

ts::utf8_string& ts::utf8_string::operator=(std::string rhs)
{
    string_data_ = std::move(rhs);
    return *this;
}

ts::utf8_string& ts::utf8_string::operator=(utf8_string&& rhs)
{
    string_data_ = std::move(rhs.string_data_);
    return *this;
}

ts::utf8_string& ts::utf8_string::operator+=(utf8::uint32_t code_point)
{
    append(code_point);
    return *this;
}

ts::utf8_string& ts::utf8_string::operator+=(const utf8_string& rhs)
{
    append(rhs);
    return *this;
}

ts::utf8_string_view::utf8_string_view()
: begin_(nullptr),
  end_(nullptr)
{
}

ts::utf8_string_view::utf8_string_view(const char* begin, const char* end)
: begin_(begin),
  end_(end)
{
}

const char* ts::utf8_string_view::data() const
{
    return begin_;
}

ts::utf8_string_view::const_iterator ts::utf8_string_view::begin() const
{
    return const_iterator(begin_, begin_, end_);
}

ts::utf8_string_view::const_iterator ts::utf8_string_view::end() const
{
    return const_iterator(end_, begin_, end_);
}

std::size_t ts::utf8_string_view::size() const
{
    return end_ - begin_;
}

std::size_t std::hash<ts::utf8_string>::operator()(const argument_type& arg) const
{
    std::size_t result = 0;
    for (auto cp : arg)
    {
        result += cp * 101;
    }

    return result;
}

bool ts::operator==(const utf8_string& lhs, const utf8_string& rhs)
{
    if (lhs.size() != rhs.size()) return false;

	return std::equal(lhs.data(), lhs.data() + lhs.size(), rhs.data());
}

bool ts::operator<(const utf8_string& lhs, const utf8_string& rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
bool ts::operator>(const utf8_string& lhs, const utf8_string& rhs)
{
    return rhs < rhs;
}

bool ts::operator!=(const utf8_string& lhs, const utf8_string& rhs)
{
    return !(lhs == rhs);
}

bool ts::operator<=(const utf8_string& lhs, const utf8_string& rhs)
{
    return !(lhs > rhs);
}

bool ts::operator>=(const utf8_string& lhs, const utf8_string& rhs)
{
    return !(lhs < rhs);
}


bool ts::operator==(const utf8_string& lhs, const utf8_string_view& rhs)
{
    if (lhs.size() != rhs.size()) return false;

    return std::equal(lhs.data(), lhs.data() + lhs.size(), rhs.data());
}

bool ts::operator<(const utf8_string& lhs, const utf8_string_view& rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

bool ts::operator>(const utf8_string& lhs, const utf8_string_view& rhs)
{
    return rhs < rhs;
}

bool ts::operator!=(const utf8_string& lhs, const utf8_string_view& rhs)
{
    return !(lhs == rhs);
}

bool ts::operator<=(const utf8_string& lhs, const utf8_string_view& rhs)
{
    return !(lhs > rhs);
}

bool ts::operator>=(const utf8_string& lhs, const utf8_string_view& rhs)
{
    return !(lhs < rhs);
}

bool ts::operator==(const utf8_string_view& lhs, const utf8_string_view& rhs)
{
    if (lhs.size() != rhs.size()) return false;

    return std::equal(lhs.data(), lhs.data() + lhs.size(), rhs.data());
}

bool ts::operator<(const utf8_string_view& lhs, const utf8_string_view& rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

bool ts::operator>(const utf8_string_view& lhs, const utf8_string_view& rhs)
{
    return rhs < rhs;
}

bool ts::operator!=(const utf8_string_view& lhs, const utf8_string_view& rhs)
{
    return !(lhs == rhs);
}

bool ts::operator<=(const utf8_string_view& lhs, const utf8_string_view& rhs)
{
    return !(lhs > rhs);
}

bool ts::operator>=(const utf8_string_view& lhs, const utf8_string_view& rhs)
{
    return !(lhs < rhs);
}


bool ts::operator==(const utf8_string_view& lhs, const utf8_string& rhs)
{
    if (lhs.size() != rhs.size()) return false;

    return std::equal(lhs.data(), lhs.data() + lhs.size(), rhs.data());
}

bool ts::operator<(const utf8_string_view& lhs, const utf8_string& rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

bool ts::operator>(const utf8_string_view& lhs, const utf8_string& rhs)
{
    return rhs < rhs;
}

bool ts::operator!=(const utf8_string_view& lhs, const utf8_string& rhs)
{
    return !(lhs == rhs);
}

bool ts::operator<=(const utf8_string_view& lhs, const utf8_string& rhs)
{
    return !(lhs > rhs);
}

bool ts::operator>=(const utf8_string_view& lhs, const utf8_string& rhs)
{
    return !(lhs < rhs);
}


ts::utf8_string ts::operator+(utf8_string lhs, const utf8_string& rhs)
{
    return lhs += rhs;
}

std::ostream& ts::operator<<(std::ostream& stream, const utf8_string& string)
{
    return stream << string.string();
}

std::istream& ts::operator>>(std::istream& stream, utf8_string& string)
{
    std::string std_string;
    stream >> std_string;

    string = std::move(std_string);
    return stream;
}

ts::utf8_string ts::to_lower(const utf8_string& string)
{
    std::string data = string.string();

    boost::to_lower(data);

    return utf8_string(std::move(data));
}

ts::utf8_string ts::to_upper(const utf8_string& string)
{
    std::string data = string.string();

    boost::to_upper(data);

    return utf8_string(std::move(data));
}