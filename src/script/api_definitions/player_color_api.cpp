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
#include "player_color_api.hpp"

#include "script/script_delegates.hpp"
#include "script/script_userdata.hpp"

#include "resources/player_color.hpp"

namespace ts
{
    namespace script_api
    {
        using namespace script;

        namespace player_color
        {
            SQInteger PlayerColor(HSQUIRRELVM vm);
            SQInteger setPrimaryComponent(HSQUIRRELVM vm);
            SQInteger setSecondaryComponent(HSQUIRRELVM vm);
            SQInteger setTertiaryComponent(HSQUIRRELVM vm);

            SQInteger getPrimaryComponent(HSQUIRRELVM vm);
            SQInteger getSecondaryComponent(HSQUIRRELVM vm);
            SQInteger getTertiaryComponent(HSQUIRRELVM vm);

            const Static_function_definition static_functions[] =
            {
                { "PlayerColor", PlayerColor }
            };

            const Member_function_definition member_functions[] =
            {
                { "setPrimaryComponent", setPrimaryComponent },
                { "setSecondaryComponent", setSecondaryComponent },
                { "setTertiaryComponent", setTertiaryComponent },

                { "getPrimaryComponent", getPrimaryComponent },
                { "getSecondaryComponent", getSecondaryComponent },
                { "getTertiaryComponent", getTertiaryComponent }
            };
        }

        template <>
        struct Delegate_traits<resources::Player_color>
        {
            static Range<const Member_function_definition*> member_functions();
        };
    }
}

ts::Range<const ts::script::Member_function_definition*> ts::script_api::Delegate_traits<ts::resources::Player_color>::member_functions()
{
    return make_range(std::begin(player_color::member_functions), std::end(player_color::member_functions));
}

ts::script::API_definition ts::script_api::player_color_api()
{
    API_definition result;

    result.delegates.push_back(create_delegate_definition<resources::Player_color>());
    result.static_functions.assign(std::begin(player_color::static_functions), std::end(player_color::static_functions));

    return result;
}

SQInteger ts::script_api::player_color::PlayerColor(HSQUIRRELVM vm)
{
    make_userdata(vm, resources::Player_color(sf::Color::White)).push();
    return 1;
}

SQInteger ts::script_api::player_color::setPrimaryComponent(HSQUIRRELVM vm)
{
    return 0;
}

SQInteger ts::script_api::player_color::setSecondaryComponent(HSQUIRRELVM vm)
{
    return 0;
}

SQInteger ts::script_api::player_color::setTertiaryComponent(HSQUIRRELVM vm)
{
    return 0;
}

SQInteger ts::script_api::player_color::getPrimaryComponent(HSQUIRRELVM vm)
{
    return 0;
}

SQInteger ts::script_api::player_color::getSecondaryComponent(HSQUIRRELVM vm)
{
    return 0;
}

SQInteger ts::script_api::player_color::getTertiaryComponent(HSQUIRRELVM vm)
{
    return 0;
}