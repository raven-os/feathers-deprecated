list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

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
find_package(Wayland REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(DRM REQUIRED)
find_package(EGL REQUIRED)
find_package(UDEV REQUIRED)
find_package(OpenGLES3 REQUIRED)
find_package(claws CONFIG REQUIRED)
find_package(magma CONFIG REQUIRED)

include_directories(
  ${HEADER_DIRECTORY}
  ${WAYLAND_CLIENT_INCLUDE_DIR}
  ${WAYLAND_SERVER_INCLUDE_DIR}
  ${DRM_INCLUDE_DIR}
  ${GBM_INCLUDE_DIR}
  ${UDEV_INCLUDE_DIR}
  ${EGL_INCLUDE_DIR}
  ${OPENGLES3_INCLUDE_DIR}
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

target_link_libraries(${PROJECT_NAME}
  ${Vulkan_LIBRARY}
  ${WAYLAND_CLIENT_LIBRARIES}
  ${WAYLAND_SERVER_LIBRARIES}
  ${DRM_LIBRARY}
  ${GBM_LIBRARY}
  ${UDEV_LIBRARY}
  ${EGL_LIBRARY}
  ${OPENGLES3_LIBRARY}
  xkbcommon
  libinput
  magma::core
  )
