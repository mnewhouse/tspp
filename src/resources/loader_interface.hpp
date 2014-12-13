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

#pragma once

#ifndef LOADING_INTERFACE_HPP
#define LOADING_INTERFACE_HPP

namespace ts
{
    namespace resources
    {
        // A generic wrapper for asynchronous loading, to be inherited from.

        template <typename StateType, typename ResultType>
        class Generic_loader
        {
        public:
            Generic_loader(StateType state = StateType());

            void poll();

            bool is_loading() const;
            bool is_completed() const;

            ResultType transfer_result();

            StateType state() const;

            // Progress is in the range [0.0, 1.0]
            double progress() const;

            template <typename F>
            void set_state_change_handler(F&& func);

            template <typename F>
            void set_completion_handler(F&& func);

        protected:
            ~Generic_loader() = default;

            template <typename F, typename... Args>
            void async_load(F&& func, Args&&... args);

            void set_state(StateType state);
            void set_progress(double progress);

        private:
            std::future<ResultType> future_;
            std::atomic<StateType> state_;
            std::atomic<double> progress_;
            StateType last_state_;

            std::function<void(StateType)> state_change_handler_;
            std::function<void()> completion_handler_;
        };
    }
}

template <typename StateType, typename ResultType>
ts::resources::Generic_loader<StateType, ResultType>::Generic_loader(StateType state)
    : state_(state),
    last_state_(state)
{
}

template <typename StateType, typename ResultType>
template <typename F, typename... Args>
void ts::resources::Generic_loader<StateType, ResultType>::async_load(F&& function, Args&&... args)
{
    future_ = std::async(std::launch::async, std::forward<F>(function), std::forward<Args>(args)...);
}

template <typename StateType, typename ResultType>
template <typename F>
void ts::resources::Generic_loader<StateType, ResultType>::set_state_change_handler(F&& function)
{
    state_change_handler_ = std::forward<F>(function);
}

template <typename StateType, typename ResultType>
template <typename F>
void ts::resources::Generic_loader<StateType, ResultType>::set_completion_handler(F&& function)
{
    completion_handler_ = std::forward<F>(function);
}

template <typename StateType, typename ResultType>
void ts::resources::Generic_loader<StateType, ResultType>::poll()
{
    if (state_ != last_state_)
    {
        if (state_change_handler_)
        {
            state_change_handler_(state_);
        }

        last_state_ = state_;
    }

    if (is_completed() && completion_handler_)
    {
        auto handler = std::move(completion_handler_);
        handler();
    }
}

template <typename StateType, typename ResultType>
void ts::resources::Generic_loader<StateType, ResultType>::set_state(StateType state)
{
    state_ = state;
}

template <typename StateType, typename ResultType>
StateType ts::resources::Generic_loader<StateType, ResultType>::state() const
{
    return state_;
}

template <typename StateType, typename ResultType>
void ts::resources::Generic_loader<StateType, ResultType>::set_progress(double progress)
{
    progress_ = progress;
}

template <typename StateType, typename ResultType>
double ts::resources::Generic_loader<StateType, ResultType>::progress() const
{
    return progress_;
}

template <typename StateType, typename ResultType>
bool ts::resources::Generic_loader<StateType, ResultType>::is_loading() const
{
    return future_.valid();
}

template <typename StateType, typename ResultType>
bool ts::resources::Generic_loader<StateType, ResultType>::is_completed() const
{
    return is_loading() && future_.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
}

template <typename StateType, typename ResultType>
ResultType ts::resources::Generic_loader<StateType, ResultType>::transfer_result()
{
    try
    {
        return future_.get();
    }

    catch (...)
    {
        future_ = {};
        throw;
    }
}


#endif