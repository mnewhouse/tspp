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

#include "cup/stage_assembler.hpp"
#include "cup/cup_listener.hpp"
#include "cup/cup.hpp"

namespace ts
{
    namespace server
    {
        namespace impl
        {
            class Script_loader;
        }
    }
}

class ts::server::Stage_interface::Impl
    : public game::Stage_interface, public cup::Cup_listener
{
public:
    Impl(Message_center* message_center, cup::Cup* cup);
    ~Impl();

    void launch_action();
    void update(std::size_t frame_duration);
    virtual void on_state_change(cup::Cup_state old_state, cup::Cup_state new_state) override;

    cup::Cup* cup_;
    Message_center* message_center_;

    std::unique_ptr<Stage_conductor> stage_conductor_;
};

ts::server::Stage_interface::Impl::Impl(Message_center* message_center, cup::Cup* cup)
: cup_(cup),
  message_center_(message_center)
{
    cup->add_cup_listener(this);
}

ts::server::Stage_interface::Impl::~Impl()
{
    cup_->remove_cup_listener(this);
}

void ts::server::Stage_interface::Impl::update(std::size_t frame_duration)
{
    poll();

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

void ts::server::Stage_interface::Impl::on_state_change(cup::Cup_state old_state, cup::Cup_state new_state)
{
    if (new_state == cup::Cup_state::Initializing)
    {
        // Gather stage data.
        auto stage_data = cup::assemble_stage(*cup_);
        cup_->initialize_action(stage_data);
    }
}

ts::server::Stage_interface::Stage_interface(Message_center* message_center, cup::Cup* cup)
   : impl_(std::make_unique<Impl>(message_center, cup))
{
}

ts::server::Stage_interface::~Stage_interface()
{
}

const ts::game::Stage_loader* ts::server::Stage_interface::async_load_stage(const cup::Stage_data& stage_data, std::function<void(const action::Stage*)> completion_callback)
{
    return impl_->async_load_stage(stage_data, completion_callback);
}

void ts::server::Stage_interface::launch_action()
{
    impl_->launch_action();
}

void ts::server::Stage_interface::clean_stage()
{
    impl_->clean_stage();
    impl_->stage_conductor_ = nullptr;
}

void ts::server::Stage_interface::update(std::size_t frame_duration)
{
    impl_->update(frame_duration);
}

const ts::action::Stage* ts::server::Stage_interface::stage() const
{
    return impl_->stage();
}
