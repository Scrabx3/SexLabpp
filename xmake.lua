set_xmakever("2.9.5")

-- Globals
PROJECT_NAME = "SexLabUtil"

-- Project
set_project(PROJECT_NAME)
set_version("2.15.5")
set_languages("cxx23")
set_license("apache-2.0")
set_warnings("allextra", "error")

-- Options
option("copy_to_papyrus")
    set_default(true)
    set_description("Copy finished build to Papyrus SKSE folder")
option_end()

option("skyrim_se")
    set_default(false)
    set_description("Enable support for Skyrim 1.5")
option_end()

option("skyrim_vr")
    set_default(false)
    set_description("Enable support for Skyrim VR")
    add_defines("SKYRIM_SUPPORT_VR=1")
option_end()

-- Dependencies & Includes
-- https://github.com/xmake-io/xmake-repo/tree/dev
add_requires("yaml-cpp", "magic_enum", "nlohmann_json", "simpleini", "glm", "eigen")

if get_config("skyrim_vr") then
    includes("lib/commonlibvr")
else
    includes("lib/commonlibsse")
    if get_config("skyrim_se") then
        set_config("skyrim_ae", false)
    else
        set_config("skyrim_ae", true)
    end
end

-- policies
set_policy("package.requires_lock", true)

-- rules
add_rules("mode.debug", "mode.release")

if is_mode("debug") then
    add_defines("DEBUG")
    set_optimize("none")
elseif is_mode("release") then
    add_defines("NDEBUG")
    set_optimize("fastest")
    set_symbols("debug")
end

-- Target
target(PROJECT_NAME)
    -- Dependencies
    add_packages("yaml-cpp", "magic_enum", "nlohmann_json", "simpleini", "glm", "eigen")

    -- CommonLibSSE
    add_deps("commonlibsse")
    add_rules("commonlibsse.plugin", {
        name = PROJECT_NAME,
        author = "Scrab",
        description = "Backend for skyrims adult animation framework 'SexLab'."
        })

    -- Source files
    set_pcxxheader("src/PCH.h")
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_includedirs("src")

    -- flags
    add_cxxflags(
        "cl::/cgthreads4",
        "cl::/diagnostics:caret",
        "cl::/external:W0",
        "cl::/fp:contract",
        "cl::/fp:except-",
        "cl::/guard:cf-",
        "cl::/Zc:enumTypes",
        "cl::/Zc:preprocessor",
        "cl::/Zc:templateScope",
        "cl::/utf-8"
        )
    -- flags (cl: warnings -> errors)
    add_cxxflags("cl::/we4715") -- `function` : not all control paths return a value
    -- flags (cl: disable warnings)
    add_cxxflags(
        "cl::/wd4068", -- unknown pragma 'clang'
        "cl::/wd4201", -- nonstandard extension used : nameless struct/union
        "cl::/wd4265" -- 'type': class has virtual functions, but its non-trivial destructor is not virtual; instances of this class may not be destructed correctly
        )
    -- Conditional flags
    if is_mode("debug") then
        add_cxxflags("cl::/bigobj")
    elseif is_mode("release") then
        add_cxxflags("cl::/Zc:inline", "cl::/JMC-", "cl::/Ob3")
    end
    -- Post Build 
    after_build(function (target)
        os.exec("python scripts/generate_config.py")

        local mod_folder = os.getenv("XSE_TES5_MODS_PATH")
        if not has_config("copy_to_papyrus") then
            print("Notice: copy_to_papyrus not defined. Skipping post-build copy.")
        elseif mod_folder then
            local plugin_path = path.join(mod_folder, "SL-Dev", "SKSE/Plugins")
            if not os.isdir(plugin_path) then
                os.mkdir(plugin_path)
            end
            os.cp(target:targetfile(), plugin_path)
            os.cp(target:symbolfile(), plugin_path)
            os.cp("scripts/out/*", plugin_path)
        else
            print("Warning: SkyrimPath not defined. Skipping post-build copy.")
        end
        print("Build finished. Skyrim V" .. (get_config("skyrim_vr") and "VR" or (get_config("skyrim_ae") and "1.6" or "1.5")))
    end)
target_end()
