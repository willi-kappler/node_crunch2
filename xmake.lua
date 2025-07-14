add_rules("mode.debug", "mode.release")

set_warnings("everything", "error", "extra", "pedantic")
add_cxxflags("-Wconversion")
set_languages("c11")
set_version("0.1.0")
-- set_optimize("fastest")

target("node_crunch2")
    set_kind("shared")
    add_files("src/*.cpp", "src/*.hpp")

target("test")
    set_kind("binary")
    add_files("test/*.cpp", "test/*.hpp")
    add_deps("node_crunch2")
