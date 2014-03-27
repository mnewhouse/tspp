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

#include "script_functions.hpp"
#include "world_interface.hpp"
#include "hud_overlay.hpp"

#include "world/car.hpp"
#include "world/world.hpp"
#include "controls/control_center.hpp"

namespace ts
{
    namespace script
    {
        namespace functions
        {
            void assert_entity_alive(Entity_handle* entity_handle);
        }
    }
}

// -------------------------------------------------------------------
// World functions
// -------------------------------------------------------------------

std::uint64_t ts::script::functions::World_interface_object::getGameTime()
{
    return identity()->world()->game_time();
}

void ts::script::functions::World_interface_object::startGameTimer()
{
    identity()->world()->start_game_timer();
}

// -------------------------------------------------------------------
// HUD functions
// -------------------------------------------------------------------

ts::script::Text_display* ts::script::functions::HUD_object::createTextDisplay
    (const std::string& text, std::uint32_t character_size, int font)
{
    auto font_type = ts::script::Font_type::Sans;
    if (font == 1) font_type = ts::script::Font_type::Monospace;

    return identity()->create_text_display(text, character_size, font_type);
}

void ts::script::functions::TextDisplay_addRef(Text_display* text_display)
{
    ++text_display->ref_count;
}

void ts::script::functions::TextDisplay_release(Text_display* text_display)
{
    if (text_display->ref_count == 0 || --text_display->ref_count == 0)
    {
        text_display->hud->destroy_text_display(text_display);
    }
}

void ts::script::functions::TextDisplay_setPosition(const Vector2<double>& position, Text_display* text_display)
{
    text_display->text.setPosition(position.x, position.y);
}

ts::Vector2<double> ts::script::functions::TextDisplay_getPosition(Text_display* text_display)
{
    const auto& result = text_display->text.getPosition();
    return Vector2<double>(result.x, result.y);
}

void ts::script::functions::TextDisplay_setColor(std::uint32_t color, Text_display* text_display)
{
    std::uint8_t red = (color >> 24) & 0xFF;
    std::uint8_t green = (color >> 16) & 0xFF;
    std::uint8_t blue = (color >> 8) & 0xFF;
    std::uint8_t alpha = color & 0xFF;

    text_display->text.setColor(sf::Color(red, green, blue, alpha));
}

void ts::script::functions::TextDisplay_setText(const std::string& text, Text_display* text_display)
{
    text_display->text.setString(text);
}

void ts::script::functions::TextDisplay_setCharacterSize(std::uint32_t character_size, Text_display* text_display)
{
    text_display->text.setCharacterSize(character_size);
}
            
std::uint32_t ts::script::functions::TextDisplay_getColor(Text_display* text_display)
{
    auto color = text_display->text.getColor();

    auto result = std::uint32_t(color.r) << 24;
    result |= color.g << 16;
    result |= color.b << 8;
    result |= color.a;

    return result;
}

std::string ts::script::functions::TextDisplay_getText(Text_display* text_display)
{
    return text_display->text.getString();
}

std::uint32_t ts::script::functions::TextDisplay_getCharacterSize(Text_display* text_display)
{
    return text_display->text.getCharacterSize();
}

ts::Vector2<double> ts::script::functions::TextDisplay_getDimensions(Text_display* text_display)
{
    auto bounds = text_display->text.getLocalBounds();
    return Vector2<double>(bounds.width, bounds.height);
}

// -------------------------------------------------------------------
// Vector2 functions
// -------------------------------------------------------------------

void ts::script::functions::Vector2i_construct(std::int32_t x, std::int32_t y, void* mem)
{
    new (mem) Vector2<std::int32_t>(x, y);
}

void ts::script::functions::Vector2i_default_construct(void* mem)
{
    new (mem) Vector2<std::int32_t>();
}

void ts::script::functions::Vector2i_copy_construct(const Vector2<std::int32_t>& vec, void* mem)
{
    new (mem) Vector2<double>(vec);
}

void ts::script::functions::Vector2f_construct(double x, double y, void* mem)
{
    new (mem) Vector2<double>(x, y);
}

void ts::script::functions::Vector2f_default_construct(void* mem)
{
    new (mem) Vector2<double>();
}

void ts::script::functions::Vector2f_copy_construct(const Vector2<double>& vec, void* mem)
{
    new (mem) Vector2<double>(vec);
}


void ts::script::functions::Vector2i_destruct(void* mem)
{
}

void ts::script::functions::Vector2f_destruct(void* mem)
{
}

// -------------------------------------------------------------------
// Control center functions
// -------------------------------------------------------------------

void ts::script::functions::Control_center_object::disableGlobalControl(const std::string& control_string)
{
    toggleGlobalControl(control_string, false);
}

void ts::script::functions::Control_center_object::enableGlobalControl(const std::string& control_string)
{
    toggleGlobalControl(control_string, true);
}

void ts::script::functions::Control_center_object::toggleGlobalControl(const std::string& control_string, bool enable)
{    
    auto control = controls::control_from_string(control_string);

    using controls::Control;
    if (control != Control::None) {
        identity()->toggle_global_control(control, enable);
    }
}

// -------------------------------------------------------------------
// Entity functions
// -------------------------------------------------------------------

void ts::script::functions::assert_entity_alive(Entity_handle* entity_handle)
{
    if (entity_handle->alive) return;

    std::cout << "HAUEUAHEUHAE\n";

    auto context = asGetActiveContext();
    context->SetException("attempt to operate on dead entity");
}

ts::Vector2<double> ts::script::functions::Entity_getVelocity(Entity_handle* entity_handle)
{
    assert_entity_alive(entity_handle);

    return entity_handle->entity->velocity();
}


ts::Vector2<double> ts::script::functions::Entity_getPosition(Entity_handle* entity_handle)
{
    assert_entity_alive(entity_handle);

    return entity_handle->entity->position();
}

double ts::script::functions::Entity_getRotation(Entity_handle* entity_handle)
{
    assert_entity_alive(entity_handle);

    return entity_handle->entity->rotation().degrees();
}

double ts::script::functions::Entity_getSpeed(Entity_handle* entity_handle)
{
    assert_entity_alive(entity_handle);

    return entity_handle->entity->speed();
}

double ts::script::functions::Entity_getZPosition(Entity_handle* entity_handle)
{
    assert_entity_alive(entity_handle);

    return entity_handle->entity->z_position();
}

void ts::script::functions::Entity_addRef(Entity_handle* entity_handle)
{
    ++entity_handle->ref_count;
}

void ts::script::functions::Entity_release(Entity_handle* entity_handle)
{
    if (entity_handle->ref_count == 0 || --entity_handle->ref_count == 0)
    {
        entity_handle->world_interface->release_entity_handle(entity_handle);
    }
}

// -------------------------------------------------------------------
// Entity iterator functions
// -------------------------------------------------------------------

template <typename EntityType>
void ts::script::functions::EntityIterator_increment(Entity_iterator& entity_iterator)
{
    const auto& entity_list = entity_iterator.interface->world()->entity_list();

    auto size = entity_list.size();

    auto index = entity_iterator.index;
    while (++index < size && !dynamic_cast<EntityType*>(entity_list[index]));

    entity_iterator.index = index;
}

template <typename EntityType>
void ts::script::functions::EntityIterator_decrement(Entity_iterator& entity_iterator)
{
    const auto& entity_list = entity_iterator.interface->world()->entity_list();

    auto size = entity_list.size();

    auto index = entity_iterator.index;
    while (index < size && !dynamic_cast<EntityType*>(entity_list[index])) --index;

    entity_iterator.index = index;
}

void ts::script::functions::EntityIterator_construct(void* mem)
{
    auto engine = asGetActiveContext()->GetEngine();
    auto interface = static_cast<World_interface*>(engine->GetUserData(WorldInterface_udata));

    auto entity_iterator = new(mem) Entity_iterator();
    entity_iterator->index = 0;
    entity_iterator->interface = interface;
}

ts::script::Entity_handle* ts::script::functions::EntityIterator_invoke(const Entity_iterator& entity_iterator)
{
    const auto& entity_list = entity_iterator.interface->world()->entity_list();

    auto index = entity_iterator.index;
    if (index >= entity_list.size()) return nullptr;

    return entity_iterator.interface->get_entity_handle(entity_list[index]);
}

bool ts::script::functions::EntityIterator_valid(const Entity_iterator& entity_iterator)
{
    const auto& entity_list = entity_iterator.interface->world()->entity_list();

    return entity_iterator.index < entity_list.size();
}

namespace
{
    template <typename EntityType>
    void instantiate_entity_iterator_type()
    {
        ts::script::Entity_iterator it;
        ts::script::functions::EntityIterator_increment<EntityType>(it);
        ts::script::functions::EntityIterator_decrement<EntityType>(it);
    }

    template void instantiate_entity_iterator_type<ts::world::Entity>();
    template void instantiate_entity_iterator_type<ts::world::Car>();
}