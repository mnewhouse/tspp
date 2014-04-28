#pragma once

#ifndef GUI_ELEMENTS_ARROW_HPP
#define GUI_ELEMENTS_ARROW_HPP

#include "vertex_array.hpp"

namespace ts
{
    namespace gui
    {
        namespace elements
        {
            class Left_arrow
                : public Vertex_element
            {
            public:
                Left_arrow(double size, sf::Color base_color, sf::Color hover_color)
                    : Vertex_element(Vector2i(size, size), make_vertices(size, base_color),
                    make_vertices(size, hover_color))
                {}

            private:
                sf::VertexArray make_vertices(double size, sf::Color color)
                {
                    sf::VertexArray vertices(sf::Triangles, 3);

                    vertices[0].position = sf::Vector2f(0.0, size * 0.5);
                    vertices[1].position = sf::Vector2f(size, 0.0);
                    vertices[2].position = sf::Vector2f(size, size);

                    for (auto i = 0; i != 3; ++i) vertices[i].color = color;

                    return vertices;
                }
            };

            
            class Right_arrow
                : public Vertex_element
            {
            public:
                Right_arrow(double size, sf::Color base_color, sf::Color hover_color)
                    : Vertex_element(Vector2i(size, size), make_vertices(size, base_color),
                    make_vertices(size, hover_color))
                {}

            private:
                sf::VertexArray make_vertices(double size, sf::Color color)
                {
                    sf::VertexArray vertices(sf::Triangles, 3);

                    vertices[0].position = sf::Vector2f(size, size * 0.5);
                    vertices[1].position = sf::Vector2f(0.0, 0.0);
                    vertices[2].position = sf::Vector2f(0.0, size);

                    for (auto i = 0; i != 3; ++i) vertices[i].color = color;

                    return vertices;
                }
            };

            class Up_arrow
                : public Vertex_element
            {
            public:
                Up_arrow(double size, sf::Color base_color, sf::Color hover_color)
                    : Vertex_element(Vector2i(size, size), make_vertices(size, base_color),
                    make_vertices(size, hover_color))
                {}

            private:
                sf::VertexArray make_vertices(double size, sf::Color color)
                {
                    sf::VertexArray vertices(sf::Triangles, 3);

                    vertices[0].position = sf::Vector2f(size * 0.5, 0.0);
                    vertices[1].position = sf::Vector2f(0.0, size);
                    vertices[2].position = sf::Vector2f(size, size);

                    for (auto i = 0; i != 3; ++i) vertices[i].color = color;

                    return vertices;
                }
            };

            class Down_arrow
                : public Vertex_element
            {
            public:
                Down_arrow(double size, sf::Color base_color, sf::Color hover_color)
                    : Vertex_element(Vector2i(size, size), make_vertices(size, base_color),
                    make_vertices(size, hover_color))
                {}

            private:
                sf::VertexArray make_vertices(double size, sf::Color color)
                {
                    sf::VertexArray vertices(sf::Triangles, 3);

                    vertices[0].position = sf::Vector2f(size * 0.5, size);
                    vertices[1].position = sf::Vector2f(0.0, 0.0);
                    vertices[2].position = sf::Vector2f(size, 0.0);

                    for (auto i = 0; i != 3; ++i) vertices[i].color = color;

                    return vertices;
                }
            };

        }
    }
}

#endif

