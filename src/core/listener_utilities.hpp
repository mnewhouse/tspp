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

#ifndef LISTENER_UTILITIES_HPP
#define LISTENER_UTILITIES_HPP

namespace ts
{
    namespace core
    {
        // Listener_host.
        // To be used as a base class.
        template <typename ListenerType>
        class Listener_host
        {
        public:
            using host_type = Listener_host<ListenerType>;

            void add_listener(ListenerType* listener) const
            {
                if (callback_guard_ != 0)
                {
                    mutations_.push_back(std::make_pair(listener, Add));
                }

                else
                {
                    listeners_.push_back(listener);
                }               
            }

            void remove_listener(ListenerType* listener) const
            {
                if (callback_guard_ != 0)
                {
                    mutations_.push_back(std::make_pair(listener, Remove));
                }

                else
                {
                    listeners_.erase(std::remove(listeners_.begin(), listeners_.end(), listener), listeners_.end());
                }                
            }

            std::size_t listener_count() const
            {
                return listeners_.size();
            }

            template <typename... Params, typename... Args>
            void call_listeners(void(ListenerType::*func)(Params...), Args&&... args) const
            {
                {
                    Callback_guard guard(this);

                    for (auto listener : listeners_)
                    {
                        (listener->*func)(std::forward<Args>(args)...);
                    }
                }

                if (callback_guard_ == 0)
                {
                    apply_mutations();
                }                
            }

        protected:
            ~Listener_host() = default;

        private:
            void apply_mutations() const
            {
                for (const auto mutation : mutations_)
                {
                    if (mutation.second == Add)
                    {
                        listeners_.push_back(mutation.first);
                    }

                    else
                    {
                        listeners_.erase(std::remove(listeners_.begin(), listeners_.end(), mutation.first), listeners_.end());
                    }
                }

                mutations_.clear();
            }

            enum Mutation
            {
                Add,
                Remove
            };

            struct Callback_guard
            {
                explicit Callback_guard(const host_type* host)
                    : host_(host)
                {
                    ++host_->callback_guard_;
                }

                Callback_guard(const Callback_guard&) = delete;
                Callback_guard& operator=(const Callback_guard&) = delete;

                ~Callback_guard()
                {
                    --host_->callback_guard_;
                }

                const host_type* host_;
            };

            // Oh boy, the m-word. Somewhat justified because adding or removing a listener does not affect
            // state of the derived class.
            mutable std::size_t callback_guard_ = 0;
            mutable std::vector<ListenerType*> listeners_;
            mutable std::vector<std::pair<ListenerType*, Mutation>> mutations_;
        };

        template <typename ListenerType>
        class Scoped_listener
            : public ListenerType
        {
        public:
            using host_type = Listener_host<ListenerType>;

            explicit Scoped_listener(const host_type* host = nullptr)
                : host_(host)
            {
                if (host_)
                {
                    host_->add_listener(this);
                }
            }

            Scoped_listener(const Scoped_listener<ListenerType>& other)
                : host_(other.host)
            {
                if (host_)
                {
                    host_->add_listener(this);
                }
            }

            Scoped_listener<ListenerType>& operator=(const Scoped_listener<ListenerType>& rhs)
            {
                start_listening(rhs.host_);
            }

            void start_listening(const Listener_host<ListenerType>* host)
            {
                if (host_)
                {
                    host_->remove_listener(this);
                }

                host_ = host;
                if (host_)
                {
                    host_->add_listener(this);
                }
            }

            void stop_listening()
            {
                if (host_)
                {
                    host_->remove_listener(this);
                    host_ = nullptr;
                }
            }

            const host_type* listener_host() const
            {
                return host_;
            }

        protected:
            ~Scoped_listener()
            {
                if (host_)
                {
                    host_->remove_listener(this);
                }
            }

        private:
            const host_type* host_;
        };
    }
}

#endif