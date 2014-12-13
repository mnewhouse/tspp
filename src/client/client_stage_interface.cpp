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
#include "client_stage_interface.hpp"
#include "client_stage_conductor.hpp"

#include "action/stage_interface.hpp"

class ts::client::Stage_interface::Impl
    : public action::Stage_interface
{
public:
    Impl(Message_center* message_center, resources::Network_settings* network_settings);

    void update(std::size_t frame_duration);
    void launch_action();

    Message_center* message_center_;
    resources::Network_settings* network_settings_;
    std::unique_ptr<Stage_conductor> stage_conductor_;
};

ts::client::Stage_interface::Impl::Impl(Message_center* message_center, resources::Network_settings* network_settings)
: message_center_(message_center),
  network_settings_(network_settings)
{
}

void ts::client::Stage_interface::Impl::update(std::size_t frame_duration)
{
    if (stage_conductor_)
    {
        stage_conductor_->update(frame_duration);
    }
}

void ts::client::Stage_interface::Impl::launch_action()
{
    stage_conductor_ = std::make_unique<Stage_conductor>(message_center_, stage(), network_settings_);
}

ts::client::Stage_interface::Stage_interface(Message_center* message_center, resources::Network_settings* network_settings)
: impl_(std::make_unique<Impl>(message_center, network_settings))
{
}

ts::client::Stage_interface::~Stage_interface()
{
}

const ts::resources::Loading_interface* ts::client::Stage_interface::async_load_stage(const cup::Stage_data& stage_data, std::function<void(const action::Stage*)> completion_callback)
{
    return impl_->async_load_stage(stage_data, completion_callback);
}

ts::action::Stage_interface* ts::client::Stage_interface::base()
{
    return impl_.get();
}

const ts::action::Stage* ts::client::Stage_interface::stage() const
{
    return impl_->stage();
}

void ts::client::Stage_interface::clear()
{
    impl_->stage_conductor_ = nullptr;
    impl_->clear();
}

void ts::client::Stage_interface::poll()
{
    impl_->poll();
}

void ts::client::Stage_interface::update(std::size_t frame_duration)
{
    impl_->update(frame_duration);
}

void ts::client::Stage_interface::launch_action()
{
    impl_->launch_action();
}
