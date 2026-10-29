cmake_minimum_required(VERSION 3.16)
project(flock_sim C CXX)

#find_package(glfw3 REQUIRED)   # or add_subdirectory(external/glfw)
add_subdirectory(engine)

add_executable(app main.cpp)

target_link_libraries(app 
    PRIVATE 
    glm
    engine
    imgui
)

engine_copy_shaders(app)