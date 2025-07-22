add_rules("mode.debug", "mode.release")

set_warnings("error", "everything", "extra", "pedantic")
add_cxxflags("-Wconversion", "-Wshadow")
-- -Wdouble-promotion

set_languages("c++23")
set_version("0.1.0")
-- set_optimize("fastest")

add_requires("taocpp-json")
add_requires("snitch")
add_requires("lz4")


target("node_crunch2")
    set_kind("shared")
    add_files("src/*.cpp")
    add_packages("taocpp-json")
    add_packages("lz4")

target("nc_test")
    set_kind("binary")
    add_files("tests/test_all.cpp")
    add_packages("snitch")
    add_deps("node_crunch2")
    add_includedirs("src")
