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

#ifndef SCRIPT_FUNCTIONS_HPP
#define SCRIPT_FUNCTIONS_HPP

#include <cstdint>
#include <cstddef>
#include <string>

#include "core/vector2.hpp"

#include "scriptarray/scriptarray.h"

namespace ts
{
    namespace world
    {
        class World;
        class Entity;
    }

    namespace controls
    {
        class Control_center;
    }

    namespace script
    {
        class HUD_overlay;
        struct Text_display;
        class World_interface;

        struct Entity_handle;
        struct Entity_iterator;

        namespace functions
        {
            template <typename PtrType>
            void pointer_assign(PtrType*& out, PtrType* in)
            {
                if (in && out) out = in;
            }

            template <typename PtrType>
            PtrType* pointer_construct(PtrType* in)
            {
                return in;
            }

            template <typename T>
            struct Identity
            {
                T* identity() { return static_cast<T*>(static_cast<void*>(this)); }
            };

            struct World_interface_object
                : Identity<World_interface>
            {
                void startGameTimer();
                std::size_t getGameTime();
            };
            
            struct Control_center_object
                : Identity<ts::controls::Control_center>
            {

                void disableGlobalControl(const std::string& control_string);
                void enableGlobalControl(const std::string& control_string);

            private:
                void toggleGlobalControl(const std::string& control_string, bool enable);
            };

            struct HUD_object
                : Identity<ts::script::HUD_overlay>
            {
                Text_display* createTextDisplay(const std::string& text, std::uint32_t character_size, int font);
            };

            void TextDisplay_addRef(Text_display* text_display);
            void TextDisplay_release(Text_display* text_display);
            
            void TextDisplay_setPosition(const Vector2<double>& position, Text_display* text_display);
            void TextDisplay_setColor(std::uint32_t color, Text_display* text_display);
            void TextDisplay_setText(const std::string& text, Text_display* text_display);
            void TextDisplay_setCharacterSize(std::uint32_t character_size, Text_display* text_display);
            
            std::uint32_t TextDisplay_getColor(Text_display* text_display);
            std::string TextDisplay_getText(Text_display* text_display);
            std::uint32_t TextDisplay_getCharacterSize(Text_display* text_display);
            Vector2<double> TextDisplay_getPosition(Text_display* text_display);
            Vector2<double> TextDisplay_getDimensions(Text_display* text_display);


            // Vector functions

            void Vector2i_construct(std::int32_t x, std::int32_t y, void* mem);
            void Vector2i_default_construct(void* mem);
            void Vector2i_copy_construct(const Vector2<std::int32_t>& vec, void* mem);

            void Vector2f_construct(double x, double y, void* mem);
            void Vector2f_default_construct(void* mem);
            void Vector2f_copy_construct(const Vector2<double>& vec, void* mem);

            void Vector2i_destruct(void* mem);
            void Vector2f_destruct(void* mem);


            Vector2<double> Entity_getVelocity(Entity_handle* entity_handle);
            Vector2<double> Entity_getPosition(Entity_handle* entity_handle);
            double Entity_getRotation(Entity_handle* entity_handle);
            double Entity_getSpeed(Entity_handle* entity_handle);
            double Entity_getZPosition(Entity_handle* entity_handle);

            void Entity_addRef(Entity_handle* entity_handle);
            void Entity_release(Entity_handle* entity_handle);

            void EntityIterator_construct(void* mem);

            template <typename EntityType>
            void EntityIterator_increment(Entity_iterator& entity_iterator);

            template <typename EntityType>
            void EntityIterator_decrement(Entity_iterator& entity_iterator);

            Entity_handle* EntityIterator_invoke(const Entity_iterator& entity_iterator);
            bool EntityIterator_valid(const Entity_iterator& entity_iterator);
        }
    }
}


#endif