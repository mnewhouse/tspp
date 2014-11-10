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
#include "server_stage_interface.hpp"
#include "server_stage_conductor.hpp"
#include "server_messages.hpp"

#include "game/stage_interface.hpp"

#include "script/interfaces/server_script_interface.hpp"

class ts::server::Stage_interface::Impl
    : public game::Stage_interface
{
public:
    Impl(Message_center* message_center);
    ~Impl();

    void launch_action();
    void update(std::size_t frame_duration);

    void load_scripts(const cup::Stage_data& stage_data, const action::Stage* stage);

    Message_center* message_center_;

    std::unique_ptr<Stage_conductor> stage_conductor_;
    std::unique_ptr<script_api::Server_interface> script_interface_;
};

ts::server::Stage_interface::Impl::Impl(Message_center* message_center)
: message_center_(message_center)
{
}

ts::server::Stage_interface::Impl::~Impl()
{
}

void ts::server::Stage_interface::Impl::load_scripts(const cup::Stage_data& stage_data, const action::Stage* stage)
{
    script_interface_ = std::make_unique<script_api::Server_interface>(stage);

    for (auto resource : stage_data.script_resources)
    {
        auto module = script_interface_->create_module(resource->name());
        for (const auto& file : resource->server_scripts())
        {
            module->do_file(file);
        }
    }
}

void ts::server::Stage_interface::Impl::update(std::size_t frame_duration)
{
    if (stage_conductor_)
    {
        stage_conductor_->update(frame_duration);
    }
}

void ts::server::Stage_interface::Impl::launch_action()
{
    stage_conductor_ = std::make_unique<Stage_conductor>(message_center_, stage());

    stage()->launch_game();
}

ts::server::Stage_interface::Stage_interface(Message_center* message_center)
   : impl_(std::make_unique<Impl>(message_center))
{
}

ts::server::Stage_interface::~Stage_interface()
{
}

const ts::game::Stage_loader* ts::server::Stage_interface::async_load_stage(const cup::Stage_data& stage_data, std::function<void(const action::Stage*)> completion_callback)
{
    auto script_loader = [=](const action::Stage* stage)
    {
        impl_->load_scripts(stage_data, stage);
    };

    return impl_->async_load_stage(stage_data, script_loader, completion_callback);
}

void ts::server::Stage_interface::launch_action()
{
    impl_->launch_action();
}

void ts::server::Stage_interface::clean_stage()
{
    impl_->stage_conductor_ = nullptr;
    impl_->script_interface_ = nullptr;

    impl_->clean_stage();    
}

void ts::server::Stage_interface::update(std::size_t frame_duration)
{
    impl_->update(frame_duration);
}

void ts::server::Stage_interface::poll_loader()
{
    impl_->poll_loader();
}

const ts::action::Stage* ts::server::Stage_interface::stage() const
{
    return impl_->stage();
}

ts::cup::Stage_data ts::server::Stage_interface::stage_data() const
{
    return impl_->stage()->stage_data();
}