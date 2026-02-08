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

-- Force rebuild:
-- rm -rf build/ ~/.xmake/
-- xmake repo --update
-- xmake f -c
-- xmake clean
-- xmake -a -r

-- For all compilers:
-- set_warnings("error", "everything", "extra", "pedantic", "all")

-- For GCC and Clang only:
if is_kind("gcc", "clang") then
    -- For GCC only:
    if is_kind("gcc") then
        set_warnings("everything", "all", "extra", "pedantic", "error")
        add_cxxflags("-Walloca", "-Wcast-align=strict", "-Wimplicit-fallthrough=5")
    elseif is_kind("clang") then
        set_warnings("all", "extra", "pedantic", "error")
        -- Clang prefers the boolean flag without the numeric level
        add_cxxflags("-Wimplicit-fallthrough")
    end
--    add_cxxflags("-Wnull-dereference", "-Wswitch-enum")
    add_cxxflags("-Wconversion", "-Wshadow", "-Wsign-conversion", "-Wdouble-promotion", "-Wformat=2")
    add_cxxflags("-Wundef", "-Wcast-qual", "-Wnon-virtual-dtor", "-Wold-style-cast")
    add_cxxflags("-Woverloaded-virtual", "-Wunused", "-Wuninitialized", "-Winit-self")
    add_cxxflags("-Wredundant-decls", "-Wsuggest-override")
end

-- For MSVC only:
if is_kind("cl") then
    set_warnings("everything", "all", "extra", "pedantic", "error")
    -- add_cxxflags("/Wall")
    add_cxxflags("/W4") -- Level 4 is the standard "Strict" for MSVC
    add_cxxflags("/w14242") -- 'identifier': conversion from 'type1' to 'type1', possible loss of data
    add_cxxflags("/w14265") -- 'class': class has virtual functions, but destructor is not virtual
    add_cxxflags("/w14287") -- 'operator': unsigned/negative constant mismatch
    add_cxxflags("/we4289") -- nonstandard extension used: 'variable': control variable declared in the for-loop is used outside the for-loop scope
    add_cxxflags("/w14296") -- 'operator': expression is always false
    add_cxxflags("/w14311") -- 'variable' : pointer truncation from 'type' to 'type'
    add_cxxflags("/wd4068") -- disable "unknown pragma" (useful if you use GCC pragmas)
end

set_languages("c++23")
set_version("0.1.0")

-- set_defaultmode("debug")
set_optimize("fastest")
set_defaultmode("release")


add_requires("taocpp-json")
add_requires("snitch")
add_requires("lz4", {system = false})
add_requires("openssl3")
add_requires("asio")
add_requires("spdlog", {configs = {header_only = false}})
add_requires("argparse")

target("node_crunch2")
    set_kind("shared")
    add_files("src/*.cpp")
    add_packages("taocpp-json")
    add_packages("lz4")
    add_packages("openssl3")
    add_packages("asio")
    add_packages("spdlog")
    -- For spdlog, so that every object file sees the global logger:
    add_defines("SPDLOG_COMPILED_LIB", {public = true})
    -- Tell xmake which headers to give to the user when installing it:
    add_headerfiles("src/*.hpp")

target("nc_test")
    set_kind("binary")
    add_files("tests/test_all.cpp")
    add_packages("snitch")
    add_packages("spdlog")
    add_packages("asio")
    add_deps("node_crunch2")
    add_includedirs("src")
    set_default(false) -- Don't build by default

target("mandel_nc")
    set_kind("binary")
    add_files("example/mandel/src/*.cpp")
    add_packages("argparse")
    add_packages("spdlog")
    add_packages("asio")
    add_deps("node_crunch2")
    add_includedirs("src")
    set_default(false) -- Don't build by default
