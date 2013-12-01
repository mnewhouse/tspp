#ifndef GRAPHICS_TEXTURE_HPP
#define GRAPHICS_TEXTURE_HPP

#include <SFML/Graphics.hpp>

#include "core/rect.hpp"

namespace ts
{

    namespace graphics
    {

        class Texture
        {
        public:
            explicit Texture(const sf::Image& image, Rect<int> area = Rect<int>{});
            explicit Texture(const std::string& file_name, Rect<int> area = Rect<int>{});

            Texture(Texture&& texture);
            Texture& operator=(Texture&& texture);

            void load_from_file(const std::string& file_name, Rect<int> area = Rect<int>{});
            void load_from_image(const sf::Image& image, Rect<int> area = Rect<int>{});

            Vector2u size() const;

            template <typename Operation>
            void perform(Operation op, Rect<int> area = Rect<int>{});

        private:
            Int_rect sub_texture_rect(int id) const;

            std::vector<sf::Texture> sub_textures_;
            sf::Vector2u total_size_;
        };

        template <typename Operation>
        void Texture::perform(Operation op, Rect<int> area)
        {
            if (area.width == 0 || area.height == 0) {
                area.width = total_size_.x;
                area.height = total_size_.y;
                area.left = 0;
                area.top = 0;
            }

            for (unsigned int id = 0; id < sub_textures_.size(); ++id) {
                auto rect = intersection(sub_texture_rect(id), area);

                if (rect.width && rect.height) {
                    op(sub_textures_[id], sf::IntRect(rect.left, rect.top, rect.width, rect.height));
                }
            }
        }


    }

}

#endif