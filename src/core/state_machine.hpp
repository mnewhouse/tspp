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

#ifndef CORE_STATE_MACHINE_HPP
#define CORE_STATE_MACHINE_HPP

namespace ts
{
    namespace core
    {

        // class template State_machine
        // Provides abstractions for managing game states. It uses some magic to ensure
        // only one state of the same type can be active at a time, and it also overloads operator->
        // to allow direct function calls on the current state object.
        template <typename StateType>
        class State_machine
        {
        public:
            State_machine()
                : popped_item_count_(0)
            {}

            template <typename ConcreteType>
            void change_state(std::unique_ptr<ConcreteType> state);

            void change_state();
            void clear();
            void clear_now();

            void update();

            operator const void*()
            {
                return state_stack_.empty() ? nullptr : this;
            }

            std::size_t stack_size() const {
                return state_stack_.size();
            }

            StateType* operator->() const
            {
                return state_stack_.back().state;
            }

            bool empty() const
            {
                return state_stack_.empty();
            }

        private:
            typedef void* state_type_id;

            std::map<state_type_id, std::unique_ptr<StateType>> state_map_;

            struct Stack_entry
            {
                StateType* state;
                state_type_id type_id;
            };

            std::vector<Stack_entry> state_stack_;
            std::size_t popped_item_count_;
            std::pair<state_type_id, std::unique_ptr<StateType>> pending_state_;
        };

        template <typename StateType>
        template <typename ConcreteType>
        void State_machine<StateType>::change_state(std::unique_ptr<ConcreteType> state)
        {
            auto id = type_id<ConcreteType>();
            pending_state_.first = id;
            pending_state_.second = std::move(state);
        }

        template <typename StateType>
        void State_machine<StateType>::change_state()
        {
            ++popped_item_count_;
        }

        template <typename StateType>
        void State_machine<StateType>::clear()
        {
            popped_item_count_ = state_stack_.size();
        }

        template <typename StateType>
        void State_machine<StateType>::clear_now()
        {
            state_stack_.clear();
            state_map_.clear();
        }

        template <typename StateType>
        void State_machine<StateType>::update()
        {
            bool mutated = popped_item_count_ != 0 || pending_state_.second;

            for (; popped_item_count_ != 0 && !state_stack_.empty(); --popped_item_count_) {
                state_map_.erase(state_stack_.back().type_id);
                state_stack_.pop_back();
            }

            if (pending_state_.second) {
                state_stack_.erase(std::remove_if(state_stack_.begin(), state_stack_.end(),
                    [this](Stack_entry state) { return state.type_id == pending_state_.first; }
                ), state_stack_.end());

                Stack_entry stack_entry;
                stack_entry.type_id = pending_state_.first;
                stack_entry.state = &*pending_state_.second;;

                state_stack_.push_back(stack_entry);
                state_map_[pending_state_.first] = std::move(pending_state_.second);
            }

            if (mutated && !state_stack_.empty())
            {
                state_stack_.back().state->activate();
            }
        }
    }
}

#endif