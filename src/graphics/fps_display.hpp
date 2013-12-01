#ifndef GRAPHICS_FPS_DISPLAY_HPP
#define GRAPHICS_FPS_DISPLAY_HPP

#include "fps_counter.hpp"
#include "render_scene.hpp"

namespace ts
{

    namespace graphics
    {

        class FPS_Display
            : public Render_scene
        {
        public:
            virtual void render(Render_target& render_target) override;

        private:
            mutable FPS_Counter fps_counter_;
        };

    }

}

#endif