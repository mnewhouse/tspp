#pragma once

#ifndef GUI_ELEMENTS_DRAGGABLE_HPP
#define GUI_ELEMENTS_DRAGGABLE_HPP

#include <iostream>

namespace ts
{
    namespace gui
    {
        namespace elements
        {
            class Draggable
                : public Element
            {
            public:
                void set_base_position(Vector2i base_position)
                {
                    base_position_ = base_position;
                }

                Vector2i base_position() const
                {
                    return base_position_;
                }

                void set_x_offset_limit(std::int32_t min, std::int32_t max)
                {
                    min_x_offset_ = min;
                    max_x_offset_ = max;
                }

                void set_y_offset_limit(std::int32_t min, std::int32_t max)
                {
                    min_y_offset_ = min;
                    max_y_offset_ = max;
                }

                std::int32_t min_x_offset() const { return min_x_offset_; }
                std::int32_t min_y_offset() const { return min_y_offset_; }

                std::int32_t max_x_offset() const { return max_x_offset_; }
                std::int32_t max_y_offset() const { return max_y_offset_; }

                Vector2i drag_offset() const
                {
                    return drag_offset_;
                }


                virtual void on_mouse_move(Vector2i old_position, Vector2i new_position) override
                {
                    if (!button_state(mouse::Button::Left))
                    {
                        being_dragged_ = false;
                        return;
                    }

                    if (!hover() && !being_dragged_)
                    {
                        return;
                    }

                    drag_offset_.x += new_position.x - old_position.x;
                    drag_offset_.x = std::min(drag_offset_.x, max_x_offset_);
                    drag_offset_.x = std::max(drag_offset_.x, min_x_offset_);

                    drag_offset_.y += new_position.y - old_position.y;
                    drag_offset_.y = std::min(drag_offset_.y, max_y_offset_);
                    drag_offset_.y = std::max(drag_offset_.y, min_y_offset_);

                    set_position(base_position_ + drag_offset_);

                    being_dragged_ = true;
                }

            private:
                Vector2i base_position_;

                std::int32_t min_x_offset_ = 0;
                std::int32_t max_x_offset_ = 0;

                std::int32_t min_y_offset_ = 0;
                std::int32_t max_y_offset_ = 0;

                Vector2i drag_offset_;
                bool being_dragged_ = false;
            };
        }
    }
}

#endif