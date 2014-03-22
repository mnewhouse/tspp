#include "engine_sounds.hpp"

#include <SFML/Audio.hpp>

#include "world/car.hpp"
namespace {
    static const double base_pitch = 0.33;
}

void ts::audio::Engine_sound_controller::on_car_create(world::Car* car)
{
    const auto& car_definition = car->car_definition();
    auto audio_buffer = audio_store_[car_definition.engine_sample];

    engine_sounds_.emplace_back();

    auto& engine_sound = engine_sounds_.back();
    engine_sound.first = car;

    if (audio_buffer)
    {
        engine_sound.second = std::make_unique<sf::Sound>(*audio_buffer);
        engine_sound.second->play();
        engine_sound.second->setLoop(true);
        engine_sound.second->setPitch(base_pitch);
        engine_sound.second->setVolume(base_pitch * 100.0);
    }
}

void ts::audio::Engine_sound_controller::update(std::size_t ticks)
{
    auto frame_duration = ticks * 0.001;

    for (auto& sound : engine_sounds_)
    {
        auto car = sound.first;        

        auto pitch_factor = car->speed() / car->top_speed();
        auto new_pitch = base_pitch + (1.0 - base_pitch) * pitch_factor;

        if (!car->control_state(controls::Control::Accelerate))
        {
            new_pitch = std::max(base_pitch + (new_pitch - base_pitch) * 0.67, sound.second->getPitch() - frame_duration);
        }

        sound.second->setPitch(new_pitch);
        sound.second->setVolume(new_pitch * 100.0);
    }
}

void ts::audio::Engine_sound_controller::on_entity_destroy(world::Entity* entity)
{
    auto pred = [entity](const Engine_sound& sound)
    {
        return sound.first == entity;
    };

    engine_sounds_.erase(std::remove_if(engine_sounds_.begin(), engine_sounds_.end(), pred), engine_sounds_.end());
}