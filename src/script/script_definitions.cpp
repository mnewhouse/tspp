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

#include "script_definitions.hpp"
#include "script_functions.hpp"
#include "world_interface.hpp"
#include "control_point_interface.hpp"

#include <angelscript.h>

using namespace ts::script::functions;

namespace ts
{
    namespace script
    {
        void register_entity_definitions(asIScriptEngine* as_engine, const char* type_name)
        {
            as_engine->RegisterObjectType(type_name, 0, asOBJ_REF);
            as_engine->RegisterObjectMethod(type_name, "Vector2f getVelocity()", asFUNCTION(Entity_getVelocity), asCALL_CDECL_OBJFIRST);
            as_engine->RegisterObjectMethod(type_name, "Vector2f getPosition()", asFUNCTION(Entity_getPosition), asCALL_CDECL_OBJFIRST);
            as_engine->RegisterObjectMethod(type_name, "double getSpeed()", asFUNCTION(Entity_getSpeed), asCALL_CDECL_OBJFIRST);
            as_engine->RegisterObjectMethod(type_name, "double getRotation()", asFUNCTION(Entity_getRotation), asCALL_CDECL_OBJFIRST);

            as_engine->RegisterObjectBehaviour(type_name, asBEHAVE_ADDREF, "void f()", asFUNCTION(Entity_addRef), asCALL_CDECL_OBJFIRST);
            as_engine->RegisterObjectBehaviour(type_name, asBEHAVE_RELEASE, "void f()", asFUNCTION(Entity_release), asCALL_CDECL_OBJFIRST);
        };

        void register_entity_control_point_functions(asIScriptEngine* as_engine, const char* type_name)
        {
            as_engine->RegisterObjectMethod(type_name, decl::Entity_setControlPoint,
                                            asFUNCTION(Entity_setControlPoint), asCALL_CDECL_OBJFIRST);

            as_engine->RegisterObjectMethod(type_name, decl::Entity_setControlPoint,
                                            asFUNCTION(Entity_ignoreControlPoints), asCALL_CDECL_OBJFIRST);
        }
    }
}

void ts::script::register_utility_definitions(Engine* engine)
{
    auto as_engine = engine->engine();

    auto mask = asOBJ_VALUE | asOBJ_APP_CLASS_CDAK;

    as_engine->RegisterObjectType("Vector2i", sizeof(Vector2<std::int32_t>), mask);
    as_engine->RegisterObjectType("Vector2f", sizeof(Vector2<double>), mask);

    as_engine->RegisterObjectProperty("Vector2i", "int32 x", asOFFSET(Vector2<std::int32_t>, x));
    as_engine->RegisterObjectProperty("Vector2i", "int32 y", asOFFSET(Vector2<std::int32_t>, y));

    as_engine->RegisterObjectProperty("Vector2f", "double x", asOFFSET(Vector2<double>, x));
    as_engine->RegisterObjectProperty("Vector2f", "double y", asOFFSET(Vector2<double>, y));

    as_engine->RegisterObjectMethod("Vector2i", "Vector2i& opAssign(const Vector2i& in)", 
                                    asMETHODPR(Vector2<std::int32_t>, operator=, (const Vector2<std::int32_t>&), Vector2<std::int32_t>&), asCALL_THISCALL);

    as_engine->RegisterObjectMethod("Vector2f", "Vector2f& opAssign(const Vector2f& in)", 
                                    asMETHODPR(Vector2<double>, operator=, (const Vector2<double>&), Vector2<double>&), asCALL_THISCALL);


    as_engine->RegisterObjectBehaviour("Vector2i", asBEHAVE_CONSTRUCT, "void f(int32 x, int32 y)", asFUNCTION(Vector2i_construct), asCALL_CDECL_OBJLAST);
    as_engine->RegisterObjectBehaviour("Vector2i", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(Vector2i_default_construct), asCALL_CDECL_OBJLAST);
    as_engine->RegisterObjectBehaviour("Vector2i", asBEHAVE_CONSTRUCT, "void f(const Vector2i &in)", asFUNCTION(Vector2i_copy_construct), asCALL_CDECL_OBJLAST);

    as_engine->RegisterObjectBehaviour("Vector2f", asBEHAVE_CONSTRUCT, "void f(double x, double y)", asFUNCTION(Vector2f_construct), asCALL_CDECL_OBJLAST);
    as_engine->RegisterObjectBehaviour("Vector2f", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(Vector2f_default_construct), asCALL_CDECL_OBJLAST);
    as_engine->RegisterObjectBehaviour("Vector2f", asBEHAVE_CONSTRUCT, "void f(const Vector2f &in)", asFUNCTION(Vector2f_copy_construct), asCALL_CDECL_OBJLAST);

    as_engine->RegisterObjectBehaviour("Vector2i", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(Vector2i_destruct), asCALL_CDECL_OBJLAST);
    as_engine->RegisterObjectBehaviour("Vector2f", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(Vector2f_destruct), asCALL_CDECL_OBJLAST);
}

void ts::script::register_control_point_definitions(Engine* engine, Control_point_interface* cp_interface)
{
    auto as_engine = engine->engine();

    as_engine->SetUserData(cp_interface, ControlPointInterface_udata);

    as_engine->RegisterObjectType(decl::ControlPoint, 0, asOBJ_REF);
    as_engine->RegisterObjectMethod(decl::ControlPoint, decl::ControlPoint_getId,
                                    asFUNCTION(ControlPoint_getId), asCALL_CDECL_OBJFIRST);


    as_engine->RegisterInterface(decl::ControlPointListener);
    as_engine->RegisterInterfaceMethod(decl::ControlPointListener, decl::ControlPointListener_onControlPointHit);
    as_engine->RegisterInterfaceMethod(decl::ControlPointListener, decl::ControlPointListener_onControlAreaEnter);
    as_engine->RegisterInterfaceMethod(decl::ControlPointListener, decl::ControlPointListener_onControlAreaLeave);

    as_engine->RegisterGlobalFunction(decl::addControlPointListener, asMETHOD(Control_point_interface, addEventListener),
        asCALL_THISCALL_ASGLOBAL, cp_interface);

    as_engine->RegisterGlobalFunction(decl::removeControlPointListener, asMETHOD(Control_point_interface, addEventListener),
        asCALL_THISCALL_ASGLOBAL, cp_interface);

    as_engine->RegisterObjectBehaviour(decl::ControlPoint, asBEHAVE_ADDREF, "void f()",
                                       asFUNCTION(ControlPoint_addRef), asCALL_CDECL_OBJFIRST);

    as_engine->RegisterObjectBehaviour(decl::ControlPoint, asBEHAVE_RELEASE, "void f()",
                                       asFUNCTION(ControlPoint_release), asCALL_CDECL_OBJFIRST);

    as_engine->RegisterGlobalFunction(decl::getControlPointById, asMETHOD(Control_point_interface, get_control_point_by_id),
                                      asCALL_THISCALL_ASGLOBAL, cp_interface);

}

void ts::script::register_world_definitions(Engine* engine, World_interface* world_interface)
{
    auto as_engine = engine->engine();

    as_engine->SetUserData(world_interface, WorldInterface_udata);

    register_entity_definitions(as_engine, "Entity");
    register_entity_definitions(as_engine, "Car");

    as_engine->RegisterGlobalFunction("void startGameTimer()", asMETHOD(World_interface_object, startGameTimer),
                                      asCALL_THISCALL_ASGLOBAL, world_interface);
    as_engine->RegisterGlobalFunction("uint64 getGameTime()", asMETHOD(World_interface_object, getGameTime),
                                      asCALL_THISCALL_ASGLOBAL, world_interface);

    as_engine->RegisterInterface("IWorldListener");
    as_engine->RegisterInterfaceMethod("IWorldListener", "void onTick(uint64)");
    as_engine->RegisterInterfaceMethod("IWorldListener", "void onUpdate()");

    as_engine->RegisterGlobalFunction("void addEventListener(IWorldListener@)", asMETHOD(World_interface, addEventListener),
                                      asCALL_THISCALL_ASGLOBAL, world_interface);

    as_engine->RegisterGlobalFunction("void removeEventListener(IWorldListener@)", asMETHOD(World_interface, removeEventListener),
                                      asCALL_THISCALL_ASGLOBAL, world_interface);

    as_engine->RegisterObjectType("EntityIterator", sizeof(Entity_iterator), asOBJ_VALUE | asOBJ_POD);

    as_engine->RegisterObjectBehaviour("EntityIterator", asBEHAVE_CONSTRUCT, "void f()",
                                       asFUNCTION(EntityIterator_construct), asCALL_CDECL_OBJFIRST);

    as_engine->RegisterObjectMethod("EntityIterator", "EntityIterator& opPreInc()",
                                    asFUNCTION(EntityIterator_increment<world::Entity>), asCALL_CDECL_OBJFIRST);

    as_engine->RegisterObjectMethod("EntityIterator", "EntityIterator& opPreDec()",
                                    asFUNCTION(EntityIterator_increment<world::Entity>), asCALL_CDECL_OBJFIRST);

    as_engine->RegisterObjectMethod("EntityIterator", "Entity& opCall() const",
                                    asFUNCTION(EntityIterator_invoke), asCALL_CDECL_OBJFIRST);

    as_engine->RegisterObjectMethod("EntityIterator", "bool valid() const",
                                    asFUNCTION(EntityIterator_valid), asCALL_CDECL_OBJFIRST);


}

void ts::script::register_control_definitions(Engine* engine, ts::controls::Control_center* control_center)
{
    auto as_engine = engine->engine();

    as_engine->RegisterGlobalFunction("void disableGlobalControl(const string& in)", asMETHOD(Control_center_object, disableGlobalControl), 
                                   asCALL_THISCALL_ASGLOBAL, control_center);

    as_engine->RegisterGlobalFunction("void enableGlobalControl(const string& in)", asMETHOD(Control_center_object, enableGlobalControl), 
                                   asCALL_THISCALL_ASGLOBAL, control_center);
}


void ts::script::register_display_definitions(Engine* engine, HUD_overlay* hud)
{
    auto as_engine = engine->engine();

    as_engine->RegisterEnum("Font");
    as_engine->RegisterEnumValue("Font", "FONT_SANS", 0);
    as_engine->RegisterEnumValue("Font", "FONT_MONO", 1);


    as_engine->RegisterObjectType("TextDisplay", 0, asOBJ_REF);

    as_engine->RegisterObjectBehaviour("TextDisplay", asBEHAVE_FACTORY, "TextDisplay@ f(const string& in, uint32 character_size, Font)",
                                       asMETHOD(HUD_object, createTextDisplay), asCALL_THISCALL_ASGLOBAL, hud);

    as_engine->RegisterObjectBehaviour("TextDisplay", asBEHAVE_ADDREF, "void f()", asFUNCTION(TextDisplay_addRef),
                                       asCALL_CDECL_OBJLAST);

    as_engine->RegisterObjectBehaviour("TextDisplay", asBEHAVE_RELEASE, "void f()", asFUNCTION(TextDisplay_release),
                                       asCALL_CDECL_OBJLAST);

    as_engine->RegisterObjectMethod("TextDisplay", "void setPosition(const Vector2f& in)", asFUNCTION(TextDisplay_setPosition), asCALL_CDECL_OBJLAST);
    as_engine->RegisterObjectMethod("TextDisplay", "void setColor(uint32 color)", asFUNCTION(TextDisplay_setColor), asCALL_CDECL_OBJLAST);
    as_engine->RegisterObjectMethod("TextDisplay", "void setCharacterSize(uint32 character_size)", asFUNCTION(TextDisplay_setCharacterSize), asCALL_CDECL_OBJLAST);
    as_engine->RegisterObjectMethod("TextDisplay", "void setText(const string& in)", asFUNCTION(TextDisplay_setText), asCALL_CDECL_OBJLAST);

    as_engine->RegisterObjectMethod("TextDisplay", "Vector2f getPosition()", asFUNCTION(TextDisplay_getPosition), asCALL_CDECL_OBJLAST);
    as_engine->RegisterObjectMethod("TextDisplay", "Vector2f getDimensions()", asFUNCTION(TextDisplay_getDimensions), asCALL_CDECL_OBJLAST);
    as_engine->RegisterObjectMethod("TextDisplay", "uint32 getCharacterSize()", asFUNCTION(TextDisplay_getCharacterSize), asCALL_CDECL_OBJLAST);
    as_engine->RegisterObjectMethod("TextDisplay", "uint32 getColor()", asFUNCTION(TextDisplay_getColor), asCALL_CDECL_OBJLAST);
    as_engine->RegisterObjectMethod("TextDisplay", "string getText()", asFUNCTION(TextDisplay_getText), asCALL_CDECL_OBJLAST);
}