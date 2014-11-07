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

#ifndef STDINC_HPP
#define STDINC_HPP

#include <vector>
#include <string>
#include <array>
#include <list>
#include <queue>
#include <random>
#include <chrono>
#include <future>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <algorithm>
#include <numeric>
#include <set>
#include <deque>
#include <memory>
#include <utility>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <locale>
#include <cmath>
#include <cstdint>
#include <cstddef>
#include <cassert>
#include <type_traits>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>

#include "core/vector2.hpp"
#include "core/rotation.hpp"
#include "core/bresenham.hpp"
#include "core/line_intersection.hpp"
#include "core/config.hpp"
#include "core/utility.hpp"
#include "core/rect.hpp"
#include "core/sha256.hpp"
#include "core/directive_reader.hpp"
#include "core/pointer_handle.hpp"
#include "core/stream_utility.hpp"
#include "core/transform.hpp"
#include "core/utf8_string.hpp"
#include "core/unicode.hpp"

#include "utf8.h"

#endif