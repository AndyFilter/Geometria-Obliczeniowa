cmake_minimum_required(VERSION 3.27)
project(lab01)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME}
        main.cpp
        ../External/ImGui/imgui.cpp
        ../External/ImGui/imgui_demo.cpp
        ../External/ImGui/imgui_draw.cpp
        ../External/ImGui/imgui_impl_glfw.cpp
        ../External/ImGui/imgui_impl_opengl3.cpp
        ../External/ImGui/imgui_tables.cpp
        ../External/ImGui/imgui_widgets.cpp
        ../External/ImGui/imgui_impl_glfw.cpp
        ../gui/gui.cpp
        ../gui/ImGuiExtensions.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ../)

message(${CMAKE_CURRENT_BINARY_DIR})
message(${CMAKE_SOURCE_DIR}/../Dependencies/GLFW/Include)

#find_package(glfw3 3.3 REQUIRED)
#target_link_libraries(${PROJECT_NAME} glfw3)


#find_package(glfw3 3.3 REQUIRED)
#target_link_libraries(${PROJECT_NAME} glfw3)
#
#find_package(OpenGL REQUIRED)
#target_link_libraries(${PROJECT_NAME} OpenGL::GL)
#
#target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/../Dependencies/GLFW/lib-vc2022)
#target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/../Dependencies/GLFW/Include)

target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/../Dependencies/GLFW/lib-vc2022)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/../Dependencies/GLFW/Include)

find_package(glfw3 3.3 REQUIRED)
target_link_libraries(${PROJECT_NAME} glfw) # Possibly glfw3 for Windows

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} OpenGL::GL)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/Dependencies/GLFW/Include)