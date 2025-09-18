add_rules("mode.debug", "mode.release")

-- Set clang:
-- xmake f --toolchain=clang --runtimes=c++_shared
-- xmake f --toolchain=clang --runtimes=stdc++_shared
--  


set_warnings("error", "everything", "extra", "pedantic", "all")
add_cxxflags("-Wconversion", "-Wshadow", "-Wsign-conversion", "-Wdouble-promotion", "-Wformat=2")
add_cxxflags("-Wundef", "-Wcast-qual", "-Wcast-align=strict", "-Wnon-virtual-dtor", "-Wold-style-cast")
add_cxxflags("-Woverloaded-virtual", "-Wunused", "-Wuninitialized", "-Winit-self")
add_cxxflags("-Wredundant-decls", "-Wsuggest-override", "-Wimplicit-fallthrough=5", "-Walloca")
-- add_cxxflags("-Wnull-dereference", "-Wswitch-enum")

-- -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow
-- -Wdouble-promotion -Wformat=2 -Wundef -Wcast-qual -Wcast-align=strict
-- -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual
-- -Wnull-dereference -Wunused -Wuninitialized -Winit-self -Wswitch-enum -Wredundant-decls
-- -Wsuggest-override -Wimplicit-fallthrough=5 -Walloca -Werror



set_languages("c++23")
set_version("0.1.0")
-- set_optimize("fastest")

add_requires("taocpp-json")
add_requires("snitch")
add_requires("lz4")
add_requires("openssl3")
add_requires("asio")
add_requires("spdlog")

target("node_crunch2")
    set_kind("shared")
    add_files("src/*.cpp")
    add_packages("taocpp-json")
    add_packages("lz4")
    add_packages("openssl3")
    add_packages("asio")
    add_packages("spdlog")

target("nc_test")
    set_kind("binary")
    add_files("tests/test_all.cpp")
    add_packages("snitch")
    add_packages("spdlog")
    add_packages("asio")
    add_deps("node_crunch2")
    add_includedirs("src")
