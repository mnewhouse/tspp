#ifndef GRAPHICS_RENDER_SCENE_HPP
#define GRAPHICS_RENDER_SCENE_HPP

#include "renderer.hpp"

#include <memory>


namespace ts
{

    namespace graphics
    {

        class Render_scene
        {
        public:
            Render_scene() : visible_(true) {}

            void render_if_visible(Render_target& render_target) { if (visible()) render(render_target); }

            void set_visible(bool visible) { visible_ = visible; }
            bool visible() const { return visible_; }

        private:
            virtual void render(Render_target& render_target) = 0;

            bool visible_;
        };

        struct Background
            : public Render_scene
        {
        public:
            template <typename ConcreteType>
            Background(const std::shared_ptr<ConcreteType>& scene)
                : scene_(std::static_pointer_cast<Render_scene>(scene))
            {}

            Background() {}

            virtual void render(Render_target& render_target) override
            {
                scene_->render_if_visible(render_target);
            }

            operator bool() const
            {
                return scene_.operator bool();
            }

        private:
            std::shared_ptr<Render_scene> scene_;
        };

    }

}

#endif