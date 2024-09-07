cmake_minimum_required(VERSION 3.17)

project(3d)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME}
    glad.c 
    main.cpp 
    shader/shader.cpp shader/shader_loader.cpp
    model/mesh.cpp model/model.cpp 
    scene/object.cpp scene/scene.cpp scene/camera.cpp 
    inner/state.cpp inner/buffer.cpp inner/vao.cpp inner/event_handler.cpp
    imgui/imgui.cpp imgui/imgui_demo.cpp imgui/imgui_draw.cpp imgui/imgui_impl_glfw.cpp imgui/imgui_impl_opengl3.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp)

add_subdirectory(stb_image)

target_link_libraries(${PROJECT_NAME} PUBLIC Stb_Image GL dl glfw assimp)
target_include_directories(${PROJECT_NAME} PUBLIC 
    "${PROJECT_SOURCE_DIR}/scene" 
    "${PROJECT_SOURCE_DIR}/camera" 
    "${PROJECT_SOURCE_DIR}/shader"
    "${PROJECT_SOURCE_DIR}/inner"
    "${PROJECT_SOURCE_DIR}/model"
    "${PROJECT_SOURCE_DIR}/stb_image")
