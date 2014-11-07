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
#include "gui_context.hpp"
#include "document.hpp"

#include "graphics/sprite.hpp"

ts::gui::Context::Context(sf::RenderTarget* render_target, const resources::Font_library* font_library)
: render_target_(render_target),
  font_library_(font_library)
{
}

ts::gui::Document_handle ts::gui::Context::create_document(utf8_string document_name)
{
    auto document = std::make_unique<Document>(document_name, this);
    auto document_ptr = &*document;

    auto it = document_map_.emplace(std::move(document_name), std::move(document));
    if (!it.second)
    {
        return Document_handle();
    }    
    
    if (callback_guard_count_ != 0)
    {
        pending_documents_.push_back(document_ptr);
    }

    else
    {
        document_list_.push_back(document_ptr);
    }

    document_ptr->set_visible(false);
    return Document_handle(document_ptr, this);
}

void ts::gui::Context::destroy_document(Document_handle&& document)
{
    destroy_document(document->document_name());

    document = Document_handle();
}

ts::gui::Document* ts::gui::Context::document_by_name(const utf8_string& document_name) const
{
    auto it = document_map_.find(document_name);
    if (it == document_map_.end())
    {
        return nullptr;
    }

    return it->second.get();
}

void ts::gui::Context::hide_all()
{
    for (auto document : document_list_)
    {
        document->set_visible(false);
    }

    for (auto document : pending_documents_)
    {
        document->set_visible(false);
    }
}

void ts::gui::Context::destroy_document(const utf8_string& document_name)
{
    if (callback_guard_count_ != 0)
    {
        destroyed_documents_.insert(document_name);
    }

    else
    {
        auto it = document_map_.find(document_name);
        if (it != document_map_.end())
        {
            document_list_.erase(std::remove(document_list_.begin(), document_list_.end(), &*it->second), document_list_.end());
            document_map_.erase(it);
        }
    }
}

const ts::gui::Font_library& ts::gui::Context::font_library() const
{
    return *font_library_;
}

std::shared_ptr<ts::graphics::Texture> ts::gui::Context::load_texture(const utf8_string& file_name)
{
    return texture_loader_.load_from_file(file_name);
}

void ts::gui::Context::update_documents()
{
    Callback_guard guard(this);

    for (const auto& document : document_list_)
    {
        if (document->visible() && document->active())
        {
            document->update();
        }
    }
}

void ts::gui::Context::render_documents()
{
    Callback_guard guard(this);

    for (const auto& document : document_list_)
    {
        if (document->visible())
        {
            document->render_hierarchy(*render_target_);
        }
    }

    render_mouse_cursor();
}

ts::Vector2i ts::gui::Context::screen_size() const
{
    auto size = render_target_->getSize();
    return { static_cast<std::int32_t>(size.x), static_cast<std::int32_t>(size.y) };
}

bool ts::gui::Context::load_mouse_cursor(const utf8_string& file_path)
{
    mouse_cursor_ = load_texture(file_path);
    return mouse_cursor_ != nullptr;
}

void ts::gui::Context::render_mouse_cursor()
{
    if (mouse_cursor_ != nullptr && mouse_cursor_visible_)
    {
        graphics::Sprite sprite(*mouse_cursor_);
        sprite.setPosition(static_cast<float>(mouse_position_.x), static_cast<float>(mouse_position_.y));

        render_target_->draw(sprite);
    }
}

void ts::gui::Context::set_mouse_cursor_visible(bool visible)
{
    mouse_cursor_visible_ = visible;
}


void ts::gui::Context::process_mouse_move(Vector2i new_position, unsigned int key_modifiers)
{
    Callback_guard guard(this);

    for (const auto& document : document_list_)
    {
        if (document->visible() && document->active())
        {
            document->process_mouse_move(mouse_position_, new_position);
        }
    }

    mouse_position_ = new_position;
}

void ts::gui::Context::process_mouse_button_down(mouse::Button button, unsigned int key_modifiers)
{
    Callback_guard guard(this);

    for (const auto& document : document_list_)
    {
        if (document->visible() && document->active())
        {
            document->process_mouse_button_down(button, mouse_position_);
        }
    }
}

void ts::gui::Context::process_mouse_button_up(mouse::Button button, unsigned int key_modifiers)
{
    Callback_guard guard(this);

    for (const auto& document : document_list_)
    {
        if (document->visible() && document->active())
        {
            document->process_mouse_button_up(button, mouse_position_);
        }
    }
}

void ts::gui::Context::process_mouse_wheel(int wheel_delta, unsigned int key_modifiers)
{
    Callback_guard guard(this);

    for (const auto& document : document_list_)
    {
        if (document->visible() && document->active())
        {
            document->process_mouse_wheel(wheel_delta, key_modifiers);
        }
    }
}

void ts::gui::Context::process_key_up(sf::Keyboard::Key key_code, unsigned int key_modifiers)
{
    Callback_guard guard(this);

    for (const auto& document : document_list_)
    {
        if (document->active() && document->visible())
        {
            document->process_key_up(key_code, key_modifiers);
        }
    }
}

void ts::gui::Context::process_key_down(sf::Keyboard::Key key_code, unsigned int key_modifiers)
{
    Callback_guard guard(this);

    for (const auto& document : document_list_)
    {
        if (document->active() && document->visible())
        {
            document->process_key_down(key_code, key_modifiers);
        }
    }
}


void ts::gui::Context::process_text_input(utf8::uint32_t code_point)
{
    Callback_guard guard(this);

    for (const auto& document : document_list_)
    {
        if (document->visible() && document->active())
        {
            document->process_text_input(code_point);
        }
    }
}

void ts::gui::Context::add_pending_documents()
{
    for (auto document : pending_documents_)
    {
        document_list_.push_back(document);
    }

    pending_documents_.clear();
}

void ts::gui::Context::release_destroyed_documents()
{
    for (auto& document_name : destroyed_documents_)
    {
        auto it = document_map_.find(document_name);
        if (it != document_map_.end())
        {
            document_list_.erase(std::remove(document_list_.begin(), document_list_.end(), &*it->second), document_list_.end());
            document_map_.erase(it);
        }
    }

    destroyed_documents_.clear();
}

ts::gui::Context::Callback_guard::Callback_guard(Context* context)
  : context_(context)
{
    ++context_->callback_guard_count_;
}

ts::gui::Context::Callback_guard::~Callback_guard()
{
    if (--context_->callback_guard_count_ == 0)
    {
        context_->release_destroyed_documents();
        context_->add_pending_documents();
    }
}
