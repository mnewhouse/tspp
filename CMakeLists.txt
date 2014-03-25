cmake_minimum_required(VERSION 2.8.12)
project(tspp)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(TSPP_OS_WINDOWS 1)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
    
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(TSPP_OS_LINUX 1)
    add_definitions(-std=c++1y)
    add_definitions(-g)
    add_definitions(-Wall)
endif()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

set(EXECUTABLE_NAME "tspp")

set(SRC src/main.cpp

        src/audio/audio_store.cpp
        src/audio/audio_store.hpp
        src/audio/collision_sounds.cpp
        src/audio/collision_sounds.hpp
        src/audio/car_sounds.cpp
        src/audio/car_sounds.hpp

        src/core/config.hpp
        src/core/directive_reader.hpp
        src/core/game.hpp
        src/core/game.cpp        
        src/core/game_state.hpp
        src/core/handle.hpp
        src/core/rect.hpp
        src/core/rotation.hpp
        src/core/state_machine.hpp
        src/core/transform.hpp
        src/core/utility.hpp
        src/core/vector2.hpp
               
        src/controls/control.hpp
        src/controls/control_center.hpp
        src/controls/control_center.cpp
        src/controls/controllable.hpp
        src/controls/controllable.cpp
        src/controls/key_mapping.hpp
        src/controls/key_mapping.cpp

        src/game/camera.hpp
        src/game/camera.cpp
        src/game/cup.hpp
        src/game/cup.cpp
        src/game/drawable_entity.hpp
        src/game/drawable_entity.cpp
        src/game/particle_generator.hpp
        src/game/particle_generator.cpp
        src/game/player.hpp
        src/game/stage_data.hpp
        src/game/track_builder.hpp
        src/game/track_builder.cpp

        src/graphics/fps_counter.hpp
        src/graphics/fps_counter.cpp
        src/graphics/fps_display.hpp
        src/graphics/fps_display.cpp
        src/graphics/particle_drawer.hpp
        src/graphics/particle_drawer.cpp
        src/graphics/render_scene.hpp
        src/graphics/renderer.hpp
        src/graphics/star_field.hpp        
        src/graphics/star_field.cpp
        src/graphics/texture.hpp
        src/graphics/texture.cpp
        src/graphics/texture_loader.hpp
        src/graphics/texture_loader.cpp

        src/graphics/fonts/font.hpp
		src/graphics/fonts/font.cpp
        src/graphics/fonts/font_base.hpp
        src/graphics/fonts/font_loader.hpp
        src/graphics/fonts/font_loader.cpp
        src/graphics/fonts/monospace.hpp
        src/graphics/fonts/monospace.cpp
        src/graphics/fonts/sans.hpp
        src/graphics/fonts/sans.cpp

        src/resources/car_definition.hpp
        src/resources/car_definition.cpp
        src/resources/car_store.hpp
        src/resources/car_store.cpp
        src/resources/image_type.hpp
        src/resources/include_path.hpp
        src/resources/include_path.cpp
        src/resources/pattern.hpp
        src/resources/pattern.cpp
        src/resources/pattern_loader.hpp
        src/resources/pattern_loader.cpp
        src/resources/resource_store.hpp
        src/resources/settings.hpp
        src/resources/terrain_definition.hpp
        src/resources/terrain_definition.cpp
        src/resources/terrain_library.hpp
        src/resources/terrain_library.cpp
        src/resources/terrain_map.hpp
        src/resources/terrain_map.cpp
        src/resources/tile.hpp
        src/resources/tile_library.hpp
        src/resources/tile_library.cpp
        src/resources/track.hpp
        src/resources/track.cpp
        src/resources/track_handle.hpp
        src/resources/track_handle.cpp
        src/resources/track_store.hpp
        src/resources/track_store.cpp
        src/resources/wall_definition.hpp
        src/resources/wall_definition.cpp

        src/states/action_state.hpp
        src/states/action_state.cpp
        src/states/cup_state.hpp
        src/states/cup_state.cpp
        src/states/loading_Screen.hpp
        src/states/loading_screen.cpp
        src/states/local_cup.hpp
        src/states/local_cup.cpp
        src/states/main_menu.hpp
        src/states/main_menu.cpp
        
        src/user_interface/context.hpp
        src/user_interface/context.cpp
        src/user_interface/event_handlers.hpp
        src/user_interface/event_handlers.cpp
        src/user_interface/gui_state.hpp
        src/user_interface/gui_state.cpp
        src/user_interface/gui_scene.hpp
        src/user_interface/mouse_state.hpp
        
        src/user_interface/elements/button.hpp
        src/user_interface/elements/rectangle_region.hpp
        
        src/world/car.hpp
        src/world/car.cpp
        src/world/collision_bitmap.hpp
        src/world/collision_bitmap.cpp
        src/world/collision_helpers.hpp
        src/world/collisions.hpp
        src/world/collisions.cpp
        src/world/entity.hpp
        src/world/entity.cpp
        src/world/entity_listener.hpp
        src/world/handling.hpp
        src/world/handling.cpp
        src/world/world.hpp
        src/world/world.cpp
)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)
set(Boost_USE_MULTITHREADED ON)
set(STATIC_STD_LIBS ON)
set(SFML_STATIC_LIBRARIES ON)

include_directories("${CMAKE_SOURCE_DIR}/src")

add_executable(${EXECUTABLE_NAME} ${SRC})

find_package(Boost REQUIRED COMPONENTS system filesystem)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
    add_definitions("-DHAS_BOOST")
    target_link_libraries(${EXECUTABLE_NAME} ${Boost_LIBRARIES})
endif()

find_package(PNG REQUIRED)
if(PNG_FOUND)
  target_link_libraries(${EXECUTABLE_NAME} ${ZLIB_LIBRARY})
  target_link_libraries(${EXECUTABLE_NAME} ${PNG_LIBRARY})
  include_directories(${PNG_INCLUDE_DIR})
  
endif()

if(TSPP_OS_LINUX)
  find_package(X11 REQUIRED)
  if(X11_FOUND)
    target_link_libraries(${EXECUTABLE_NAME} ${X11_LIBRARIES})
    include_directories(${X11_INCLUDE_DIR})
  endif()
endif()

find_package(SFML 2 REQUIRED system window graphics network audio)
if(SFML_FOUND)
  include_directories(${SFML_INCLUDE_DIR})
  target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})
endif()

if (TSPP_OS_WINDOWS)
  target_link_libraries(${EXECUTABLE_NAME} "winmm.lib")
endif()
