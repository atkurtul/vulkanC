cmake_minimum_required(VERSION 2.8)

project(VulkanGcc C)

set(CMAKE_C_FLAGS "-O3 -m64 -march=native")

set(CMAKE_C_COMPILER_TARGET ${arch})

set(project_dir ${CMAKE_SOURCE_DIR})

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${project_dir})

set(PROJECT_INCLUDE_DIR ${project_dir}/include)

set(PROJECT_SOURCE_DIR ${project_dir}/src)

file(GLOB PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/*.c)

include_directories(${PROJECT_INCLUDE_DIR})

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

find_package(Vulkan REQUIRED)

find_package(glfw3 REQUIRED)

target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES})

target_link_libraries(${PROJECT_NAME} glfw)
