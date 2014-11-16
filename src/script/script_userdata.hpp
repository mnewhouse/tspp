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

#ifndef SCRIPT_USERDATA_HPP
#define SCRIPT_USERDATA_HPP

#include "userdata_impl.hpp"
#include "delegate_definition.hpp"
#include "script_utility.hpp"

namespace ts
{
    namespace script
    {        
        namespace userdata
        {
            struct construct_t {};
            static const construct_t construct;
        }

        struct Generic_userdata;

        struct Userdata_storage_base
        {
            virtual Generic_userdata clone(HSQUIRRELVM vm) const = 0;
            virtual SQUserPointer data() = 0;
            virtual ~Userdata_storage_base() = default;
        };

        template <typename T>
        struct Userdata_storage
            : public Userdata_storage_base
        {
            virtual ~Userdata_storage()
            {
                static_cast<T*>(static_cast<void*>(&data_))->~T();
            }

            template <typename... Args>
            Userdata_storage(userdata::construct_t, Args&&... args)
            {
                new (&data_)T(std::forward<Args>(args)...);
            }

            virtual SQUserPointer data() override
            {
                return static_cast<SQUserPointer>(&data_);
            }

            virtual Generic_userdata clone(HSQUIRRELVM vm) const override;

            std::aligned_storage_t<sizeof(T)> data_;
        };

        struct Userdata_base
        {
            virtual ~Userdata_base() = default;

            virtual SQUserPointer type_tag() const = 0;
            virtual void push() const = 0;
            virtual HSQUIRRELVM vm_handle() const = 0;

            virtual Generic_userdata clone(HSQUIRRELVM target_vm) const = 0;
        };

        template <typename T>
        SQInteger userdata_release_hook(SQUserPointer ptr, SQInteger size)
        {
           reinterpret_cast<T*>(ptr)->~T();
           return 1;
        }
        
        template <typename T, typename... Args>
        Object_handle initialize_userdata(HSQUIRRELVM vm, Args&&... args)
        {
            Stack_guard stack_guard(vm);

            auto pointer = sq_newuserdata(vm, sizeof(Userdata_storage<T>));
            sq_settypetag(vm, -1, userdata_type_tag<T>());
            sq_setreleasehook(vm, -1, userdata_release_hook<Userdata_storage<T>>);

            auto target_pointer = static_cast<Userdata_storage<T>*>(static_cast<Userdata_storage_base*>(pointer));
            new (target_pointer) Userdata_storage<T>(userdata::construct, std::forward<Args>(args)...);

            return Object_handle(vm, -1);
        }

        struct Userdata_internals
            : public Object_handle
        {
            Userdata_internals()
            : storage(nullptr),
              type_tag(nullptr)
            {
            }

            Userdata_internals(Object_handle object_handle)
            : Object_handle(object_handle),
              storage(nullptr),
              type_tag(nullptr)
            {
                Stack_guard stack_guard(vm());

                push();

                SQUserPointer pointer;
                sq_getuserdata(vm(), -1, &pointer, &type_tag);

                storage = static_cast<Userdata_storage_base*>(pointer);
            }

            Userdata_internals(HSQUIRRELVM vm, SQInteger index)
                : Object_handle(vm, index, OT_USERDATA),
                  storage(nullptr),
                  type_tag(nullptr)
            {
                if (*this)
                {
                    SQUserPointer pointer;
                    sq_getuserdata(vm, index, &pointer, &type_tag);

                    storage = static_cast<Userdata_storage_base*>(pointer);
                }
            }

            Userdata_storage_base* storage;
            SQUserPointer type_tag;
        };
        

        template <typename T>
        class Userdata
            : public Userdata_base
        {
        public:
            Userdata() = default;

            template <typename... Args>
            explicit Userdata(userdata::construct_t, HSQUIRRELVM vm, Args&&... args)
                : internals_(initialize_userdata<T>(vm, std::forward<Args>(args)...)),
                  pointer_(static_cast<T*>(internals_.storage->data()))
            {
                Stack_guard stack_guard(vm);
                internals_.push();

                auto delegate = get_delegate_table<T>(vm);
                if (delegate)
                {
                    delegate.push();
                    sq_setdelegate(vm, -2);
                }
            }

            Userdata(HSQUIRRELVM vm, SQInteger index)
                : internals_(vm, index), 
                  pointer_(impl::userdata_cast<T>(internals_.storage->data(), internals_.type_tag))
            {
            }

            virtual void push() const override
            {
                internals_.push();
            }

            virtual SQUserPointer type_tag() const override
            {
                return userdata_type_tag<T>();
            }

            virtual Generic_userdata clone(HSQUIRRELVM vm) const override;

            T* operator->() const
            {
                return pointer_;
            }

            T& operator*() const
            {
                return *pointer_;
            }

            virtual HSQUIRRELVM vm_handle() const override
            {
                return internals_.vm();
            }

            explicit operator bool() const
            {
                return pointer_ != nullptr;
            }

        private:
            Userdata_internals internals_;
            T* pointer_ = nullptr;
        };

        template <>
        class Userdata<void>
            : public Userdata_base
        {
        public:
            Userdata() = default;

            Userdata(HSQUIRRELVM vm, SQInteger index)
                : internals_(vm, index)
            {
            }

            virtual void push() const override
            {
                internals_.push();
            }

            virtual SQUserPointer type_tag() const override
            {
                return nullptr;
            }

            virtual HSQUIRRELVM vm_handle() const override
            {
                return internals_.vm();
            }

            virtual Generic_userdata clone(HSQUIRRELVM vm) const override;

        private:
            Userdata_internals internals_;
        };

        template <typename T>
        using Userdata_handle = std::shared_ptr<Userdata<T>>;
        using Generic_userdata_handle = std::shared_ptr<Userdata_base>;

        struct Generic_userdata
        {
        public:
            Generic_userdata() = default;

            Generic_userdata(HSQUIRRELVM vm, SQInteger integer)
                : self_(std::make_shared<Userdata<void>>(vm, integer))
            {
            }

            template <typename T>
            Generic_userdata(Userdata_handle<T> handle)
                : self_(std::move(handle))
            {}

            template <typename T>
            Generic_userdata(Userdata<T> userdata)
                : self_(std::make_shared<Userdata<T>>(std::move(userdata)))
            {                
            }

            template <typename T>
            explicit Generic_userdata(T value)
                : self_(std::make_shared<Userdata_model<T>>(userdata::construct, std::move(value)))
            {
            }

            void push() const
            {
                self_->push();
            }

        private:
            HSQUIRRELVM vm_handle()
            {
                return self_->vm_handle();
            }

            Generic_userdata_handle self_;

            template <typename T>
            friend Userdata<T> userdata_cast(const Generic_userdata& userdata);
        };

        template <typename T>
        Userdata<typename std::decay<T>::type> make_userdata(HSQUIRRELVM vm, T&& value)
        {
            return Userdata<typename std::decay<T>::type>(userdata::construct, vm, std::forward<T>(value));
        }

        template <typename T, typename... Args>
        Userdata<typename std::decay<T>::type> construct_userdata(HSQUIRRELVM vm, Args&&... args)
        {
            return Userdata<typename std::decay<T>::type>(userdata::construct, vm, std::forward<Args>(args)...);
        }

        template <typename T>
        Userdata<T> userdata_cast(const Generic_userdata& userdata)
        {
            auto vm = userdata.vm_handle();

            Stack_guard stack_guard(vm);
            userdata.push();

            return Userdata<T>(vm, -1);
        }

        template <typename T>
        struct Userdata_reader
        {
            Userdata_reader(Userdata<T>& result)
              : result(result)
            {
            }

            Userdata<T>& result;

            bool operator()(HSQUIRRELVM vm, SQInteger index) const
            {
                result = Userdata<T>(vm, index);
                return !!result;
            }
        };

        template <typename T>
        struct Userdata_forwarder
        {
            template <typename U>
            explicit Userdata_forwarder(U&& value, typename std::enable_if<!std::is_same<Userdata_forwarder<T>, typename std::decay<U>::type>::value>::type* = nullptr)
                : value(std::forward<U>(value))
            {}

            T value;
        };

        namespace impl
        {
            template <typename T>
            struct clone_helper
            {
                Generic_userdata operator()(HSQUIRRELVM vm, const T& object) const
                {
                    return clone_impl<T>(vm, object, 0);
                }

                template <typename U, typename std::enable_if<std::is_copy_constructible<U>::value>::type = 0>
                Generic_userdata clone_impl(HSQUIRRELVM vm, const T& object, int) const
                {
                    return make_userdata(vm, object);
                }

                template <typename U>
                Generic_userdata clone_impl(HSQUIRRELVM vm, const T& object, ...) const
                {
                    return Generic_userdata();
                }
            };
        }

        inline Generic_userdata Userdata<void>::clone(HSQUIRRELVM vm) const
        {
            return internals_.storage->clone(vm);
        }

        template <typename T>
        Generic_userdata Userdata<T>::clone(HSQUIRRELVM vm) const
        {
            return internals_.storage->clone(vm);
        }


        template <typename T>
        Generic_userdata Userdata_storage<T>::clone(HSQUIRRELVM vm) const
        {
            impl::clone_helper<T> clone_helper;
            return clone_helper(vm, *static_cast<const T*>(static_cast<const void*>(&data_)));
        }

        template <typename T>
        Userdata_forwarder<T&&> forward_as_userdata(T&& value)
        {
            return Userdata_forwarder<T&&>(std::forward<T>(value));
        }
    }
}

#endif