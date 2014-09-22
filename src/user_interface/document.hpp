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

#ifndef GUI_DOCUMENT_ELEMENT_HPP
#define GUI_DOCUMENT_ELEMENT_HPP

#include "gui_element.hpp"

namespace ts
{
    namespace gui
    {
        class Context;

        class Document
            : public Element
        {
        public:
            Document(utf8_string document_name, Context* context);

            virtual Document* owner_document() override;

            Context* context() const;
            bool active() const;

            void set_active(bool active);

            void register_element_id(Element* element, utf8_string id);

            template <typename ElemType = Element>
            ElemType* element_by_id(const utf8_string& id) const;

            const utf8_string& document_name() const;

        private:
            utf8_string document_name_;

            std::unordered_map<utf8_string, Element*> id_map_;
            Context* context_;

            bool active_ = true;
        };

    }
}

template <typename ElemType>
ElemType* ts::gui::Document::element_by_id(const utf8_string& id) const
{
    auto it = id_map_.find(id);
    if (it == id_map_.end())
    {
        return nullptr;
    }

    return dynamic_cast<ElemType>(it->second);
}

#endif