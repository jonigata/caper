# caper.exe
add_executable(caper
    caper.cpp
    caper_cpg.cpp
    caper_tgt.cpp
    caper_generate_cpp.cpp
    caper_generate_d.cpp
    caper_generate_csharp.cpp
    caper_generate_js.cpp
    caper_generate_java.cpp
    caper_generate_boo.cpp
    caper_generate_ruby.cpp
    caper_generate_php.cpp
    caper_generate_haxe.cpp
    caper_stencil.cpp)
target_include_directories(caper PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(caper PRIVATE ${Boost_LIBRARIES})
