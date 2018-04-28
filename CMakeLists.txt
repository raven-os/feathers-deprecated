#
cmake_minimum_required(VERSION 2.6)
project(feathers CXX)

# main sources and headers
set(HEADER_DIRECTORY "include")
set(SOURCE_DIRECTORY "source")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -W -fdiagnostics-color=always -Wall -Wcast-align -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wdisabled-optimization -Wduplicated-cond -Wextra -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wno-sign-conversion -Wnoexcept -Wnull-dereference -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wstrict-aliasing=1 -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wno-unknown-pragmas -Wuseless-cast -Wno-parentheses -Wno-unused-local-typedefs")
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -stdlib=libc++ -Wno-missing-braces -std=c++17")
endif()
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG_LAYERS")

file(GLOB_RECURSE SOURCES_FILE "${SOURCE_DIRECTORY}/*.cpp")
file(GLOB_RECURSE HEADERS_FILE "${HEADER_DIRECTORY}/*.hpp")

find_package(Vulkan REQUIRED)
find_package(PkgConfig REQUIRED)

include_directories(
  ${GLFW_INCLUDE_DIRS}
  ${HEADER_DIRECTORY}
  )

if (DEFINED CLAWS_DIR)
  include_directories(${CLAWS_DIR}/include/)
endif()
if (DEFINED MAGMA_DIR)
  include_directories(${MAGMA_DIR}/include/)
endif()


add_executable(
  ${PROJECT_NAME}
  ${SOURCES_FILE}
  ${HEADERS_FILE}
)

target_link_libraries(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARY})
