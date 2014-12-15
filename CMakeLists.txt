cmake_minimum_required(VERSION 2.8.12)
project(tspp)

set(STATIC_STD_LIBS OFF)
set(PNG_TESTS OFF)
set(PNG_SHARED ON)
set(PNG_STATIC OFF)
set(PNG_NO_STDIO OFF)

if(MSVC)
    set(TSPP_OS_WINDOWS 1)    
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
    
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(TSPP_OS_LINUX 1)
    add_definitions(-std=c++1y)
    add_definitions(-g)
    add_definitions(-Wall)
endif()

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

set(EXECUTABLE_NAME "tspp")

set(SRC src/main.cpp 
        src/stdinc.hpp

		src/action/action_messages.hpp
		src/action/action_messages.cpp
		src/action/stage.hpp
		src/action/stage.cpp
		src/action/stage_conductor.hpp
		src/action/stage_conductor.cpp
		src/action/stage_interface.hpp
		src/action/stage_interface.cpp
		src/action/stage_loader.hpp
		src/action/stage_loader.cpp

        src/audio/audio_store.cpp
        src/audio/audio_store.hpp
        src/audio/sound_effect_controller.hpp
        src/audio/sound_effect_controller.cpp

		src/client/chatbox_interface.hpp
		src/client/chatbox_interface.cpp
		src/client/client.hpp
		src/client/client.cpp
		src/client/client_control_interface.hpp
		src/client/client_control_interface.cpp
		src/client/client_interactions.hpp
		src/client/client_interactions.cpp
		src/client/client_interface.hpp
		src/client/client_interface.cpp
		src/client/client_messages.hpp		
		src/client/client_stage_conductor.hpp
		src/client/client_stage_conductor.cpp
		src/client/client_stage_interface.hpp
		src/client/client_stage_interface.cpp
		src/client/common_client_logic.hpp
		src/client/common_client_logic.cpp
		src/client/local_client.hpp
		src/client/local_client.cpp
		src/client/local_players.hpp
		src/client/local_players.cpp
		src/client/registration.hpp
		src/client/resource_downloader.hpp
		src/client/resource_downloader.cpp

		src/client/hud/chatbox_display.hpp
		src/client/hud/chatbox_display.cpp
		src/client/hud/hud_chatbox.hpp
		src/client/hud/hud_chatbox.cpp
				
        src/client/scene/action_scene.hpp
        src/client/scene/action_scene.cpp
		src/client/scene/audio_loader.hpp
		src/client/scene/audio_loader.cpp
        src/client/scene/camera.hpp
        src/client/scene/camera.cpp
		src/client/scene/client_script_loader.hpp
		src/client/scene/client_script_loader.cpp
		src/client/scene/client_script_conductor.hpp
		src/client/scene/client_script_conductor.cpp
		src/client/scene/collision_sounds.hpp
		src/client/scene/collision_sounds.cpp
        src/client/scene/drawable_entity.hpp
        src/client/scene/drawable_entity.cpp
		src/client/scene/engine_sounds.hpp
		src/client/scene/engine_sounds.cpp
		src/client/scene/loaded_sound_effects.hpp
		src/client/scene/loading_sequence.hpp
		src/client/scene/loading_sequence.cpp
        src/client/scene/particle_generator.hpp
        src/client/scene/particle_generator.cpp
		src/client/scene/scene.hpp
		src/client/scene/scene.cpp
		src/client/scene/scene_interface.hpp
		src/client/scene/scene_interface.cpp
		src/client/scene/scene_loader.hpp
        src/client/scene/scene_loader.cpp
		src/client/scene/skid_sounds.hpp
		src/client/scene/skid_sounds.cpp
		src/client/scene/sound_controller.hpp
		src/client/scene/sound_controller.cpp
		src/client/scene/track_builder.hpp
        src/client/scene/track_builder.cpp
		src/client/scene/view_context.hpp
		src/client/scene/view_context.cpp

		src/client/script/client_core_events.hpp
		src/client/script/client_core_events.cpp
		src/client/script/client_script_api.hpp
		src/client/script/client_script_api.cpp
		src/client/script/client_script_engine.hpp
		src/client/script/client_script_engine.cpp
        
        src/controls/control.hpp
        src/controls/control.cpp
        src/controls/control_center.hpp
        src/controls/control_center.cpp
		src/controls/control_event.hpp
		src/controls/control_event_translator.hpp
		src/controls/control_event_translator.cpp
		src/controls/control_event_processor.hpp
		src/controls/control_event_processor.cpp
        src/controls/control_interface.hpp
        src/controls/controllable.hpp
        src/controls/controllable.cpp
        src/controls/key_mapping.hpp
        src/controls/key_mapping.cpp

        src/core/bresenham.hpp
        src/core/config.hpp
        src/core/directive_reader.hpp
		src/core/line_intersection.hpp
		src/core/listener_utilities.hpp
        src/core/pointer_handle.hpp
        src/core/range.hpp
        src/core/rect.hpp
        src/core/rotation.hpp
        src/core/state_machine.hpp
        src/core/sha256.hpp
        src/core/sha256.cpp
        src/core/stream_utility.hpp
        src/core/stream_utility.cpp
        src/core/transform.hpp
        src/core/unicode.hpp
        src/core/unicode.cpp
        src/core/utf8_string.hpp
        src/core/utf8_string.cpp
        src/core/utility.hpp
        src/core/vector2.hpp              
		
		src/cup/chat_message.hpp
		src/cup/chat_message.cpp
		src/cup/chatbox_listener.hpp
		src/cup/chatbox.hpp
		src/cup/chatbox.cpp
		src/cup/cup.hpp
		src/cup/cup.cpp
		src/cup/cup_config.hpp
		src/cup/cup_config.cpp
		src/cup/cup_controller.hpp
		src/cup/cup_controller.cpp
		src/cup/cup_controller_listener.hpp
		src/cup/cup_listener.hpp
		src/cup/cup_messages.hpp
		src/cup/cup_messages.cpp
		src/cup/cup_metadata.hpp
		src/cup/download_messages.hpp
		src/cup/download_messages.cpp
		src/cup/player.hpp
		src/cup/stage_data.hpp

		
		src/game/event_dispatcher.hpp
		src/game/event_dispatcher.cpp
		src/game/event_handler.hpp
        src/game/game.hpp
        src/game/game.cpp        
        src/game/game_state.hpp
        src/game/game_state.cpp

		src/graphics/colored_text.hpp
		src/graphics/colored_text.cpp
        src/graphics/geometry.hpp
        src/graphics/geometry.cpp
        src/graphics/particle_drawer.hpp
        src/graphics/particle_drawer.cpp
        src/graphics/render_scene.hpp
        src/graphics/sprite.hpp
        src/graphics/sprite.cpp
        src/graphics/text.hpp
        src/graphics/text.cpp
        src/graphics/texture.hpp
        src/graphics/texture.cpp
        src/graphics/texture_area.hpp
        src/graphics/texture_area.cpp
        src/graphics/texture_loader.hpp
        src/graphics/texture_loader.cpp

		src/messages/message.hpp
		src/messages/message.cpp
		src/messages/message_center.hpp
		src/messages/message_dispatcher.hpp
		src/messages/message_listener.hpp
		src/messages/message_reader.hpp
		src/messages/message_reader.cpp		

		src/network/client_connection.hpp
		src/network/client_connection.cpp
		src/network/client_handle.hpp
		src/network/message_protocol.hpp
		src/network/server_connection.hpp
		src/network/server_connection.cpp

        src/resources/car_definition.hpp
        src/resources/car_definition.cpp
        src/resources/car_handle.hpp
        src/resources/car_handle.cpp
		src/resources/car_identifier.hpp
		src/resources/car_image_generator.hpp
		src/resources/car_image_generator.cpp
		src/resources/car_model.hpp
        src/resources/car_store.hpp
        src/resources/car_store.cpp
        src/resources/control_point.hpp
        src/resources/font_face.hpp
        src/resources/font_face.cpp
        src/resources/font_library.hpp
        src/resources/font_library.cpp
		src/resources/generic_loader.hpp
		src/resources/image_loader.hpp
		src/resources/image_loader.cpp
        src/resources/image_type.hpp
        src/resources/include_path.hpp
        src/resources/include_path.cpp
		src/resources/loading_interface.hpp
		src/resources/loading_interface.cpp
        src/resources/pattern.hpp
        src/resources/pattern.cpp
        src/resources/pattern_builder.hpp
        src/resources/pattern_builder.cpp
        src/resources/pattern_loader.hpp
        src/resources/pattern_loader.cpp
        src/resources/player_color.hpp
        src/resources/player_color.cpp
		src/resources/player_definition.hpp
        src/resources/player_store.hpp
        src/resources/player_store.cpp
		src/resources/resource_identifier.hpp
        src/resources/resource_store.hpp
		src/resources/resource_store.cpp
        src/resources/script_manager.hpp
        src/resources/script_manager.cpp
        src/resources/script_resource.hpp
        src/resources/script_resource.cpp
        src/resources/settings.hpp
        src/resources/settings.cpp
        src/resources/settings_copy.hpp
        src/resources/terrain_definition.hpp
        src/resources/terrain_definition.cpp
        src/resources/terrain_library.hpp
        src/resources/terrain_library.cpp
        src/resources/tile.hpp
        src/resources/tile_library.hpp
        src/resources/tile_library.cpp
        src/resources/track.hpp
        src/resources/track.cpp		
        src/resources/track_handle.hpp
        src/resources/track_handle.cpp
		src/resources/track_hash.hpp
		src/resources/track_identifier.hpp
		src/resources/track_loader.hpp
		src/resources/track_loader.cpp
		src/resources/track_metadata.hpp
		src/resources/track_metadata.cpp
        src/resources/track_store.hpp
        src/resources/track_store.cpp
        src/resources/wall_definition.hpp
        src/resources/wall_definition.cpp

		src/resources/settings/audio_settings.hpp
		src/resources/settings/car_settings.hpp
		src/resources/settings/car_settings.cpp
		src/resources/settings/input_settings.hpp
		src/resources/settings/network_settings.hpp
		src/resources/settings/player_settings.hpp
		src/resources/settings/player_settings.cpp
		src/resources/settings/script_settings.hpp
		src/resources/settings/script_settings.cpp
		src/resources/settings/track_settings.hpp
		src/resources/settings/track_settings.cpp
		src/resources/settings/video_settings.hpp
		src/resources/settings/video_settings.cpp
        
        src/resources/fonts/font_base.hpp
        src/resources/fonts/monospace.hpp
        src/resources/fonts/monospace.cpp
        src/resources/fonts/sans.hpp
        src/resources/fonts/sans.cpp
        
        src/script/argument_stream.hpp
        src/script/argument_stream.cpp
		src/script/class_definition.hpp
		src/script/class_definition.cpp
        src/script/delegate_definition.hpp
        src/script/delegate_definition.cpp
        src/script/delegate_traits.hpp
        src/script/delegates_impl.hpp
        src/script/error_level.hpp
        src/script/event_interface.hpp
        src/script/event_manager.hpp
        src/script/event_manager.cpp
        src/script/execution_context.hpp
        src/script/execution_context.cpp
		src/script/member_function.hpp
		src/script/module_handle.hpp
		src/script/module_handle.cpp
		src/script/module_loader.hpp
		src/script/module_loader.cpp
        src/script/object_handle.hpp
        src/script/object_handle.cpp
        src/script/script_api.hpp
        src/script/script_console.hpp
        src/script/script_console.cpp
        src/script/script_delegates.hpp
        src/script/script_engine.hpp
        src/script/script_engine.cpp
        src/script/script_function.hpp
        src/script/script_function.cpp
        src/script/script_interface.hpp
        src/script/script_interface.cpp
        src/script/script_module.hpp
        src/script/script_module.cpp
        src/script/script_table.hpp
        src/script/script_table.cpp
        src/script/script_userdata.hpp
        src/script/script_utility.hpp
        src/script/script_utility.cpp
        src/script/script_value.hpp
        src/script/script_value.cpp
        src/script/script_vm.hpp
        src/script/script_vm.cpp
        src/script/squirrel_include.hpp
        src/script/userdata_forwarder.hpp
        src/script/userdata_impl.hpp
        src/script/userdata_traits.hpp		
		src/script/api/client/view_api.hpp
		src/script/api/client/view_api.cpp

	    src/script/api/common/chatbox_api.hpp
		src/script/api/common/chatbox_api.cpp
		src/script/api/common/clock_api.hpp
		src/script/api/common/clock_api.cpp
		src/script/api/common/color_api.hpp
		src/script/api/common/color_api.cpp
        src/script/api/common/entity_api.hpp
        src/script/api/common/entity_api.cpp
		src/script/api/common/message_api.hpp
		src/script/api/common/message_api.cpp
        src/script/api/common/event_api.hpp
        src/script/api/common/event_api.cpp
		src/script/api/common/player_color_api.hpp
		src/script/api/common/player_color_api.cpp
		src/script/api/common/stage_api.hpp
        src/script/api/common/stage_api.cpp
		src/script/api/common/utility_classes.hpp
		src/script/api/common/utility_classes.cpp

	    src/script/api/server/car_store_api.hpp
		src/script/api/server/car_store_api.cpp
	    src/script/api/server/client_api.hpp
		src/script/api/server/client_api.cpp
		src/script/api/server/command_api.hpp
		src/script/api/server/command_api.cpp
		src/script/api/server/stage_assembler_api.hpp
		src/script/api/server/stage_assembler_api.cpp

		src/script/api/common_api_registration.hpp
		src/script/api/common_api_registration.cpp
		src/script/api/common_api_errors.hpp
		src/script/api/common_api_errors.cpp
        
        src/server/client_map.hpp
		src/server/client_map.cpp
		src/server/command_center.hpp
		src/server/command_center.cpp
		src/server/command_handler.hpp
		src/server/generic_client.hpp
		src/server/generic_client.cpp
		src/server/resource_download_server.hpp
		src/server/resource_download_server.cpp
		src/server/server.hpp
		src/server/server.cpp
		src/server/server_interaction_listener.hpp
		src/server/server_interaction_listener.cpp
		src/server/server_interactions.hpp
		src/server/server_interactions.cpp
		src/server/server_load_errors.hpp
		src/server/server_load_errors.cpp
		src/server/server_messages.hpp
		src/server/server_stage_conductor.hpp
		src/server/server_stage_conductor.cpp
		src/server/server_stage_interface.hpp
		src/server/server_stage_interface.cpp
		src/server/stage_assembler.hpp
		src/server/stage_assembler.cpp
        
        src/server/script/cup_module_manager.hpp
        src/server/script/cup_module_manager.cpp
        src/server/script/script_stage_modifier.hpp
        src/server/script/script_stage_modifier.cpp
        src/server/script/server_interaction_events.hpp
        src/server/script/server_interaction_events.cpp
        src/server/script/server_script_api.hpp
        src/server/script/server_script_api.cpp
        src/server/script/server_script_engine.hpp
        src/server/script/server_script_engine.cpp
        src/server/script/server_script_events.hpp
        src/server/script/stage_module_manager.hpp
        src/server/script/stage_module_manager.cpp		

        src/states/action_state_base.hpp
        src/states/action_state_base.cpp
		src/states/client_action_state.hpp
		src/states/client_action_state.cpp
		src/states/client_cup_state.hpp
		src/states/client_cup_state.cpp
		src/states/cup_gui.hpp
		src/states/cup_gui.cpp
        src/states/cup_setup.hpp
        src/states/cup_setup.cpp
        src/states/cup_state_base.hpp
        src/states/cup_state_base.cpp
        src/states/main_menu.hpp
        src/states/main_menu.cpp
		src/states/network_menu.hpp
		src/states/network_menu.cpp
        src/states/player_setup.hpp
        src/states/player_setup.cpp
        src/states/settings_menu.hpp
        src/states/settings_menu.cpp
		src/states/server_action_state.hpp
		src/states/server_action_state.cpp
		src/states/server_cup_state.hpp
		src/states/server_cup_state.cpp
        
        src/states/gui_definitions/background_document.hpp
        src/states/gui_definitions/background_document.cpp
        src/states/gui_definitions/window_template.hpp
        src/states/gui_definitions/window_template.cpp
        
        src/user_interface/background.hpp
        src/user_interface/background.cpp
        src/user_interface/background_style.hpp
        src/user_interface/document.hpp
        src/user_interface/document.cpp
        src/user_interface/document_handle.hpp
        src/user_interface/document_handle.cpp
        src/user_interface/element_style.hpp
        src/user_interface/event_manager.hpp        
        src/user_interface/gui_context.hpp
        src/user_interface/gui_context.cpp
        src/user_interface/gui_element.hpp
        src/user_interface/gui_element.cpp
        src/user_interface/gui_state.hpp
        src/user_interface/input_interface.hpp
        src/user_interface/input_interface.cpp
        src/user_interface/mouse.hpp
        src/user_interface/stylable_element.hpp
        
        src/user_interface/elements/edit_box.hpp
        src/user_interface/elements/edit_box.cpp
        src/user_interface/elements/elements.hpp
        src/user_interface/elements/grid_list.hpp
        src/user_interface/elements/integer_input.hpp
        src/user_interface/elements/integer_input.cpp
        src/user_interface/elements/option_set.hpp
        src/user_interface/elements/player_color_background.hpp
        src/user_interface/elements/player_color_background.cpp
        src/user_interface/elements/progress_bar.hpp
        src/user_interface/elements/progress_bar.cpp        
        src/user_interface/elements/scroll_pane.hpp
        src/user_interface/elements/scroll_pane.cpp
        src/user_interface/elements/slider_bar.hpp
        src/user_interface/elements/slider_bar.cpp
        src/user_interface/elements/text_element.hpp
        src/user_interface/elements/text_element.cpp
        src/user_interface/elements/vertical_list.hpp
        
        src/world/car.hpp
        src/world/car.cpp
        src/world/collision_bitmap.hpp
        src/world/collision_bitmap.cpp
        src/world/collision_helpers.hpp
        src/world/collisions.hpp
        src/world/collisions.cpp
        src/world/control_point_listener.hpp
        src/world/control_point_manager.hpp
        src/world/control_point_manager.cpp
        src/world/entity.hpp
        src/world/entity.cpp
        src/world/game_timer.hpp
        src/world/game_timer.cpp
        src/world/handling.hpp
        src/world/handling.cpp
        src/world/world.hpp
        src/world/world.cpp
        src/world/world_listener.hpp
)

SOURCE_GROUP(main REGULAR_EXPRESSION src/main\\.cpp)
SOURCE_GROUP(stdinc REGULAR_EXPRESSION src/stdinc\\..p)
SOURCE_GROUP(action REGULAR_EXPRESSION src/action/[^/]+)
SOURCE_GROUP(audio REGULAR_EXPRESSION src/audio/[^/]+)
SOURCE_GROUP(client REGULAR_EXPRESSION src/client/[^/]+)
SOURCE_GROUP(client\\scene REGULAR_EXPRESSION src/client/scene/[^/]+)
SOURCE_GROUP(client\\hud REGULAR_EXPRESSION src/client/hud/[^/]+)
SOURCE_GROUP(client\\script REGULAR_EXPRESSION src/client/script/[^/]+)
SOURCE_GROUP(controls REGULAR_EXPRESSION src/controls/[^/]+)
SOURCE_GROUP(core REGULAR_EXPRESSION src/core/[^/]+)
SOURCE_GROUP(cup REGULAR_EXPRESSION src/cup/[^/]+)
SOURCE_GROUP(game REGULAR_EXPRESSION src/game/[^/]+)
SOURCE_GROUP(graphics REGULAR_EXPRESSION src/graphics/[^/]+)
SOURCE_GROUP(messages REGULAR_EXPRESSION src/messages/[^/]+)
SOURCE_GROUP(network REGULAR_EXPRESSION src/network/[^/]+)
SOURCE_GROUP(resources REGULAR_EXPRESSION src/resources/[^/]+)
SOURCE_GROUP(resources\\fonts REGULAR_EXPRESSION src/resources/fonts/[^/]+)
SOURCE_GROUP(resources\\settings REGULAR_EXPRESSION src/resources/settings/[^/]+)
SOURCE_GROUP(server REGULAR_EXPRESSION src/server/[^/]+)
SOURCE_GROUP(server\\script REGULAR_EXPRESSION src/server/script/[^/]+)
SOURCE_GROUP(states REGULAR_EXPRESSION src/states/[^/]+)
SOURCE_GROUP(states\\gui_definitions REGULAR_EXPRESSION src/states/gui_definitions/[^/]+)
SOURCE_GROUP(script REGULAR_EXPRESSION src/script/[^/]+)
SOURCE_GROUP(script\\api REGULAR_EXPRESSION src/script/api/[^/]+)
SOURCE_GROUP(script\\api\\client REGULAR_EXPRESSION src/script/api/client/[^/]+)
SOURCE_GROUP(script\\api\\common REGULAR_EXPRESSION src/script/api/common/[^/]+)
SOURCE_GROUP(script\\api\\server REGULAR_EXPRESSION src/script/api/server/[^/]+)
SOURCE_GROUP(user_interface REGULAR_EXPRESSION src/user_interface/[^/]+)
SOURCE_GROUP(user_interface\\elements REGULAR_EXPRESSION src/user_interface/elements/[^/]+)
SOURCE_GROUP(world REGULAR_EXPRESSION src/world/[^/]+)

set(PRECOMPILED_SOURCE "src/stdinc.cpp")
if(MSVC)
    set(PRECOMPILED_BINARY "$(IntDir)/${PROJECT_NAME}.pch")
    
    SET_SOURCE_FILES_PROPERTIES(${PRECOMPILED_SOURCE}
                                PROPERTIES COMPILE_FLAGS "/Yc\"stdinc.hpp\" /Fp\"${PRECOMPILED_BINARY}\""
                                           OBJECT_OUTPUTS "${PRECOMPILED_BINARY}")
    SET_SOURCE_FILES_PROPERTIES(${SRC}
                                PROPERTIES COMPILE_FLAGS "/Yu\"stdinc.hpp\" /FI\"stdinc.hpp\" /Fp\"${PRECOMPILED_BINARY}\""
                                           OBJECT_DEPENDS "${PRECOMPILED_BINARY}")  
    
endif()

set(SRC ${SRC}
        ${PRECOMPILED_SOURCE})



set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_USE_MULTITHREADED ON)
set(SFML_STATIC_LIBRARIES OFF)

add_subdirectory(extlibs/squirrel3)
add_subdirectory(extlibs/freetype-2.5.3)
add_subdirectory(extlibs/zlib-1.2.8)
add_subdirectory(extlibs/libpng-1.2.51)

include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/extlibs/squirrel3/include")
include_directories(${FREETYPE_INCLUDE_DIR})
include_directories(${ZLIB_INCLUDE_DIR})
include_directories(${PNG_INCLUDE_DIR})


add_executable(${EXECUTABLE_NAME} ${SRC})

find_package(Boost REQUIRED COMPONENTS system filesystem locale)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
    add_definitions("-DHAS_BOOST")
    target_link_libraries(${EXECUTABLE_NAME} ${Boost_LIBRARIES})
endif()

find_package(SFML 2 REQUIRED system window graphics network audio)
if(SFML_FOUND)
  include_directories(${SFML_INCLUDE_DIR})
  target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})
endif()

if (TSPP_OS_WINDOWS)
  target_link_libraries(${EXECUTABLE_NAME} "winmm.lib")
endif()

target_link_libraries(${EXECUTABLE_NAME} squirrel)
target_link_libraries(${EXECUTABLE_NAME} sqstdlib)
target_link_libraries(${EXECUTABLE_NAME} zlib)
target_link_libraries(${EXECUTABLE_NAME} png12)
target_link_libraries(${EXECUTABLE_NAME} freetype)
