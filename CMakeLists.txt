# CMakeLists.txt --- CMake project settings
#    ex) cmake -G "Visual Studio 9 2008" .
#    ex) cmake -DCMAKE_BUILD_TYPE=Release -G "MSYS Makefiles" .
##############################################################################

# CMake minimum version
cmake_minimum_required(VERSION 2.4)

# project name and language
project(caper CXX)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # using Clang
    set(CMAKE_C_FLAGS "-static")
    set(CMAKE_CXX_FLAGS "-static")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # using GCC
    set(CMAKE_C_FLAGS "-static")
    set(CMAKE_CXX_FLAGS "-static")
elseif (MSVC)
    # replace "/MD" with "/MT" (building without runtime DLLs)
    set(CompilerFlags
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_RELWITHDEBINFO
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    foreach(CompilerFlags ${CompilerFlags})
        string(REPLACE "/MD" "/MT" ${CompilerFlags} "${${CompilerFlags}}")
    endforeach()
endif()

##############################################################################

# Boost
find_package(Boost COMPONENTS system filesystem REQUIRED)

# sub-directories
subdirs(caper capella)

# get TODAY
string(TIMESTAMP TODAY "%Y-%m-%d")

# publish
add_custom_command(
    OUTPUT tmp/caper-${TODAY}.zip
    DEPENDS caper
    COMMAND ${CMAKE_COMMAND} -E echo "Publishing..."
    COMMAND zip -r9 tmp/caper-${TODAY}.zip $<TARGET_FILE:caper>
    COMMAND ${CMAKE_COMMAND} -E echo "Done."
    WORKING_DIRECTORY .)
add_custom_target(publish DEPENDS tmp/caper-${TODAY}.zip)

##############################################################################
