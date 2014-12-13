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
#include "view_api.hpp"
#include "utility_classes.hpp"

#include "script/script_module.hpp"
#include "script/argument_stream.hpp"
#include "script/script_delegates.hpp"
#include "script/script_userdata.hpp"

#include "client/scene/action_scene.hpp"
#include "client/scene/view_context.hpp"

namespace ts
{
    namespace script_api
    {
        using namespace script;

        using scene::View_context;

        SQInteger getViewList(HSQUIRRELVM vm);
        SQInteger getScreenSize(HSQUIRRELVM vm);

        static const Static_function_definition view_functions[] =
        {
            { "getViewList", getViewList },
            { "getScreenSize", getScreenSize }
        };

        struct View
        {
            std::size_t index = 0;

            static SQInteger getCamera(HSQUIRRELVM vm);           
            static SQInteger getViewport(HSQUIRRELVM vm);
            static SQInteger setViewport(HSQUIRRELVM vm);
        };

        struct Camera
        {
            std::size_t index = 0;

            static SQInteger getZoomLevel(HSQUIRRELVM vm);
            static SQInteger setZoomLevel(HSQUIRRELVM vm);
        };

        static const Member_function_definition view_member_functions[] =
        {
            { "getCamera", View::getCamera },
            { "getViewport", View::getViewport },

            { "setViewport", View::setViewport }
        };

        static const Member_function_definition camera_member_functions[] =
        {
            { "getZoomLevel", Camera::getZoomLevel },
            { "setZoomLevel", Camera::setZoomLevel }
        };

        template <>
        struct Delegate_traits<View>
        {
            static Range<const Member_function_definition*> member_functions();
        };

        template <>
        struct Delegate_traits<Camera>
        {
            static Range<const Member_function_definition*> member_functions();
        };

        struct View_reader
        {
            View_reader(View& result);

            bool operator()(HSQUIRRELVM vm, SQInteger index) const;

        private:
            View& result_;
        };

        struct Camera_reader
        {
            Camera_reader(Camera& result);

            bool operator()(HSQUIRRELVM vm, SQInteger index) const;

        private:
            Camera& result_;
        };
    }
}

ts::script::API_definition ts::script_api::view_api(scene::Action_scene* action_scene)
{
    API_definition api_def;
    api_def.static_functions.assign(std::begin(view_functions), std::end(view_functions));

    api_def.delegates.push_back(create_delegate_definition<View>());
    api_def.delegates.push_back(create_delegate_definition<Camera>());

    api_def.interfaces.push_back(make_interface(action_scene));
    api_def.interfaces.push_back(make_interface(action_scene->view_context()));

    return api_def;
}

SQInteger ts::script_api::getViewList(HSQUIRRELVM vm)
{
    auto view_context = get_interface<scene::View_context>(vm);
    sq_newarray(vm, view_context->view_count());

    View view;
    for (view.index = 0; view.index != view_context->view_count(); ++view.index)
    {
        sq_pushinteger(vm, view.index);
        make_userdata(vm, view).push();
        sq_set(vm, -3);
    }

    return 1;
}

SQInteger ts::script_api::getScreenSize(HSQUIRRELVM vm)
{
    auto view_context = get_interface<scene::View_context>(vm);
    auto screen_size = view_context->screen_size();

    push_vector2(vm, screen_size);
    return 1;
}

ts::Range<const ts::script_api::Member_function_definition*> ts::script_api::Delegate_traits<ts::script_api::View>::member_functions()
{
    return Range<const Member_function_definition*>(std::begin(view_member_functions), std::end(view_member_functions));
}

ts::Range<const ts::script_api::Member_function_definition*> ts::script_api::Delegate_traits<ts::script_api::Camera>::member_functions()
{
    return Range<const Member_function_definition*>(std::begin(camera_member_functions), std::end(camera_member_functions));
}

// View member functions

SQInteger ts::script_api::View::getCamera(HSQUIRRELVM vm)
{
    View view;

    Argument_stream argument_stream(vm);
    argument_stream(View_reader(view));
    if (argument_stream)
    {
        Camera camera;
        camera.index = view.index;
        auto camera_udata = make_userdata(vm, camera);
        camera_udata.push();
        return 1;
    }

    else
    {
        report_argument_errors(get_module_by_vm(vm), argument_stream);
        return 0;
    }    
}

SQInteger ts::script_api::View::getViewport(HSQUIRRELVM vm)
{    
    View view;

    Argument_stream argument_stream(vm);
    argument_stream(View_reader(view));
    if (argument_stream)
    {
        auto view_context = get_interface<View_context>(vm);

        auto view_port = view_context->view(view.index).view_port;
        Vector2<double> screen_size = view_context->screen_size();

        view_port.left *= screen_size.x;
        view_port.top *= screen_size.y;
        view_port.width *= screen_size.x;
        view_port.height *= screen_size.y;

        push_rect(vm, view_port);
        return 1;

        return SQ_ERROR;
    }

    else
    {
        report_argument_errors(get_module_by_vm(vm), argument_stream);
        return 0;
    }    
}

SQInteger ts::script_api::View::setViewport(HSQUIRRELVM vm)
{
    View view;
    Double_rect viewport_rect;

    Argument_stream argument_stream(vm);
    argument_stream(View_reader(view));
    argument_stream(Rect_reader(viewport_rect));

    if (argument_stream)
    {
        auto view_context = get_interface<View_context>(vm);

        Vector2<double> screen_size = view_context->screen_size();
        viewport_rect.left /= screen_size.x;
        viewport_rect.top /= screen_size.y;
        viewport_rect.width /= screen_size.x;
        viewport_rect.height /= screen_size.y;

        view_context->view(view.index).view_port = viewport_rect;
    }

    else
    {
        report_argument_errors(get_module_by_vm(vm), argument_stream);
    }

    return 0;
}


// Camera functions
SQInteger ts::script_api::Camera::getZoomLevel(HSQUIRRELVM vm)
{
    Camera camera;

    Argument_stream argument_stream(vm);
    argument_stream(Camera_reader(camera));
    if (argument_stream)
    {
        auto view_context = get_interface<View_context>(vm);
        auto zoom_level = view_context->view(camera.index).camera.zoom_level();
        sq_pushfloat(vm, static_cast<SQFloat>(zoom_level));
        return 1;
    }

    else
    {
        report_argument_errors(get_module_by_vm(vm), argument_stream);
        return 0;
    }
}


SQInteger ts::script_api::Camera::setZoomLevel(HSQUIRRELVM vm)
{
    Camera camera;
    double zoom_level;

    Argument_stream argument_stream(vm);
    argument_stream(Camera_reader(camera));
    argument_stream(Numeric_reader(zoom_level));

    if (argument_stream)
    {
        auto view_context = get_interface<View_context>(vm);
        auto& game_cam = view_context->view(camera.index).camera;
        game_cam.set_zoom_level(zoom_level);
    }

    else
    {
        report_argument_errors(get_module_by_vm(vm), argument_stream);
    }

    return 0;
}


ts::script_api::View_reader::View_reader(View& result)
    : result_(result)
{}

bool ts::script_api::View_reader::operator()(HSQUIRRELVM vm, SQInteger index) const
{
    Userdata<View> view_udata;
    Userdata_reader<View> udata_reader(view_udata);
    if (udata_reader(vm, index))
    {
        auto view_context = get_interface<View_context>(vm);
        if (view_udata->index < view_context->view_count())
        {
            result_ = *view_udata;
            return true;
        }
    }

    return false;
}

ts::script_api::Camera_reader::Camera_reader(Camera& result)
    : result_(result)
{}

bool ts::script_api::Camera_reader::operator()(HSQUIRRELVM vm, SQInteger index) const
{
    Userdata<Camera> camera_udata;
    Userdata_reader<Camera> udata_reader(camera_udata);
    if (udata_reader(vm, index))
    {
        auto view_context = get_interface<View_context>(vm);
        if (camera_udata->index < view_context->view_count())
        {
            result_ = *camera_udata;
            return true;
        }
    }

    return false;
}