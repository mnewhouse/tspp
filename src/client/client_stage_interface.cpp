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

#include "game/stage_interface.hpp"

class ts::client::Stage_interface::Impl
    : public game::Stage_interface
{
public:
    Impl(Message_center* message_center);

    void update(std::size_t frame_duration);
    void launch_action();

    Message_center* message_center_;
    std::unique_ptr<Stage_conductor> stage_conductor_;
};

ts::client::Stage_interface::Impl::Impl(Message_center* message_center)
: message_center_(message_center)
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
    stage_conductor_ = std::make_unique<Stage_conductor>(message_center_, stage());
}

ts::client::Stage_interface::Stage_interface(Message_center* message_center)
: impl_(std::make_unique<Impl>(message_center))
{
}

ts::client::Stage_interface::~Stage_interface()
{
}

const ts::game::Stage_loader* ts::client::Stage_interface::async_load_stage(const cup::Stage_data& stage_data, std::function<void(const action::Stage*)> completion_callback)
{
    return impl_->async_load_stage(stage_data, completion_callback);
}

const ts::action::Stage* ts::client::Stage_interface::stage() const
{
    return impl_->stage();
}

void ts::client::Stage_interface::clean_stage()
{
    impl_->stage_conductor_ = nullptr;

    impl_->clean_stage();
}

void ts::client::Stage_interface::poll_loader()
{
    impl_->poll_loader();
}

void ts::client::Stage_interface::update(std::size_t frame_duration)
{
    impl_->update(frame_duration);
}

void ts::client::Stage_interface::launch_action()
{
    impl_->launch_action();
}
