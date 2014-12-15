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

#include "../common/utility_classes.hpp"
#include "../common/entity_api.hpp"

#include "../common_api_errors.hpp"

#include "script/argument_stream.hpp"
#include "script/script_delegates.hpp"
#include "script/script_userdata.hpp"

#include "client/scene/scene_interface.hpp"
#include "client/scene/action_scene.hpp"
#include "client/scene/view_context.hpp"

namespace ts
{
    namespace script_api
    {
        using namespace script;

        using scene::View_context;

        Pointer_handle<const scene::Scene_interface> get_scene_interface(HSQUIRRELVM vm);

        SQInteger getView(HSQUIRRELVM vm);

        namespace view
        {
            SQInteger getSubViews(HSQUIRRELVM vm);
            SQInteger getSize(HSQUIRRELVM vm);
        }

        static const Static_function_definition view_functions[] =
        {
            { "View", getView }
        };

        static const Member_function_definition view_member_functions[] =
        {
            { "getSubViews", view::getSubViews },
            { "getSize", view::getSize }
        };

        struct SubView
        {
            std::size_t index = 0;
            View_context* view_context;

            static SQInteger getCamera(HSQUIRRELVM vm);           
            static SQInteger getViewport(HSQUIRRELVM vm);
            static SQInteger setViewport(HSQUIRRELVM vm);
        };

        struct Camera
        {
            std::size_t index = 0;
            View_context* view_context = nullptr;

            static SQInteger getZoomLevel(HSQUIRRELVM vm);
            static SQInteger setZoomLevel(HSQUIRRELVM vm);
            
            static SQInteger getPosition(HSQUIRRELVM vm);
            static SQInteger setPosition(HSQUIRRELVM vm);

            static SQInteger getTarget(HSQUIRRELVM vm);
            static SQInteger setTarget(HSQUIRRELVM vm);

            static SQInteger followTarget(HSQUIRRELVM vm);
            static SQInteger isTargetFollowed(HSQUIRRELVM vm);
        };

        static const Member_function_definition sub_view_member_functions[] =
        {
            { "getCamera", SubView::getCamera },
            { "getViewport", SubView::getViewport },

            { "setViewport", SubView::setViewport }
        };

        static const Member_function_definition camera_member_functions[] =
        {
            { "getZoomLevel", Camera::getZoomLevel },
            { "setZoomLevel", Camera::setZoomLevel },
            
            { "getPosition", Camera::getPosition },
            { "setPosition", Camera::setPosition },
            
            { "getTarget", Camera::getTarget },
            { "setTarget", Camera::setTarget },

            { "followTarget", Camera::followTarget },
            { "isTargetFollowed", Camera::isTargetFollowed }

        };

        template <>
        struct Delegate_traits<View_context*>
        {
            static Range<const Member_function_definition*> member_functions();
        };

        template <>
        struct Delegate_traits<SubView>
        {
            static Range<const Member_function_definition*> member_functions();
        };

        template <>
        struct Delegate_traits<Camera>
        {
            static Range<const Member_function_definition*> member_functions();
        };

        struct SubView_reader
        {
            SubView_reader(SubView& result);

            bool operator()(HSQUIRRELVM vm, SQInteger index) const;

        private:
            SubView& result_;
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

ts::Pointer_handle<const ts::scene::Scene_interface> ts::script_api::get_scene_interface(HSQUIRRELVM vm)
{
    return get_interface<const ts::scene::Scene_interface>(vm);
}

ts::script::API_definition ts::script_api::view_api(const scene::Scene_interface* scene_interface)
{
    API_definition api_def;
    api_def.static_functions.assign(std::begin(view_functions), std::end(view_functions));

    api_def.delegates.push_back(create_delegate_definition<View_context*>());
    api_def.delegates.push_back(create_delegate_definition<SubView>());
    api_def.delegates.push_back(create_delegate_definition<Camera>());

    api_def.interfaces.push_back(make_interface(scene_interface));

    return api_def;
}

SQInteger ts::script_api::getView(HSQUIRRELVM vm)
{
    auto action_scene = get_scene_interface(vm)->action_scene();

    if (action_scene)
    {
        auto view_udata = make_userdata(vm, action_scene->view_context());
        view_udata.push();
        return 1;
    }

    else
    {
        report_unavailable_function(vm, "View()");
        return 0;
    }
}

SQInteger ts::script_api::view::getSubViews(HSQUIRRELVM vm)
{
    Userdata<View_context*> view_udata;

    Argument_stream argument_stream(vm, "View::getSubViews");
    argument_stream(make_reader(view_udata));

    if (argument_stream)
    {
        auto view_context = *view_udata;
        sq_newarray(vm, view_context->view_count());

        SubView sub_view;
        sub_view.view_context = view_context;

        for (sub_view.index = 0; sub_view.index != view_context->view_count(); ++sub_view.index)
        {
            sq_pushinteger(vm, sub_view.index);
            make_userdata(vm, sub_view).push();
            sq_set(vm, -3);
        }

        return 1;
    }

    return 0;
}

SQInteger ts::script_api::view::getSize(HSQUIRRELVM vm)
{
    Userdata<View_context*> view_udata;

    Argument_stream argument_stream(vm, "View::getSubViews");
    argument_stream(make_reader(view_udata));

    if (argument_stream)
    {
        auto view_context = *view_udata;
        auto screen_size = view_context->screen_size();

        push_vector2(vm, screen_size);
        return 1;
    }

    return 0;
}

ts::Range<const ts::script_api::Member_function_definition*> ts::script_api::Delegate_traits<ts::script_api::View_context*>::member_functions()
{
    return make_range(std::begin(view_member_functions), std::end(view_member_functions));
}

ts::Range<const ts::script_api::Member_function_definition*> ts::script_api::Delegate_traits<ts::script_api::SubView>::member_functions()
{
    return make_range(std::begin(sub_view_member_functions), std::end(sub_view_member_functions));
}

ts::Range<const ts::script_api::Member_function_definition*> ts::script_api::Delegate_traits<ts::script_api::Camera>::member_functions()
{
    return make_range(std::begin(camera_member_functions), std::end(camera_member_functions));
}

// SubView member functions
SQInteger ts::script_api::SubView::getCamera(HSQUIRRELVM vm)
{
    SubView sub_view;

    Argument_stream argument_stream(vm, "SubView::getCamera");
    argument_stream(SubView_reader(sub_view));
    if (argument_stream)
    {
        Camera camera;
        camera.index = sub_view.index;
        camera.view_context = sub_view.view_context;

        auto camera_udata = make_userdata(vm, camera);
        camera_udata.push();
        return 1;
    }

    return 0;
}

SQInteger ts::script_api::SubView::getViewport(HSQUIRRELVM vm)
{    
    SubView sub_view;

    Argument_stream argument_stream(vm, "SubView::getViewport");
    argument_stream(SubView_reader(sub_view));
    if (argument_stream)
    {
        auto view_context = sub_view.view_context;
        auto view_port = view_context->view(sub_view.index).view_port;
        Vector2<double> screen_size = view_context->screen_size();

        view_port.left *= screen_size.x;
        view_port.top *= screen_size.y;
        view_port.width *= screen_size.x;
        view_port.height *= screen_size.y;

        push_rect(vm, view_port);
        return 1;
    }

    return 0;
}

SQInteger ts::script_api::SubView::setViewport(HSQUIRRELVM vm)
{
    SubView sub_view;
    Double_rect viewport_rect;

    Argument_stream argument_stream(vm, "SubView::setViewport");
    argument_stream(SubView_reader(sub_view));
    argument_stream(Rect_reader(viewport_rect));

    if (argument_stream)
    {
        auto view_context = sub_view.view_context;

        Vector2<double> screen_size = view_context->screen_size();
        viewport_rect.left /= screen_size.x;
        viewport_rect.top /= screen_size.y;
        viewport_rect.width /= screen_size.x;
        viewport_rect.height /= screen_size.y;
        
        view_context->view(sub_view.index).view_port = viewport_rect;
    }

    return 0;
}


// Camera functions
SQInteger ts::script_api::Camera::getZoomLevel(HSQUIRRELVM vm)
{
    Camera camera;

    Argument_stream argument_stream(vm, "Camera::getZoomLevel");
    argument_stream(Camera_reader(camera));
   
    if (argument_stream)
    {
        auto view_context = camera.view_context;
        auto zoom_level = view_context->view(camera.index).camera.zoom_level();

        sq_pushfloat(vm, static_cast<SQFloat>(zoom_level));
        return 1;
    }

    return 0;
}


SQInteger ts::script_api::Camera::setZoomLevel(HSQUIRRELVM vm)
{
    Camera camera;
    double zoom_level;

    Argument_stream argument_stream(vm, "Camera::setZoomLevel");
    argument_stream(Camera_reader(camera));
    argument_stream(Numeric_reader(zoom_level));

    if (argument_stream)
    {
        auto view_context = camera.view_context;
        auto& game_cam = view_context->view(camera.index).camera;
        game_cam.set_zoom_level(zoom_level);
    }

    return 0;
}


SQInteger ts::script_api::Camera::setPosition(HSQUIRRELVM vm)
{
    Camera camera;
    Vector2<double> position;

    Argument_stream argument_stream(vm, "Camera::setPosition");
    argument_stream(Camera_reader(camera));
    argument_stream(Vector2_reader(position));

    if (argument_stream)
    {
        auto view_context = camera.view_context;
        auto& game_cam = view_context->view(camera.index).camera;
        game_cam.set_position(position);
    }

    return 0;
}


SQInteger ts::script_api::Camera::getPosition(HSQUIRRELVM vm)
{
    Camera camera;

    Argument_stream argument_stream(vm, "Camera::getPosition");
    argument_stream(Camera_reader(camera));

    if (argument_stream)
    {
        auto view_context = camera.view_context;
        auto& game_cam = view_context->view(camera.index).camera;
        push_vector2(vm, game_cam.position());
        return 1;
    }

    return 0;
}

SQInteger ts::script_api::Camera::getTarget(HSQUIRRELVM vm)
{
    Camera camera;

    Argument_stream argument_stream(vm, "Camera::getTarget");
    argument_stream(Camera_reader(camera));

    if (argument_stream)
    {
        auto view_context = camera.view_context;
        auto& game_cam = view_context->view(camera.index).camera;

        if (auto target = game_cam.target())
        {
            auto target_udata = make_entity_userdata(vm, target);
            target_udata.push();
        }

        else
        {
            sq_pushnull(vm);
        }

        return 1;
    }

    return 0;
}

SQInteger ts::script_api::Camera::setTarget(HSQUIRRELVM vm)
{
    // TODO
    return 0;
}

SQInteger ts::script_api::Camera::isTargetFollowed(HSQUIRRELVM vm)
{
    return 0;
}

SQInteger ts::script_api::Camera::followTarget(HSQUIRRELVM vm)
{
    Camera camera;
    bool follow = true;

    Argument_stream argument_stream(vm, "Camera::followTarget");
    argument_stream(Camera_reader(camera));
    argument_stream(Boolean_reader(follow), arg::optional);

    if (argument_stream)
    {
        auto view_context = camera.view_context;
        auto& game_cam = view_context->view(camera.index).camera;
        game_cam.follow_target(follow);
    }

    return 0;
}


ts::script_api::SubView_reader::SubView_reader(SubView& result)
    : result_(result)
{}

bool ts::script_api::SubView_reader::operator()(HSQUIRRELVM vm, SQInteger index) const
{
    Userdata<SubView> subview_udata;
    Userdata_reader<SubView> udata_reader(subview_udata);
    if (udata_reader(vm, index))
    {
        auto view_context = subview_udata->view_context;
        if (subview_udata->index < view_context->view_count())
        {
            result_ = *subview_udata;
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
        auto view_context = camera_udata->view_context;
        if (camera_udata->index < view_context->view_count())
        {
            result_ = *camera_udata;
            return true;
        }
    }

    return false;
}