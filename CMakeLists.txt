cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(fstackco)
set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(fstack REQUIRED)
find_package(dpdk REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(mlx5)

add_compile_options(-Wall -Wextra -pedantic -Werror)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif ()

string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_LENGTH)
add_definitions("-DSOURCE_PATH_LENGTH=${SOURCE_PATH_LENGTH}")

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  option(FSTACKCO_BUILD_EXAMPLES "Build examples" ON)
else()
  option(FSTACKCO_BUILD_EXAMPLES "Build examples" OFF)
endif()

include_directories(include)

set(FSTACKCO_SOURCE_FILES
  src/init.cc
  src/event_loop.cc
  src/socket.cc
  src/tcp_connection.cc
  src/tcp_listener.cc
)

add_library(fstackco STATIC ${FSTACKCO_SOURCE_FILES})
set(FSTACKCO_LINK_LIBRARIES OpenSSL::SSL dpdk::dpdk -Wl,--whole-archive fstack -Wl,--no-whole-archive)
if (mlx5_FOUND)
  find_package(ibverbs REQUIRED)
  list(APPEND FSTACKCO_LINK_LIBRARIES -Wl,--whole-archive dpdk::net_mlx5 -Wl,--no-whole-archive ibverbs mlx5)
endif ()
list(APPEND
  FSTACKCO_COMPILE_OPTIONS
  PUBLIC
  -flto
  PRIVATE
  -fno-rtti
)
list(APPEND
  FSTACKCO_LINK_OPTIONS
  PUBLIC
  -flto
  PRIVATE
  -fno-rtti
)

if (CMAKE_CXX_COMPILER_ID MATCHES Clang)
  list(APPEND
    FSTACKCO_COMPILE_OPTIONS
    PUBLIC
    -stdlib=libc++
    PUBLIC
    -fcoroutines-ts
  )
  list(APPEND
    FSTACKCO_LINK_OPTIONS
    PUBLIC
    -stdlib=libc++
  )
elseif (CMAKE_CXX_COMPILER_ID MATCHES GNU)
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "11") 
    list(APPEND
      FSTACKCO_COMPILE_OPTIONS
      PUBLIC
      -fcoroutines
    )
  endif ()
endif ()
if (FSTACKCO_COMPILE_OPTIONS)
  target_compile_options(fstackco ${FSTACKCO_COMPILE_OPTIONS})
endif ()
if (FSTACKCO_LINK_OPTIONS)
  target_link_options(fstackco ${FSTACKCO_LINK_OPTIONS})
endif ()
target_link_libraries(fstackco ${FSTACKCO_LINK_LIBRARIES})

set(FSTACKCO_EXAMPLES helloworld)
if (FSTACKCO_BUILD_EXAMPLES)
  foreach (EXAMPLE ${FSTACKCO_EXAMPLES})
    add_executable(${EXAMPLE} examples/${EXAMPLE}.cc)
    target_link_libraries(${EXAMPLE} fstackco) 
  endforeach ()
endif ()

include(GNUInstallDirs)
install(TARGETS fstackco EXPORT uringpp ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/fstackco DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
