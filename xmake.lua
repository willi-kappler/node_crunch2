add_rules("mode.debug", "mode.release")

set_warnings("error", "everything", "extra", "pedantic")
add_cxxflags("-Wconversion", "-Wshadow")
-- -Wdouble-promotion

set_languages("c++20")
set_version("0.1.0")
-- set_optimize("fastest")

add_requires("taocpp-json")
add_requires("snitch")


target("node_crunch2")
    set_kind("shared")
    add_files("src/*.cpp")
    add_packages("taocpp-json")
    -- add_headerfiles("src/*.hpp")
    -- add_includedirs("src")

target("nc_test")
    set_kind("binary")
    add_files("tests/*.cpp")
    add_packages("snitch")
    add_deps("node_crunch2")
    add_includedirs("src")
    -- add_headerfiles("test/*.hpp")
