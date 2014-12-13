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

#ifndef GENERIC_LOADER_HPP
#define GENERIC_LOADER_HPP

#include "loading_interface.hpp"

namespace ts
{
    namespace resources
    {
        // A generic wrapper for asynchronous loading, to be inherited from.
        

        template <typename StateType, typename ResultType>
        class Generic_loader
            : public Loading_interface
        {
        public:
            Generic_loader(StateType state = StateType(), double max_progress = 1.0);

            void poll();

            ResultType transfer_result();
            StateType state() const;

            template <typename F>
            void set_state_change_handler(F&& func);

            template <typename F>
            void set_completion_handler(F&& func);

            virtual utf8_string progress_string() const override;

        protected:
            ~Generic_loader() = default;

            template <typename F, typename... Args>
            void async_load(F&& func, Args&&... args);

            void set_state(StateType state);

        private:
            bool is_completed() const;

            std::future<ResultType> future_;

            std::atomic<StateType> state_;
            StateType last_state_;

            std::function<void(StateType)> state_change_handler_;
            std::function<void()> completion_handler_;
        };
    }
}

 template <typename StateType, typename ResultType>
 ts::resources::Generic_loader<StateType, ResultType>::Generic_loader(StateType state, double max_progress)
     : Loading_interface(max_progress),
       state_(state),
       last_state_(state)
 {
 }

 template <typename StateType, typename ResultType>
 template <typename F, typename... Args>
 void ts::resources::Generic_loader<StateType, ResultType>::async_load(F&& function, Args&&... args)
 {
     set_loading(true);
     set_finished(false);
     set_progress(0.0);

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

     if (is_completed())
     {
         set_finished(true);

         if (completion_handler_)
         {
             auto handler = std::move(completion_handler_);
             handler();
         }
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
 ts::utf8_string ts::resources::Generic_loader<StateType, ResultType>::progress_string() const
 {
     return to_string(state());
 }

 template <typename StateType, typename ResultType>
 bool ts::resources::Generic_loader<StateType, ResultType>::is_completed() const
 {
     return loading() && future_.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
 }

 template <typename StateType, typename ResultType>
 ResultType ts::resources::Generic_loader<StateType, ResultType>::transfer_result()
 {
     try 
     {
         set_loading(false);
         return future_.get();
     }

     catch (...)
     {
         future_ = {};
         throw;
     }
 }


#endif