cmake_minimum_required(VERSION 3.12)
project(cpp_app)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/exe)

# set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG "-O0 -ffast-math -g")
set(CMAKE_C_FLAGS_RELEASE "-O3 -ffast-math -DNDEBUG -s")
set(CMAKE_C_FLAGS_MINSIZEREL "-Os -ffast-math -DNDEBUG -s")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O3 -ffast-math -DNDEBUG -g")



file(GLOB sourcefiles "src/*.h" "src/*.cpp")
add_executable(${PROJECT_NAME} ${sourcefiles})

find_package(OpenGL)
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} SDL2_image SDL2_ttf SDL2 SDL2main SDL2_gfx)
