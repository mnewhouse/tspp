cmake_minimum_required(VERSION 2.8.12)
project(tspp)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

add_definitions(-std=c++11)

set(EXECUTABLE_NAME "tspp")

set(SRC src/main.cpp

        src/core/game.cpp

        src/controls/control_center.cpp
        src/controls/controllable.cpp
        src/controls/key_mapping.cpp

        src/cup/cup.cpp

        src/graphics/camera.cpp
        src/graphics/fps_counter.cpp
        src/graphics/texture_loader.cpp
        src/graphics/star_field.cpp
        src/graphics/drawable_entity.cpp
        src/graphics/fps_display.cpp
        src/graphics/track_builder.cpp
        src/graphics/texture.cpp

		src/graphics/fonts/font.cpp
        src/graphics/fonts/font_loader.cpp
        src/graphics/fonts/monospace.cpp
        src/graphics/fonts/sans.cpp

        src/resources/car_definition.cpp
        src/resources/car_store.cpp
        src/resources/pattern.cpp
        src/resources/pattern_loader.cpp
        src/resources/terrain_definition.cpp
        src/resources/terrain_library.cpp
        src/resources/terrain_map.cpp
        src/resources/tile_library.cpp
        src/resources/track.cpp
        src/resources/track_handle.cpp
        src/resources/track_store.cpp
        src/resources/wall_definition.cpp

        src/states/action_state.cpp
        src/states/cup_state.cpp
        src/states/loading_screen.cpp
        src/states/local_cup.cpp
        src/states/main_menu.cpp
        
        src/user_interface/context.cpp
        src/user_interface/event_handlers.cpp
        src/user_interface/gui_state.cpp
        
        src/world/car.cpp
        src/world/collisions.cpp
        src/world/entity.cpp
        src/world/handling.cpp
        src/world/world.cpp
)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_USE_MULTITHREADED ON)

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
  target_link_libraries(${EXECUTABLE_NAME} ${PNG_LIBRARY})
  include_directories(${PNG_INCLUDE_DIR})
endif()



find_package(SFML 2 REQUIRED system window graphics network audio)
if(SFML_FOUND)
  include_directories(${SFML_INCLUDE_DIR})
  target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})
endif()

