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

#ifndef GUI_CONTEXT_HPP
#define GUI_CONTEXT_HPP

#include "document_handle.hpp"
#include "input_interface.hpp"

#include "resources/font_library.hpp"
#include "graphics/texture_loader.hpp"

#include <SFML/Graphics.hpp>

namespace ts
{
    namespace gui
    {
        class Document;

        using resources::Font_library;

        namespace fonts
        {
            using resources::fonts::Sans;
            using resources::fonts::Monospace;
        }

        class Context
            : private SFML_Input_interface
        {
        public:
            Context(sf::RenderTarget* render_target, const Font_library* font_library);

            Vector2i screen_size() const;
            
            Document_handle create_document(utf8_string document_name);
            void destroy_document(Document_handle&& document);
            void destroy_document(const utf8_string& document_name);

            Document* document_by_name(const utf8_string& document_name) const;

            void render_documents();
            void update_documents();

            void hide_all();

            const Font_library& font_library() const;

            using SFML_Input_interface::handle_event;

            std::shared_ptr<graphics::Texture> load_texture(const utf8_string& texture_name);

        private:            
            virtual void process_mouse_move(Vector2i new_position, unsigned int key_modifiers) override;
            virtual void process_mouse_button_down(mouse::Button button, unsigned int key_modifiers) override;
            virtual void process_mouse_button_up(mouse::Button button, unsigned int key_modifiers) override;
            virtual void process_mouse_wheel(int wheel_delta, unsigned int key_modifiers) override;

            virtual void process_key_up(sf::Keyboard::Key key_code, unsigned int key_modifiers) override;
            virtual void process_key_down(sf::Keyboard::Key key_code, unsigned int key_modifiers) override;

            virtual void process_text_input(utf8::uint32_t code_point) override;

            void release_destroyed_documents();
            void add_pending_documents();

            struct Callback_guard
            {
                Callback_guard(Context* context);
                ~Callback_guard();

            private:
                Context* context_;
            };

            // Data members
            sf::RenderTarget* render_target_;
            const resources::Font_library* font_library_;

            std::unordered_map<utf8_string, std::unique_ptr<Document>> document_map_;
            std::vector<Document*> document_list_;

            graphics::Texture_loader texture_loader_;

            std::size_t callback_guard_count_ = 0;
            std::set<utf8_string> destroyed_documents_;
            std::vector<Document*> pending_documents_;

            Vector2i mouse_position_;
        };
    }
}


#endif