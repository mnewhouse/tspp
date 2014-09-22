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
#include "document.hpp"
#include "gui_context.hpp"

ts::gui::Document::Document(utf8_string document_name, Context* context)
: Element(context->screen_size()),
  document_name_(document_name),
  context_(context)
{
    set_auto_sizing(false);
}

ts::gui::Document* ts::gui::Document::owner_document()
{
    return this;
}

bool ts::gui::Document::active() const
{
    return active_;
}

void ts::gui::Document::set_active(bool active)
{
    active_ = active;
}

ts::gui::Context* ts::gui::Document::context() const
{
    return context_;
}

void ts::gui::Document::register_element_id(Element* element, utf8_string id)
{
    id_map_.emplace(std::move(id), element);
}

const ts::utf8_string& ts::gui::Document::document_name() const
{
    return document_name_;
}

