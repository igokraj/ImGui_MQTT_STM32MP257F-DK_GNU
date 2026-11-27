cmake_minimum_required(VERSION 3.16)
project(STM32MP257F_GUI CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 REQUIRED)

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)
set(IMGUI_BACKENDS ${CMAKE_SOURCE_DIR}/external/backends)
set(IMPLOT_DIR ${CMAKE_SOURCE_DIR}/external/ImPlot)


add_executable(app
    src/main.cpp
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_stdlib.cpp
    ${IMGUI_BACKENDS}/imgui_impl_glfw.cpp
    ${IMGUI_BACKENDS}/imgui_impl_opengl3.cpp
    ${IMPLOT_DIR}/implot.cpp
    ${IMPLOT_DIR}/implot_items.cpp
    src/calculator.cpp
    src/clock.cpp
    src/mosquitto_client.cpp
)
target_include_directories(app PRIVATE 

${CMAKE_SOURCE_DIR}/include
${IMGUI_DIR} 
${IMGUI_BACKENDS}
${IMPLOT_DIR})

target_link_libraries(app PRIVATE mosquitto)

if(CMAKE_CROSSCOMPILING)

    # STM32MP257F: OpenGL ES 3
    target_compile_definitions(app PRIVATE IMGUI_IMPL_OPENGL_ES3 GLFW_INCLUDE_ES3)
    target_link_libraries(app PRIVATE glfw GLESv2 EGL)

else()

    # PC: OpenGL
    find_package(OpenGL REQUIRED)
    target_link_libraries(app PRIVATE glfw OpenGL::GL)

endif()