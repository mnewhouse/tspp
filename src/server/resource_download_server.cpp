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
#include "resource_download_server.hpp"

#include "cup/download_messages.hpp"

#include "resources/car_identifier.hpp"
#include "resources/track_identifier.hpp"

#include "resources/resource_store.hpp"
#include "resources/track_store.hpp"
#include "resources/car_store.hpp"

#include "resources/track_loader.hpp"

namespace ts
{
    namespace server
    {
        downloads::Resource_assets load_track_assets(std::size_t resource_id, const resources::Track_store& track_store, const resources::Track_identifier& track_identifier);
        downloads::Resource_assets load_car_assets(std::size_t resource_id, const resources::Car_store& car_store, const resources::Car_identifier& car_identifier);

        static bool is_builtin_asset(const utf8_string& file_path);

        // Matching predicate to be used for concrete resource mappings.
        struct Match_resource_id
        {
            Match_resource_id(std::size_t resource_id)
                : resource_id(resource_id)
            {}

            template <typename ResourceType>
            bool operator()(const ResourceType& resource) const
            {
                return resource.resource_id == resource_id;
            }

        private:
            std::size_t resource_id;
        };
    }
}




struct ts::server::Resource_download_server::Track_resource
{
    std::size_t resource_id;
    resources::Track_identifier track_identifier;
};

struct ts::server::Resource_download_server::Car_resource
{
    std::size_t resource_id;
    resources::Car_identifier car_identifier;
};

struct ts::server::Resource_download_server::Concrete_resources
{
    std::vector<Track_resource> tracks;
    std::vector<Car_resource> cars;
};

ts::server::Resource_download_server::Resource_download_server(Message_center* message_center, const resources::Resource_store* resource_store)
: Message_listener(message_center),
  resource_store_(resource_store),
  message_center_(message_center),
  concrete_resources_(std::make_unique<Concrete_resources>())
{
}

ts::server::Resource_download_server::~Resource_download_server()
{
}

void ts::server::Resource_download_server::poll()
{
    // We need to check if any resources are finished loading, so
    // that we can send them to the clients that requested them.
    for (auto it = loading_resources_.begin(); it != loading_resources_.end(); )
    {
        auto& future = it->second;
        auto resource_id = it->first;

        auto status = future.wait_for(std::chrono::milliseconds(0));
        if (status == std::future_status::ready)
        {
            // Data is ready.
            auto assets = future.get();
            if (assets.resource_id_ == resource_id)
            {
                // If it's still being downloaded, we can store it away,
                // otherwise, we can can just drop it.
                // A bit sad if we loaded it for nothing, though. :(
                if (is_resource_being_downloaded(resource_id))
                {
                    auto& resource = resource_assets_[resource_id];

                    resource = std::move(assets);
                    resource_ready(resource);
                }

                else
                {
                    deallocate_resource(resource_id);
                }
            }

            else
            {
                // Returned invalid asset object. Refuse the download.
                refuse_download_request(resource_id);
            }

            // And delete the Future
            it = loading_resources_.erase(it);
        }

        else
        {
            ++it;
        }
    }
}

void ts::server::Resource_download_server::disconnect_client(const Generic_client& client)
{
    auto download_it = client_downloads_.find(client);
    if (download_it != client_downloads_.end())
    {
        auto& downloads = download_it->second;

        while (!downloads.empty())
        {
            auto resource_id = downloads.back().resource_id_;
            downloads.pop_back();

            if (!is_resource_being_downloaded(resource_id))
            {
                deallocate_resource(resource_id);
            }
        }

        client_downloads_.erase(download_it);
    }
}

void ts::server::Resource_download_server::resource_ready(const downloads::Resource_assets& assets)
{
    const auto& file_data = assets.file_data_;
    const auto& file_info = assets.file_info_;

    const std::size_t resource_id = assets.resource_id_;

    if (!assets.file_data_.empty())
    {
        const auto* file_ptr = file_data.data();
        const auto* file_end = file_data.data() + file_data.size();

        const std::uint8_t* data_ptr = file_ptr->data();
        const std::uint8_t* data_end = file_ptr->data() + file_ptr->size();

        Client_message out_message;

        // For all clients that need this resource
        for (auto& client_download : client_downloads_)
        {
            const auto& client = client_download.first;
            auto& downloads = client_download.second;

            auto search_result = std::find_if(downloads.begin(), downloads.end(),
                               [resource_id](const Download_info& download_info)
            {
                return download_info.resource_id_ == resource_id;
            });

            if (search_result != downloads.end())
            {
                // Initialize their file pointers
                search_result->data_ptr_ = data_ptr;
                search_result->data_end_ = data_end;

                search_result->file_ptr_ = file_ptr;
                search_result->file_end_ = file_end;

                // And dispatch file headers
                out_message.client = client;
                out_message.message = downloads::make_file_info_message(search_result->download_key_, file_info);
                message_center_->dispatch_message(out_message);
            }
        }
    }  
}

void ts::server::Resource_download_server::handle_message(const Client_message& client_message)
{
    if (client_message.message_type == Message_type::Reliable)
    {
        const auto& message = client_message.message;
        using Msg = downloads::Message_type;

        switch (message.type())
        {
        case Msg::track_download_request:
            handle_track_download_request(client_message);
            break;
            
        case Msg::car_download_request:
            handle_car_download_request(client_message);
            break;

        case Msg::pong:
            handle_pong_message(client_message);
            break;
        }
    }
}

void ts::server::Resource_download_server::handle_pong_message(const Client_message& client_message)
{
    const auto& client = client_message.client;
    const auto& message = client_message.message;

    const auto pong = downloads::parse_pong_message(message);

    auto client_it = client_downloads_.find(client);
    if (client_it != client_downloads_.end())
    {
        auto& downloads = client_it->second;
        auto search_result = std::find_if(downloads.begin(), downloads.end(),
                                          [pong](const Download_info& download_info)
        {
            return download_info.download_key_ == pong.download_key;
        });

        if (search_result != downloads.end())
        {
            send_file_chunk(*search_result, client);
        }
    }
}

void ts::server::Resource_download_server::handle_track_download_request(const Client_message& client_message)
{
    const auto& client = client_message.client;
    const auto request = downloads::parse_track_download_request(client_message.message);

    auto& track_resources = concrete_resources_->tracks;
 
    auto resource_it = std::find_if(track_resources.begin(), track_resources.end(),
                                    [&request](const Track_resource& track_resource)
    {
        return track_resource.track_identifier == request.track_identifier;
    });

    if (resource_it == track_resources.end())
    {        
        Track_resource track_resource;
        track_resource.track_identifier = request.track_identifier;
        track_resource.resource_id = allocate_resource_id();

        track_resources.push_back(track_resource);
        resource_it = std::prev(track_resources.end());

        async_load_track_assets(resource_it->resource_id, resource_it->track_identifier);
    }

    Download_info download_info;
    download_info.download_key_ = request.download_key;
    download_info.resource_id_ = resource_it->resource_id;

    auto& client_downloads = client_downloads_[client];
    client_downloads.push_back(download_info);
}

void ts::server::Resource_download_server::handle_car_download_request(const Client_message& client_message)
{
    const auto& client = client_message.client;
    const auto request = downloads::parse_car_download_request(client_message.message);

    auto& car_resources = concrete_resources_->cars;

    auto resource_it = std::find_if(car_resources.begin(), car_resources.end(),
                                    [&request](const Car_resource& car_resource)
    {
        return car_resource.car_identifier == request.car_identifier;
    });

    if (resource_it == car_resources.end())
    {
        Car_resource car_resource;
        car_resource.car_identifier = request.car_identifier;
        car_resource.resource_id = allocate_resource_id();

        car_resources.push_back(car_resource);
        resource_it = std::prev(car_resources.end());
    }

    Download_info download_info;
    download_info.download_key_ = request.download_key;
    download_info.resource_id_ = resource_it->resource_id;

    auto& client_downloads = client_downloads_[client];
    client_downloads.push_back(download_info);

    async_load_car_assets(resource_it->resource_id, resource_it->car_identifier);
}

void ts::server::Resource_download_server::async_load_track_assets(std::size_t resource_id, const resources::Track_identifier& track_identifier)
{
    auto future = std::async(std::launch::async, load_track_assets, resource_id, std::cref(resource_store_->track_store()), track_identifier);
    loading_resources_[resource_id] = std::move(future);
}

void ts::server::Resource_download_server::async_load_car_assets(std::size_t resource_id, const resources::Car_identifier& car_identifier)
{
    auto future = std::async(std::launch::async, load_car_assets, resource_id, std::cref(resource_store_->car_store()), car_identifier);
    loading_resources_[resource_id] = std::move(future);
}

void ts::server::Resource_download_server::refuse_download_request(std::size_t resource_id)
{
    Client_message out_message;

    auto asset_it = resource_assets_.find(resource_id);
    if (asset_it != resource_assets_.end())
    {
        const auto& file_info = asset_it->second.file_info_;

        // For all clients that are downloading
        for (const auto& client_downloads : client_downloads_)
        {
            const auto& client = client_downloads.first;
            const auto& downloads = client_downloads.second;

            // Find the ones that are downloading this resource
            auto search_result = std::find_if(downloads.begin(), downloads.end(),
                                              [resource_id](const Download_info& download_info)
            {
                return download_info.resource_id_ == resource_id;
            });

            if (search_result != downloads.end())
            {
                out_message.client = client;
                out_message.message = downloads::make_download_refusal_message(search_result->download_key_);
                message_center_->dispatch_message(out_message);
            }
        }
    }
}

void ts::server::Resource_download_server::send_file_chunk(Download_info& download_info, const Generic_client& client)
{
    // Send at most 4096 bytes of data
    auto chunk_size = std::min(download_info.data_end_ - download_info.data_ptr_, 4096);
    
    Client_message out_message;
    out_message.client = client;
    out_message.message = downloads::make_file_chunk_message(download_info.download_key_, download_info.data_ptr_, chunk_size);
    message_center_->dispatch_message(out_message);

    download_info.data_ptr_ += chunk_size;
    if (download_info.data_ptr_ == download_info.data_end_)
    {
        // End of file was reached.
        send_eof_message(download_info, client);
    }
}

void ts::server::Resource_download_server::send_eof_message(Download_info& download_info, const Generic_client& client)
{
    Client_message out_message;
    out_message.client = client;
    out_message.message = downloads::make_eof_message(download_info.download_key_);
    message_center_->dispatch_message(out_message);

    if (++download_info.file_ptr_ < download_info.file_end_)
    {
        // New file to be sent.
        auto file_ptr = download_info.file_ptr_;
        download_info.data_ptr_ = file_ptr->data();
        download_info.data_end_ = file_ptr->data() + file_ptr->size();
    }

    else
    {
        send_finish_message(client, download_info.download_key_);
    }
}

void ts::server::Resource_download_server::send_finish_message(const Generic_client& client, std::uint32_t download_key)
{
    Client_message out_message;
    out_message.client = client;
    out_message.message = downloads::make_finished_message(download_key);
    message_center_->dispatch_message(out_message);

    auto client_it = client_downloads_.find(client);
    if (client_it != client_downloads_.end())
    {
        auto& downloads = client_it->second;
        auto search_result = std::find_if(downloads.begin(), downloads.end(), 
            [download_key](const Download_info& download)
        {
            return download_key == download.download_key_;
        });

        if (search_result != downloads.end())
        {
            // Remove this download from the queue
            std::size_t resource_id = search_result->resource_id_;
            downloads.erase(search_result);

            // And if nobody else is downloading the resource anymore...
            if (!is_resource_being_downloaded(resource_id))
            {
                // Free the whole thing from memory.
                deallocate_resource(resource_id);
            }               
        }
    }
}

bool ts::server::Resource_download_server::is_resource_being_downloaded(std::size_t resource_id) const
{
    for (const auto& client_download : client_downloads_)
    {
        const auto& downloads = client_download.second;
        auto search_result = std::find_if(downloads.begin(), downloads.end(),
            [resource_id](const Download_info& download)
        {
            return download.resource_id_ == resource_id;
        });

        if (search_result != downloads.end())
        {
            return true;
        }
    }

    return false;
}

void ts::server::Resource_download_server::deallocate_resource(std::size_t resource_id)
{
    // Remove download information
    for (auto& client_downloads : client_downloads_)
    {
        auto& downloads = client_downloads.second;
        downloads.erase(std::remove_if(downloads.begin(), downloads.end(),
                       [resource_id](const Download_info& download_info)
        {
            return download_info.resource_id_ == resource_id;
        }), downloads.end());
    }

    auto& cars = concrete_resources_->cars;
    auto& tracks = concrete_resources_->tracks;

    Match_resource_id match_resource_id(resource_id);

    // Remove all track/car/etc. -> resource_id mappings
    cars.erase(std::remove_if(cars.begin(), cars.end(), match_resource_id), cars.end());
    tracks.erase(std::remove_if(tracks.begin(), tracks.end(), match_resource_id), tracks.end());

    // And remove the loaded resource itself.
    resource_assets_.erase(resource_id);
}

std::size_t ts::server::Resource_download_server::allocate_resource_id() const
{
    std::size_t id = 1;
    for (; resource_assets_.count(id) != 0 || loading_resources_.count(id) != 0; ++id) {}

    return id;
}

ts::downloads::Resource_assets ts::server::load_track_assets(std::size_t resource_id, const resources::Track_store& track_store,
                                                             const resources::Track_identifier& track_identifier)
{
    downloads::Resource_assets result;
    result.resource_id_ = 0;

    auto track_handle = track_store.get_track_by_name(track_identifier.track_name);
    if (track_handle)
    {
        result.resource_id_ = resource_id;
        
        const auto& track_path = track_handle.path();
        auto track_contents = core::read_file_contents(track_path);

        {
            auto filename = boost::filesystem::path(track_path.string()).filename().string();

            result.file_info_.emplace_back();
            result.file_info_.back().file_name = filename;
            result.file_info_.back().file_size = track_contents.size();

            result.file_data_.emplace_back(track_contents.begin(), track_contents.end());
        }
        
        resources::Track_loader track_loader;

        {
            std::istringstream stream(std::string(track_contents.begin(), track_contents.end()));

            boost::filesystem::path track_directory = boost::filesystem::path(track_path.string()).parent_path();
            track_loader.load_from_stream(stream, track_directory.string());
        }

        for (const auto& file : track_loader.assets())
        {
            if (is_builtin_asset(file)) continue;

            auto contents = core::read_file_contents(file);
            auto file_name = boost::filesystem::path(file.string()).filename().string();

            result.file_info_.emplace_back();
            result.file_info_.back().file_name = std::move(file_name);
            result.file_info_.back().file_size = contents.size();

            result.file_data_.emplace_back(contents.begin(), contents.end());
        }
    }

    return result;
}

ts::downloads::Resource_assets ts::server::load_car_assets(std::size_t resource_id, const resources::Car_store& car_store,
                                                           const resources::Car_identifier& car_identifier)
{
    downloads::Resource_assets result;
    result.resource_id_ = 0;

    auto car_handle = car_store.get_car_by_name(car_identifier.car_name);
    if (car_handle)
    {
        result.resource_id_ = resource_id;
        const utf8_string* files[] =
        {
            &car_handle->file_path, &car_handle->pattern_file, &car_handle->image_file, &car_handle->engine_sample
        };

        for (auto file_ptr : files)
        {
            if (is_builtin_asset(*file_ptr)) continue;

            auto contents = core::read_file_contents(*file_ptr);
            auto base_name = boost::filesystem::basename(file_ptr->string());

            result.file_info_.emplace_back();
            result.file_info_.back().file_name = std::move(base_name);
            result.file_info_.back().file_size = contents.size();

            result.file_data_.emplace_back();
            result.file_data_.back().assign(contents.begin(), contents.end());
        }
    }

    return result;
}

bool ts::server::is_builtin_asset(const utf8_string& path_string)
{
    boost::filesystem::path data = config::data_directory;
    boost::filesystem::path sounds = config::sound_directory;

    for (boost::filesystem::path path = path_string.string(); !path.empty(); path = path.parent_path())
    {
        if (equivalent(path, data) || equivalent(path, sounds))
        {
            return true;
        }
    }

    return false;  
}

ts::downloads::Resource_assets::Resource_assets()
: resource_id_(0)
{
}

ts::downloads::Resource_assets::Resource_assets(Resource_assets&& other)
: resource_id_(other.resource_id_),
  file_info_(std::move(other.file_info_)),
  file_data_(std::move(other.file_data_))
{
    other.resource_id_ = 0;
}

ts::downloads::Resource_assets& ts::downloads::Resource_assets::operator=(Resource_assets&& other)
{
    resource_id_ = other.resource_id_;
    file_info_ = other.file_info_;
    file_data_ = other.file_data_;

    other.resource_id_ = 0;
    return *this;
}