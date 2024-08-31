cmake_minimum_required(VERSION 3.22)

set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
option(BUILD_SKYRIMAE "Build for Skyrim AE" OFF)
option(BUILD_SKYRIMVR "Build for Skyrim VR" OFF)

# ---- Options ----
option(COPY_TO_PAPYRUS "Copy finished build to Papyrus SKSE Folder" ON)

# Create a path variable "MODS_FOLDER", change "PAPYRUS_ROOT" to the papyrus dev folder
# If option "COPY_TO_PAPYRUS", files will be copied to <Path>/PAPYRUS_ROOT/<version_folder>
set(PAPYRUS_ROOT "SL-Dev")
set(PAPYRUS_SE "1597")
set(PAPYRUS_AE "SKSE/Plugins")
set(PAPYRUS_VR "14vr")

# ---- Project ----
project(
    SexLabUtil
    VERSION 2.11.2.1
    LANGUAGES CXX
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Plugin.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/Plugin.h
    @ONLY
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/CMake/version.rc.in
    ${CMAKE_CURRENT_BINARY_DIR}/version.rc
    @ONLY
)

# ---- Globals ----
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Include guards ----
if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed.")
endif()

# ---- Cache build vars ----
macro(set_from_environment VARIABLE)
    if(NOT DEFINED ${VARIABLE} AND DEFINED ENV{${VARIABLE}})
        set(${VARIABLE} $ENV{${VARIABLE}})
    endif()
endmacro()

set_from_environment(VCPKG_ROOT)
set_from_environment(MODS_FOLDER)

if(DEFINED VCPKG_ROOT)
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    set(VCPKG_TARGET_TRIPLET "x64-windows-static" CACHE STRING "")
else()
    message(WARNING "Variable VCPKG_ROOT is not set. Continuing without vcpkg.")
endif()

# ---- Dependencies ----
if(BUILD_SKYRIMVR)
    add_compile_definitions(SKYRIMVR)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_subdirectory("./lib/CommonLibVR" CommonLibSSE)
    set(SkyrimPath ${MODS_FOLDER}/${PAPYRUS_ROOT}/${PAPYRUS_VR})
    set(CommonLibName "CommonLibVR")
    message("Building Skyrim VR")
else()
    if(BUILD_SKYRIMAE)
        add_compile_definitions(SKYRIM_AE)
        add_compile_definitions(SKYRIM_SUPPORT_AE)
        set(SkyrimPath ${MODS_FOLDER}/${PAPYRUS_ROOT}/${PAPYRUS_AE})
        message("Building Skyrim 1.6")
    else()
        set(SkyrimPath ${MODS_FOLDER}/${PAPYRUS_ROOT}/${PAPYRUS_SE})
        message("Building Skyrim 1.5")
    endif()

    set(CommonLibName "CommonLibSSE")
    add_subdirectory("./lib/CommonLibSSE" CommonLibSSE)
endif()

find_package(yaml-cpp REQUIRED)
find_package(spdlog REQUIRED CONFIG)
find_package(glm REQUIRED CONFIG)
find_package(Eigen3 REQUIRED NO_MODULE)
find_package(magic_enum REQUIRED)

# ---- Add source files ----
include(CMake/source-list.cmake)

source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES ${CXX_FILES}
)

source_group(
    TREE "${CMAKE_CURRENT_BINARY_DIR}"
    FILES "${CMAKE_CURRENT_BINARY_DIR}/include/Plugin.h"
)

# ---- Create DLL ----
add_library(
    "${PROJECT_NAME}"
    SHARED
    ${CXX_FILES}
    "${CMAKE_CURRENT_BINARY_DIR}/include/Plugin.h"
    "${CMAKE_CURRENT_BINARY_DIR}/version.rc"
    ".clang-format"
    ".editorconfig"
)

target_compile_features(
    "${PROJECT_NAME}"
    PRIVATE
    cxx_std_23
)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_compile_options(
        "${PROJECT_NAME}"
        PRIVATE
        "/sdl" # Enable Additional Security Checks
        "/utf-8" # Set Source and Executable character sets to UTF-8
        "/Zi" # Debug Information Format

        "/permissive-" # Standards conformance
        "/Zc:preprocessor" # Enable preprocessor conformance mode

        "/wd4201" # nonstandard extension used : nameless struct/union

        "$<$<CONFIG:DEBUG>:/bigobj>"
        "$<$<CONFIG:RELEASE>:/Zc:inline;/JMC-;/Ob3>"
    )

    target_link_options(
        "${PROJECT_NAME}"
        PRIVATE
        "$<$<CONFIG:DEBUG>:/INCREMENTAL;/OPT:NOREF;/OPT:NOICF>"
        "$<$<CONFIG:RELEASE>:/INCREMENTAL:NO;/OPT:REF;/OPT:ICF;/DEBUG:FULL>"
    )
endif()

target_include_directories(
    "${PROJECT_NAME}"
    PRIVATE
    "${CMAKE_CURRENT_BINARY_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(
    "${PROJECT_NAME}"
    PUBLIC
    ${CommonLibName}::${CommonLibName}
    spdlog::spdlog
    yaml-cpp
    magic_enum::magic_enum
    Eigen3::Eigen
)

target_precompile_headers("${PROJECT_NAME}"
    PUBLIC
    "src/PCH.h"
)

# ---- Post build ----
if(COPY_TO_PAPYRUS)
    if(DEFINED SkyrimPath)
        add_custom_command(
            TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${SkyrimPath}
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PDB_FILE:${PROJECT_NAME}> ${SkyrimPath}
        )
    else()
        message(
            WARNING
            "Variable ${SkyrimPath} is not defined. Skipping post-build copy command."
        )
    endif()
endif()
