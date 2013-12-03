#include "star_field.hpp"

#include <random>
#include <chrono>
#include <cstdint>

namespace
{
	std::mt19937 random_engine(std::random_device{}());
}

ts::graphics::Star_field::Star_field(std::size_t count, sf::Color background_color)
    : stars_(count), vertices_(), clock_(), background_color_(background_color)
{
    for (auto& position : stars_) {
        position = random_position(true);
    }
}

void ts::graphics::Star_field::render(Render_target& render_target)
{
    render_target.clear(background_color_);

    const auto screen_size = render_target.getSize();
    sf::CircleShape circle(3.0f, 5);
    auto color = sf::Color::White;

    static const float update_interval = 0.01f;

    auto elapsed_time = clock_.getElapsedTime().asSeconds();
    if (elapsed_time >= update_interval) {
        clock_.restart();
    }

    auto vertex_count = (9 * stars_.size());
    vertices_.resize(vertex_count);

    for (unsigned int idx = 0; idx != stars_.size(); ++idx) {
        
        auto& position = stars_[idx];

        auto scale = float(1.0 - 0.8 * position.z);
        circle.setScale(scale, scale);

        // Set alpha
        color.a = static_cast<sf::Uint8>(200 * (1.0 - position.z));

        auto x = position.x / position.z + screen_size.x / 2.0;
        auto y = position.y / position.z + screen_size.y / 2.0;
        circle.setPosition(float(x), float(y));        

        const auto& transform = circle.getTransform();

        auto* vertices = &vertices_[idx * 9];

        vertices[0].position = transform.transformPoint(circle.getPoint(0));
        vertices[1].position = transform.transformPoint(circle.getPoint(1));
        vertices[2].position = transform.transformPoint(circle.getPoint(2));
        vertices[3].position = vertices[0].position;
        vertices[4].position = transform.transformPoint(circle.getPoint(4));
        vertices[5].position = transform.transformPoint(circle.getPoint(3));
        vertices[6].position = vertices[0].position;
        vertices[7].position = vertices[2].position;
        vertices[8].position = vertices[5].position;

        for (unsigned int i = 0; i != 9; ++i) {
            vertices[i].color = color;
        }

        if (elapsed_time >= update_interval &&
            (position.z -= 0.0013) <= 0.0)
        {
            position = random_position();
        }
    }

    render_target.draw(&vertices_[0], vertex_count, sf::Triangles);
}

ts::graphics::Star_field::Star_position ts::graphics::Star_field::random_position(bool randomize_z) const
{
    std::uniform_real_distribution<double> distribution(-100.0, 100.0);

    Star_position result;
    result.x = distribution(random_engine);
    result.y = distribution(random_engine);

    if (randomize_z) result.z = (distribution(random_engine) + 100.0) * 0.005;
    else result.z = 1.0;

    return result;
}