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

#include "gui_scene.hpp"
#include "gui_element.hpp"

#include "context.hpp"

ts::gui::Scene::Scene(Context* context)
    : context_(context)
{
}

void ts::gui::Scene::register_element(Element* element)
{
    registered_elements_.push_back(element);
}

void ts::gui::Scene::unregister_element(Element* element)
{
    auto it = std::remove(registered_elements_.begin(), registered_elements_.end(), element);
    registered_elements_.erase(it, registered_elements_.end());
}

void ts::gui::Scene::update()
{
    const auto mouse_state = context_->mouse_state();

    for (auto element : registered_elements_)
    {
        if (element->visible()) element->update(mouse_state);
    }
}