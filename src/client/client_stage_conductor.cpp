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
#include "client_stage_conductor.hpp"

#include "action/stage_conductor.hpp"
#include "action/action_messages.hpp"

ts::client::Stage_conductor::Stage_conductor(Message_center* message_center, action::Stage* stage,
                                             resources::Network_settings* network_settings)
: Message_listener(message_center),
  stage_conductor_(std::make_unique<action::Stage_conductor>(stage)),
  network_settings_(network_settings)
{
}

ts::client::Stage_conductor::~Stage_conductor()
{
}

void ts::client::Stage_conductor::update(std::size_t frame_duration)
{
    if (!queued_game_state_.empty())
    {
        auto tp = std::chrono::high_resolution_clock::now();

        const auto& game_state = queued_game_state_.front();

        auto old_stage_time = stage_conductor_->stage_time();
        auto new_stage_time = game_state.stage_time;

        stage_conductor_->process_game_state_message(game_state);

        if (old_stage_time >= new_stage_time)
        {
            std::int32_t server_difference = old_stage_time - new_stage_time;

            if (network_settings_->min_prediction > server_difference - advance_time_ && 
                advance_time_ > static_cast<std::int32_t>(frame_duration))
            {
                // Prediction seems too high, adjust
                advance_time_ -= frame_duration;

                if (advance_time_ < network_settings_->min_prediction)
                {
                    network_settings_->min_prediction -= frame_duration;
                    network_settings_->max_prediction -= frame_duration;
                }
            }
        }

        else
        {
            // Received a newer game state than the one we had
            // Client should fix their prediction, or we could do it for them.

            advance_time_ += frame_duration;
            if (advance_time_ > network_settings_->max_prediction)
            {
                network_settings_->min_prediction += frame_duration;
                network_settings_->max_prediction += frame_duration;
            }                  
        }

        for (std::int32_t time = 0; time < advance_time_; time += frame_duration)
        {
            stage_conductor_->update(frame_duration);
        }
        
        stage_conductor_->purge_old_events(old_stage_time);
        queued_game_state_.clear();
    }

    // Just update (TM).
    stage_conductor_->update(frame_duration);
}

void ts::client::Stage_conductor::handle_message(const Server_message& server_message)
{
    const auto& message = server_message.message;

    if (server_message.message_type == Message_type::Fast)
    {
        if (action::is_control_event_message(message))
        {
            handle_control_event_message(message);
        }

        using Msg = action::Message_type;

        switch (message.type())
        {
        case Msg::game_state:
            handle_game_state_message(message);
            break;
        }
    }
}

void ts::client::Stage_conductor::handle_control_event_message(const Message& message)
{
    auto event = action::parse_control_event_message(message);
    stage_conductor_->process_control_event_message(event);
}

void ts::client::Stage_conductor::handle_game_state_message(const Message& message)
{
    auto state_message = action::parse_game_state_message(message);

    if (queued_game_state_.empty())
    {
        queued_game_state_.push_back(std::move(state_message));
    }

    else
    {
        queued_game_state_.front() = std::move(state_message);
    }
    
}
