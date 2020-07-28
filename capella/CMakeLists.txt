# capella.exe
add_executable(capella
    capella.cpp
    capella_dependency.cpp
    capella_generate_boost.cpp
    capella_generate_cpp.cpp
    capella_generate_dot.cpp
    capella_generate_stub.cpp
    capella_generate_cs.cpp)
add_custom_command(
    OUTPUT capella.hpp
    DEPENDS caper
    COMMAND ${CMAKE_COMMAND} -E echo "Generating capella.hpp..."
    COMMAND $<TARGET_FILE:caper> capella.cpg capella.hpp
    COMMAND ${CMAKE_COMMAND} -E echo "Generated."
    WORKING_DIRECTORY .)
add_custom_target(capella_header DEPENDS capella.hpp)
add_dependencies(capella capella_header)
