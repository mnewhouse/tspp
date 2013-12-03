#include "render_scene.hpp"

#include <vector>

namespace ts
{

    namespace graphics
    {

        class Star_field
            : public Render_scene
        {
        public:
            explicit Star_field(std::size_t star_count, sf::Color background_color = sf::Color::Transparent);

            virtual void render(Render_target& render_target) override;

        private:
            struct Star_position
            {
                double x;
                double y;
                double z;
            };

            Star_position random_position(bool randomize_z = false) const;

            std::vector<Star_position> stars_;
            std::vector<sf::Vertex> vertices_;

            sf::Clock clock_;

            sf::Color background_color_;

        };

    }

}