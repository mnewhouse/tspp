#ifndef AUDIO_ENGINE_SOUNDS_HPP
#define AUDIO_ENGINE_SOUNDS_HPP

#include "audio_store.hpp"
#include "world/entity_listener.hpp"

#include <vector>
#include <memory>

namespace ts
{
    namespace audio
    {
        class Engine_sound_controller
            : public world::Entity_listener
        {
        public:
            virtual void on_car_create(world::Car* car) override;
            virtual void on_entity_destroy(world::Entity* entity) override;

            void update(std::size_t ticks);

        private:
            // pair hack - I would use a struct, but MSVC still doesn't have 
            // compiler-generated move operations. 
            using Engine_sound = std::pair<const world::Car*, std::unique_ptr<sf::Sound>>;

            std::vector<Engine_sound> engine_sounds_;
            Audio_store audio_store_;
        };
    }
}

#endif