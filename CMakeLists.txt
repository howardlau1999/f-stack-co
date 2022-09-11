cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(f-stack-co)
set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(dpdk REQUIRED)
find_package(OpenSSL REQUIRED)

add_compile_options(-Wall -Wextra -pedantic -Werror)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif ()

string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_LENGTH)
add_definitions("-DSOURCE_PATH_LENGTH=${SOURCE_PATH_LENGTH}")

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  option(F_STACK_CO_BUILD_EXAMPLES "Build examples" ON)
else()
  option(F_STACK_CO_BUILD_EXAMPLES "Build examples" OFF)
endif()

include_directories(include)

set(F_STACK_CO_SOURCE_FILES
  src/event_loop.cc
)

add_library(f-stack-co STATIC ${F_STACK_CO_SOURCE_FILES})
set(F_STACK_CO_LINK_LIBRARIES fstack dpdk::dpdk OpenSSL::SSL)
list(APPEND
  F_STACK_CO_COMPILE_OPTIONS
  PUBLIC
  -flto
  PRIVATE
  -fno-rtti
)
list(APPEND
  F_STACK_CO_LINK_OPTIONS
  PUBLIC
  -flto
  PRIVATE
  -fno-rtti
)

if (CMAKE_CXX_COMPILER_ID MATCHES Clang)
  list(APPEND
    F_STACK_CO_COMPILE_OPTIONS
    PUBLIC
    -stdlib=libc++
    PUBLIC
    -fcoroutines-ts
  )
  list(APPEND
    F_STACK_CO_LINK_OPTIONS
    PUBLIC
    -stdlib=libc++
  )
elseif (CMAKE_CXX_COMPILER_ID MATCHES GNU)
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "11") 
    list(APPEND
      F_STACK_CO_COMPILE_OPTIONS
      PUBLIC
      -fcoroutines
    )
  endif ()
endif ()
if (F_STACK_CO_COMPILE_OPTIONS)
  target_compile_options(f-stack-co ${F_STACK_CO_COMPILE_OPTIONS})
endif ()
if (F_STACK_CO_LINK_OPTIONS)
  target_link_options(f-stack-co ${F_STACK_CO_LINK_OPTIONS})
endif ()
target_link_libraries(f-stack-co ${F_STACK_CO_LINK_LIBRARIES})

set(F_STACK_CO_EXAMPLES helloworld)
if (F_STACK_CO_BUILD_EXAMPLES)
  foreach (EXAMPLE ${F_STACK_CO_EXAMPLES})
    add_executable(${EXAMPLE} examples/${EXAMPLE}.cc)
    target_link_libraries(${EXAMPLE} f-stack-co) 
  endforeach ()
endif ()

include(GNUInstallDirs)
install(TARGETS f-stack-co EXPORT uringpp ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/f-stack-co DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})