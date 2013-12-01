/*
 * Turbo Sliders++
 * Copyright (C) 2013 Martin Newhouse
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

#ifndef GUI_SCENE_HPP
#define GUI_SCENE_HPP

#include "graphics/render_scene.hpp"
#include "context.hpp"

#include "core/handle.hpp"

namespace ts
{

    namespace gui
    {

        class Scene
            : public graphics::Render_scene
        {
        public:
            Scene(const Handle<Context>& context)
                : context_(context), base_id_(context->allocate_base_id())
            {
            }

            const Handle<Context>& context() const { return context_; }

            element_id make_id(element_id id) const { return base_id_ + id; }

        private:
            graphics::Background background_;
            element_id base_id_;
            Handle<Context> context_;

        };

    }

}

#endif