#ifndef AUDIO_AUDIO_STORE_HPP
#define AUDIO_AUDIO_STORE_HPP

#include "core/handle.hpp"

#include <map>

#include <SFML/Audio.hpp>

namespace ts
{
    namespace audio
    {
        using Audio_handle = std::shared_ptr<sf::SoundBuffer>;
        struct Load_error
            : std::exception
        {
            Load_error(const std::string& file_name) {}
        };

        class Audio_store
        {
        public:
            Audio_handle operator[](const std::string& file_name);

        private:
            std::map<std::string, Audio_handle> loaded_samples_;
            
        };
    }
}

#endif