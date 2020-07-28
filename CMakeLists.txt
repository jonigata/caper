# CMakeLists.txt --- CMake project settings
#    ex) cmake -G "Visual Studio 14 2015" .
#    ex) cmake -DCMAKE_BUILD_TYPE=Release -G "MSYS Makefiles" .
##############################################################################

# CMake minimum version
cmake_minimum_required(VERSION 2.4)

# project name and languages
project(caper CXX)

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
