add_requires("entt", "raylib")
add_requires("raylib")

set_project("WoRm2")
set_languages("c++20")

includes("../../Github/EngineSquared/xmake.lua")

target("WoRm2")
    set_kind("binary")

    add_files("src/**.cpp")

    add_deps("EngineSquared")

    add_includedirs("src/")

    add_packages("entt", "raylib")


if is_mode("debug") then
    add_defines("DEBUG")
    set_symbols("debug")
    set_optimize("none")
end

if is_mode("release") then
    set_optimize("fastest")
end