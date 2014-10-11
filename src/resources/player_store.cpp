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
#include "player_store.hpp"

namespace ts
{
    namespace resources
    {
        struct Player_file_entry
        {
            Player player_data;
            std::uint64_t checksum;
        };

        std::uint64_t generate_player_checksum(const Player& player);
        Player_file_entry read_player_data(std::istream& stream);
        resources::Player_color read_color_data(std::istream& stream);
    }
}

ts::resources::Player_store::Player_store(utf8_string players_file)
: file_name_(players_file)
{
    std::random_device dev;
    std::seed_seq seed_seq({ dev(), dev(), dev(), dev(), dev(), dev() });

    random_engine_.seed(seed_seq);

    load_from_file();
}

ts::resources::Player_store::~Player_store()
{
    save_to_file();
}

void ts::resources::Player_store::load_from_file()
{
    boost::filesystem::ifstream file_stream(file_name_.string());
    if (!file_stream) return;

    auto read_color = [](std::istream& stream)
    {
        sf::Color color;
        color.r = stream.get();
        color.g = stream.get();
        color.b = stream.get();
        color.a = stream.get(), 255;
        return color;
    };

    auto player_count = core::read32(file_stream);

    for (auto n = 0; n != player_count; ++n)
    {
        Player player_data;
        player_data.internal_id = core::read64(file_stream);
        player_data.id = core::read64(file_stream);
        
        auto name_length = core::read32(file_stream);
        std::string player_name(name_length, 0);

        file_stream.read(&player_name[0], name_length);

        player_data.name = std::move(player_name);

        auto& color = player_data.color;
        
        auto base_color_type = static_cast<Color_base::Type>(core::read32(file_stream));
        if (base_color_type >= Color_base::Max_value) base_color_type = Color_base::Plain;
        
        color.base_color.type = base_color_type;
        color.base_color.primary_color = read_color(file_stream);
        color.base_color.secondary_color = read_color(file_stream);

        auto primary_extra_type = static_cast<Color_extra::Type>(core::read32(file_stream));
        if (primary_extra_type >= Color_extra::Max_value) primary_extra_type = Color_extra::None;

        color.primary_extra.type = primary_extra_type;
        color.primary_extra.primary_color = read_color(file_stream);
        color.primary_extra.secondary_color = read_color(file_stream);

        auto secondary_extra_type = static_cast<Color_extra::Type>(core::read32(file_stream));
        if (secondary_extra_type >= Color_extra::Max_value) secondary_extra_type = Color_extra::None;

        color.secondary_extra.type = secondary_extra_type;
        color.secondary_extra.primary_color = read_color(file_stream);
        color.secondary_extra.secondary_color = read_color(file_stream);

        auto checksum = core::read64(file_stream);
        auto generated = generate_player_checksum(player_data);

        if (checksum == generated)
        {
            player_map_.emplace(player_data.internal_id, player_data);
        }
    }
}

void ts::resources::Player_store::save_to_file()
{
    boost::filesystem::ofstream out(file_name_.string(), std::ios::binary | std::ios::out);

    auto write_32 = [](std::ostream& out, std::uint32_t data)
    {
        out.put((data >> 24) & 0xFF);
        out.put((data >> 16) & 0xFF);
        out.put((data >> 8) & 0xFF);
        out.put(data & 0xFF);            
    };

    auto write_64 = [](std::ostream& out, std::uint64_t data)
    {
        out.put((data >> 56) & 0xFF);
        out.put((data >> 48) & 0xFF);
        out.put((data >> 40) & 0xFF);
        out.put((data >> 32) & 0xFF);
        out.put((data >> 24) & 0xFF);
        out.put((data >> 16) & 0xFF);
        out.put((data >> 8) & 0xFF);
        out.put(data & 0xFF);
    };

    auto write_color = [](std::ostream& out, sf::Color color)
    {
        out.put(color.r);
        out.put(color.g);
        out.put(color.b);
        out.put(color.a);
    };

    write_32(out, player_map_.size());

    for (auto& player : player_map_)
    {
        auto id = player.first;
        const auto& player_data = player.second;
        const auto& color = player_data.color;

        const auto& player_name = player_data.name.string();
        auto name_length = player_name.size();

        write_64(out, player_data.internal_id);
        write_64(out, player_data.id);
        write_32(out, name_length);
        out.write(player_name.data(), name_length);

        write_32(out, static_cast<std::uint32_t>(color.base_color.type));
        write_color(out, color.base_color.primary_color);
        write_color(out, color.base_color.secondary_color);

        write_32(out, static_cast<std::uint32_t>(color.primary_extra.type));
        write_color(out, color.primary_extra.primary_color);
        write_color(out, color.primary_extra.secondary_color);

        write_32(out, static_cast<std::uint32_t>(color.secondary_extra.type));
        write_color(out, color.secondary_extra.primary_color);
        write_color(out, color.secondary_extra.secondary_color);

        write_64(out, generate_player_checksum(player_data));
    }
}


std::uint64_t ts::resources::generate_player_checksum(const Player& player)
{

    SHA256 sha256;

    const auto& raw_player_name = player.name.string();
    sha256.add(raw_player_name.data(), raw_player_name.size());

    unsigned char buffer[] =
    {
        0x52, 0xA7, 0x19, 0xBA, 0x63, 0xC9, 0x83, 0xF6,

        (player.id >> 0) & 0xFF,
        (player.id >> 5) & 0xFF,
        (player.id >> 11) & 0xFF,
        (player.id >> 17) & 0xFF,
        (player.id >> 23) & 0xFF,
        (player.id >> 29) & 0xFF,
        (player.id >> 31) & 0xFF,
        (player.id >> 37) & 0xFF,
        (player.id >> 41) & 0xFF,
        (player.id >> 47) & 0xFF,
        (player.id >> 53) & 0xFF,
        (player.id >> 59) & 0xFF,
        (player.id >> 61) & 0xFF,

        ((player.id >> 5) ^ (player.id >> 55)) & 0xFF,
        ((player.id >> 15) ^ (player.id >> 45)) & 0xFF,
        ((player.id >> 25) ^ (player.id >> 35)) & 0xFF,
    };

    std::string message = player.name.string();
    message.insert(message.end(), std::begin(buffer), std::end(buffer));
    
    auto digest = sha256(message);

    std::uint64_t result = 0;
    result |= static_cast<std::uint64_t>(digest[0]) << 32;
    result |= static_cast<std::uint64_t>(digest[4]);

    result ^= static_cast<std::uint64_t>(digest[1]) << 53;
    result ^= static_cast<std::uint64_t>(digest[2]) << 41;
    result ^= static_cast<std::uint64_t>(digest[3]) << 37;

    result ^= static_cast<std::uint64_t>(digest[5]) << 29;
    result ^= static_cast<std::uint64_t>(digest[6]) << 17;
    result ^= static_cast<std::uint64_t>(digest[7]) << 13;

    return result;
}

ts::resources::Player_handle ts::resources::Player_store::create_player(utf8_string nickname)
{
    auto unique_id = allocate_unique_id();

    auto& player = player_map_[unique_id];

    player.name.clear();

    auto nick_length = nickname.count();
    std::copy_n(nickname.begin(), std::min(nick_length, max_player_name_length), std::back_inserter(player.name));

    player.id = generate_player_id();
    player.color = generate_player_color();
    player.internal_id = unique_id;
    

    return Player_handle(&player);
}

void ts::resources::Player_store::delete_player(Player_handle player)
{
    player_map_.erase(player->internal_id);
}

ts::resources::Player_store::unique_id ts::resources::Player_store::allocate_unique_id() const
{
    if (player_map_.empty()) return 1;

    return player_map_.rbegin()->first + 1;
}

ts::resources::Player_handle ts::resources::Player_store::get_player_by_id(unique_id id) const
{
    auto it = player_map_.find(id);
    if (it != player_map_.end()) return Player_handle(&it->second);

    return Player_handle();
}


std::uint64_t ts::resources::Player_store::generate_player_id() const
{
    return random_engine_();
}

void ts::resources::Player_store::set_player_color(Player_handle player, Player_color new_color)
{
    auto player_entry = player_map_.find(player->internal_id);
    if (player_entry != player_map_.end())
    {
        player_entry->second.color = new_color;
    }
}

ts::resources::Player_color ts::resources::Player_store::generate_player_color() const
{
    std::uniform_int_distribution<std::uint16_t> dist(std::numeric_limits<std::uint16_t>::min(), std::numeric_limits<std::uint16_t>::max());

    sf::Color primary_color;
    primary_color.r = dist(random_engine_) & 0xFF;
    primary_color.g = dist(random_engine_) & 0xFF;
    primary_color.b = dist(random_engine_) & 0xFF;
    primary_color.a = 255;

    sf::Color secondary_color;
    secondary_color.r = dist(random_engine_) & 0xFF;
    secondary_color.g = dist(random_engine_) & 0xFF;
    secondary_color.b = dist(random_engine_) & 0xFF;
    secondary_color.a = 255;


    std::uniform_int_distribution<std::uint32_t> type_dist(0, Color_base::Max_value - 1);

    Player_color result;
    result.base_color.type = static_cast<Color_base::Type>(type_dist(random_engine_));
    result.base_color.primary_color = primary_color;
    result.base_color.secondary_color = secondary_color;

    return result;
}

ts::resources::Player_store::iterator ts::resources::Player_store::begin() const
{
    return iterator(player_map_.begin());
}

ts::resources::Player_store::iterator ts::resources::Player_store::end() const
{
    return iterator(player_map_.end());
}

ts::resources::Player_store::iterator::iterator(base_type it)
    : it_(it)
{ 
}

ts::resources::Player_handle ts::resources::Player_store::iterator::operator*() const
{
    return Player_handle(&it_->second);
}

ts::resources::Player_handle ts::resources::Player_store::iterator::operator->() const
{
    return Player_handle(&it_->second);
}

ts::resources::Player_store::iterator& ts::resources::Player_store::iterator::operator++()
{
    ++it_;
    return *this;
}

ts::resources::Player_store::iterator& ts::resources::Player_store::iterator::operator--()
{
    --it_;
    return *this;
}

ts::resources::Player_store::iterator ts::resources::Player_store::iterator::operator++(int)
{
    auto result = *this;
    ++*this;
    return result;
}

ts::resources::Player_store::iterator ts::resources::Player_store::iterator::operator--(int)
{
    auto result = *this;
    --*this;
    return result;
}

bool ts::resources::Player_store::iterator::operator==(const iterator& rhs) const
{
    return it_ == rhs.it_;
}

bool ts::resources::Player_store::iterator::operator!=(const iterator& rhs) const
{
    return it_ != rhs.it_;
}