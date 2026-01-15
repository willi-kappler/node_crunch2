-- project("node_crunch2")

add_rules("mode.debug", "mode.release")

-- Clean configuration:
-- xmake f -c

-- Set configuration:
-- xmake f -m release
-- xmake f -m debug

-- Set clang:
-- xmake f --toolchain=clang --runtimes=c++_shared
-- xmake f --toolchain=clang --runtimes=stdc++_shared

set_warnings("error", "everything", "extra", "pedantic", "all")
add_cxxflags("-Wconversion", "-Wshadow", "-Wsign-conversion", "-Wdouble-promotion", "-Wformat=2")
add_cxxflags("-Wundef", "-Wcast-qual", "-Wcast-align=strict", "-Wnon-virtual-dtor", "-Wold-style-cast")
add_cxxflags("-Woverloaded-virtual", "-Wunused", "-Wuninitialized", "-Winit-self")
add_cxxflags("-Wredundant-decls", "-Wsuggest-override", "-Wimplicit-fallthrough=5", "-Walloca")
add_cxxflags("-O3")
-- add_cxxflags("-Wnull-dereference", "-Wswitch-enum")


set_languages("c++23")
set_version("0.1.0")

-- set_defaultmode("debug")
set_optimize("fastest")
set_defaultmode("release")


add_requires("taocpp-json")
add_requires("snitch")
add_requires("lz4")
add_requires("openssl3")
add_requires("asio")
add_requires("spdlog")
add_requires("argparse")

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

target("mandel_nc")
    set_kind("binary")
    add_files("example/mandel/src/*.cpp")
    add_packages("argparse")
    add_packages("spdlog")
    add_packages("asio")
    add_deps("node_crunch2")
    add_includedirs("src")
