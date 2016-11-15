project(whiteboard_gui)
cmake_minimum_required(VERSION 3.0)

find_package(Qt5Widgets)
find_package(Qt5Network)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall")

add_executable(whiteborad_gui

	main.cc
	resources.qrc

	main_window.cc main_window.hh main_window.ui
	scene.cc scene.hh
	debugger.cc debugger.hh
	source_view.cc source_view.hh source_view.ui

	process_runner.hh process_runner.cc

	mem_event.hh

	memory_block_item.cc memory_block_item.hh
	stack_block_item.cc stack_block_item.hh
	mem_cell_item.hh mem_cell_item.cc
)

target_link_libraries(whiteborad_gui

	Qt5::Widgets
	Qt5::Network
)

