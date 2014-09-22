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
#include "background_document.hpp"

#include "user_interface/gui_context.hpp"
#include "user_interface/document.hpp"

namespace
{
    static const ts::utf8_string background_document_name = "__BACKGROUND";
    static const ts::utf8_string background_image_path = "data/background.jpg";
}


ts::gui::Document_handle ts::gui_definitions::create_background_document(gui::Context* context)
{
    auto document = context->create_document(background_document_name);
    document->set_visible(true);

    auto background_texture = context->load_texture(background_image_path);

    auto background_style = gui::make_background_style<gui::Textured_background>(background_texture);
    document->set_background_style(background_style);

    return document;
}

void ts::gui_definitions::show_background_document(gui::Context* context)
{
    auto document = context->document_by_name(background_document_name);
    if (document)
    {
        document->set_visible(true);
    }
}

void ts::gui_definitions::hide_background_document(gui::Context* context)
{
    auto document = context->document_by_name(background_document_name);
    if (document)
    {
        document->set_visible(false);
    }
}