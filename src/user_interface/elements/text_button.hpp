#ifndef GUI_ELEMENTS_TEXT_BUTTON_HPP
#define GUI_ELEMENTS_TEXT_BUTTON_HPP

#include "button.hpp"

#include "graphics/renderer.hpp"

#include <string>

namespace ts
{
    namespace gui
    {
        namespace elements
        {
            struct Text_button
                : public Rectangular_button
            {
            public:
                Text_button(Vector2i area_size, const std::string& text, const sf::Font& font, unsigned character_size)
                    : Rectangular_button(area_size), 
                      text_display_(text, font, character_size)
                {
                    color_ = text_display_.getColor();
                    hover_color_ = color_;
                }

                void set_offset(Vector2i offset)
                {
                    text_display_.setOrigin(-offset.x, -offset.y);
                }

                void set_character_size(unsigned size)
                {
                    text_display_.setCharacterSize(size);
                }
            
                void set_color(sf::Color color)
                {
                    color_ = color;

                    if (!element_state().hover) text_display_.setColor(color);
                }

                void set_text(const std::string& text)
                {
                    text_display_.setString(text);
                }

                void set_hover_color(sf::Color color)
                {
                    hover_color_ = color;
                }

                virtual void on_state_change(Element_state old_state, Element_state new_state) override
                {
                    if (new_state.hover) text_display_.setColor(hover_color_);
                    else text_display_.setColor(color_);
                }

                virtual void draw(graphics::Render_target& render_target, graphics::Render_states render_states) const override
                {
                    render_target.draw(text_display_, render_states);
                }

            private:

                sf::Color color_;
                sf::Color hover_color_;
                sf::Text text_display_;
            };
        }
    }
}






#endif