/*
* Turbo Sliders++
* Copyright (C) 2013-2014 Martin Newhouse
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "stdinc.hpp"
#include "sound_effect_controller.hpp"


ts::audio::Sound_effect_controller::Sound_effect_controller(resources::Audio_settings audio_settings)
: audio_settings_(audio_settings)
{
    sound_queue_.resize(audio_settings.effects_channels);

    for (auto& sound : sound_queue_)
    {
        sound.sound = std::make_unique<sf::Sound>();
    }
}

ts::audio::Sound_effect_handle ts::audio::Sound_effect_controller::play_sound(const Audio_handle& audio_handle, int priority, Sound_properties properties)
{
    // Needs to be the *first* element with priority == `priority`
    // High priority comes first.

    auto insert_position = std::lower_bound(sound_queue_.begin(), sound_queue_.end(), priority,
        [](const Sound_effect& effect, int priority)
    {
        return effect.priority > priority;
    });

    if (insert_position == sound_queue_.end())
    {
        return Sound_effect_handle();
    }

    std::rotate(insert_position, sound_queue_.end() - 1, sound_queue_.end());

    auto& sound_effect = *insert_position;
    sound_effect.effect_id = ++effect_id_;
    sound_effect.priority = priority;
        
    auto& sound = *sound_effect.sound;   

    properties.volume = std::min(properties.volume, 1.0f);

    sound.setBuffer(*audio_handle);
    sound.setLoop(properties.looped);
    sound.setVolume(properties.volume * properties.volume * 100.0f);
    sound.setPitch(properties.pitch);
    sound.setAttenuation(properties.attenuation);
    sound.setMinDistance(properties.min_distance);
    sound.setPosition({ properties.position.x, properties.position.y, 0.0f });
    sound.setPlayingOffset(sf::seconds(properties.offset));
    sound.setRelativeToListener(properties.relative_to_listener);
    sound.play();

    return Sound_effect_handle(sound_effect.effect_id);
}

void ts::audio::Sound_effect_controller::stop_sound(Sound_effect_handle sound_handle)
{
    auto effect_id = sound_handle.effect_id_;

    auto it = std::find_if(sound_queue_.begin(), sound_queue_.end(), 
        [effect_id](const Sound_effect& effect)
    {
        return effect.effect_id == effect_id;
    });

    if (it != sound_queue_.end())
    {
        it->sound->stop();
    }
}

ts::audio::Sound_effect_handle ts::audio::Sound_effect_controller::play_looped_sound(const Audio_handle& audio_handle, int priority, Sound_properties properties)
{
    properties.looped = true;
    return play_sound(audio_handle, priority, properties);
}

void ts::audio::Sound_effect_controller::update()
{
    for (auto& sound_effect : sound_queue_)
    {
        if (sound_effect.sound->getStatus() != sf::Sound::Playing)
        {
            sound_effect.priority = 0;
            sound_effect.effect_id = 0;
        }
    }

    auto priority_comparison = [](const Sound_effect& a, const Sound_effect& b)
    {
        return a.priority > b.priority;
    };

    std::sort(sound_queue_.begin(), sound_queue_.end(), priority_comparison);
}

void ts::audio::Sound_effect_controller::set_sound_position(Sound_effect_handle sound, Vector2<double> position)
{
    if (sound)
    {
        auto it = std::find_if(sound_queue_.begin(), sound_queue_.end(), [sound](const Sound_effect& effect)
        {
            return effect.effect_id == sound.effect_id_;
        });

        if (it != sound_queue_.end())
        {
            it->sound->setPosition(static_cast<float>(position.x), static_cast<float>(position.y), 0.0);
        }
    }
}

void ts::audio::Sound_effect_controller::set_sound_volume(Sound_effect_handle sound, double volume)
{
    if (sound)
    {
        auto it = std::find_if(sound_queue_.begin(), sound_queue_.end(), [sound](const Sound_effect& effect)
        {
            return effect.effect_id == sound.effect_id_;
        });

        if (it != sound_queue_.end())
        {
            it->sound->setVolume(static_cast<float>(volume * volume * 100.0));
        }
    }
}


ts::audio::Sound_effect_handle::Sound_effect_handle(std::size_t effect_id)
    : effect_id_(effect_id)
{
}

ts::audio::Sound_effect_handle::operator bool() const
{
    return effect_id_ != 0;
}

ts::audio::Sound_effect_controller::Sound_effect::Sound_effect()
  : sound(std::make_unique<sf::Sound>())
{
}

ts::audio::Sound_effect_controller::Sound_effect::Sound_effect(Sound_effect&& other)
: sound(std::move(other.sound)),
  effect_id(other.effect_id),
  priority(other.priority)
{
    other.effect_id = 0;
}

ts::audio::Sound_effect_controller::Sound_effect& ts::audio::Sound_effect_controller::Sound_effect::operator=(Sound_effect&& other)
{
    sound = std::move(other.sound);
    effect_id = other.effect_id;
    priority = other.priority;

    other.effect_id = 0;
    return *this;
}