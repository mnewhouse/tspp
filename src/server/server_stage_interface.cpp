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
    Impl(cup::Cup* cup);
    ~Impl();

private:
    virtual void on_state_change(cup::Cup_state old_state, cup::Cup_state new_state) override;

    cup::Cup* cup_;
};

ts::server::Stage_interface::Impl::Impl(cup::Cup* cup)
: cup_(cup)
{
    cup->add_cup_listener(this);
}

ts::server::Stage_interface::Impl::~Impl()
{
    cup_->remove_cup_listener(this);
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

ts::server::Stage_interface::Stage_interface(cup::Cup* cup)
   : impl_(std::make_unique<Impl>(cup))
{
}

ts::server::Stage_interface::~Stage_interface()
{
}

const ts::game::Stage_loader* ts::server::Stage_interface::async_load_stage(const cup::Stage_data& stage_data, std::function<void(const action::Stage*)> completion_callback)
{
    return impl_->async_load_stage(stage_data, completion_callback);
}

void ts::server::Stage_interface::poll()
{
    impl_->poll();
}

void ts::server::Stage_interface::clean_stage()
{
    impl_->clean_stage();
}

void ts::server::Stage_interface::update(std::size_t frame_duration)
{
    impl_->update(frame_duration);
}

const ts::action::Stage* ts::server::Stage_interface::stage() const
{
    return impl_->stage();
}