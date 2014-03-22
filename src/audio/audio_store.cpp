#include "audio_store.hpp"

ts::audio::Audio_handle ts::audio::Audio_store::operator[](const std::string& file_name)
{
    auto it = loaded_samples_.find(file_name);
    if (it != loaded_samples_.end()) return it->second;

    auto audio_handle = std::make_shared<sf::SoundBuffer>();

    if (!audio_handle->loadFromFile(file_name))
    {
        throw Load_error(file_name);
    }

    auto result = loaded_samples_.insert(std::make_pair(file_name, audio_handle));
    return audio_handle;
}