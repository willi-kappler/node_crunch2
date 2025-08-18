add_rules("mode.debug", "mode.release")

-- Set clang:
-- xmake f --toolchain=clang --runtimes=c++_shared
-- xmake f --toolchain=clang --runtimes=stdc++_shared
--  


set_warnings("error", "everything", "extra", "pedantic")
add_cxxflags("-Wconversion", "-Wshadow")
-- -Wdouble-promotion

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
    add_deps("node_crunch2")
    add_includedirs("src")
