cmake_minimum_required(VERSION 3.28)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15 CACHE STRING "Build for 10.14")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# TODO Remove This before we ship non beta version. Remember
# the AU rescans only when versions change even if binaries are
# different.
string(TIMESTAMP DAY_OF_YEAR "%j")
string(TIMESTAMP YEAR "%Y")
math(EXPR PART0 "${YEAR}-2025 + 5")
math(EXPR PART1 "${DAY_OF_YEAR} + 1")

project(soemdsp-sandbox-audioplugin VERSION 0.${PART0}.${PART1} LANGUAGES C CXX)
set(PRODUCT_NAME "soemdsp")


option(USE_SANITIZER "Build and link with ASAN" FALSE)
option(COPY_AFTER_BUILD "Will copy after build" TRUE)
option(BUILD_SINGLE_ONLY "Only build the one plugin - no seven sines out" FALSE)

include(cmake/compile-options.cmake)

## New version
include(libs/sst/sst-plugininfra/cmake/git-version-functions.cmake)
version_from_versionfile_or_git()
message(STATUS "DISPLAY_VERSION=${GIT_IMPLIED_DISPLAY_VERSION}; COMMIT_HASH=${GIT_COMMIT_HASH}; BRANCH=${GIT_BRANCH}")


include(cmake/CmakeRC.cmake)
file(GLOB_RECURSE PATCHES  "resources/factory_patches/*.soempatch")
cmrc_add_resource_library(${PROJECT_NAME}-patches NAMESPACE soemdsp_patches ${PATCHES})

set(SOEMDSP_PERFORM_DIR "${CMAKE_SOURCE_DIR}/resources/perform")
if(EXISTS "${CMAKE_SOURCE_DIR}/../soemdsp-sandbox/perform.html")
    set(SOEMDSP_PERFORM_DIR "${CMAKE_SOURCE_DIR}/../soemdsp-sandbox")
endif()
file(GLOB SOEMDSP_PERFORM_FILES "${SOEMDSP_PERFORM_DIR}/*")
cmrc_add_resource_library(${PROJECT_NAME}-perform NAMESPACE soemdsp_perform
        WHENCE ${SOEMDSP_PERFORM_DIR}
        ${SOEMDSP_PERFORM_FILES})

set(JUCE_PATH "${CMAKE_SOURCE_DIR}/libs/JUCE")
add_subdirectory(libs)
add_subdirectory(runtime)

add_library(${PROJECT_NAME}-impl STATIC
        src/clap/plugin-clap.cpp
        src/clap/plugin-clap-entry-impl.cpp

        src/ui/plugin-editor.cpp
        src/ui/perform-webview.cpp
        src/ui/main-panel.cpp

        src/presets/preset-manager.cpp

        src/engine/engine.cpp
        src/engine/voice.cpp
        src/engine/patch.cpp
        src/engine/sandbox_graph.cpp
        src/engine/sandbox_patch_json.cpp

)
target_include_directories(${PROJECT_NAME}-impl PUBLIC src)
# Important setup variables
target_compile_definitions(${PROJECT_NAME}-impl PRIVATE
        PRODUCT_NAME="${PRODUCT_NAME}"
        PATCH_EXTENSION=".soempatch"
)

if (${BUILD_SINGLE_ONLY})
    message(STATUS "Building single plugin version only")
    target_compile_definitions(${PROJECT_NAME}-impl PRIVATE
            CLAP_PLUGIN_COUNT=1
    )
endif()

if (WIN32)
    message(STATUS "Activating wchar presets")
    target_compile_definitions(${PROJECT_NAME}-impl PUBLIC USE_WCHAR_PRESET=1)
endif()

target_link_libraries(${PROJECT_NAME}-impl PUBLIC
        clap
)
target_link_libraries(${PROJECT_NAME}-impl PRIVATE
        simde
        clap-helpers clap-wrapper-extensions
        mts-esp-client
        fmt-header-only
        sst-basic-blocks sst-voicemanager sst-jucegui sst-cpputils
        sst-plugininfra
        sst-plugininfra::filesystem
        sst-plugininfra::tinyxml
        sst-plugininfra::strnatcmp
        sst-plugininfra::patchbase
        sst-plugininfra::version_information
        sst::clap_juce_shim sst::clap_juce_shim_headers
        ${PROJECT_NAME}-patches
        ${PROJECT_NAME}-perform
        soemdsp-graph-runtime
)

if(WIN32)
    file(GLOB SOEMDSP_WEBVIEW2_ROOTS "$ENV{USERPROFILE}/.nuget/packages/microsoft.web.webview2/*")
    list(SORT SOEMDSP_WEBVIEW2_ROOTS)
    list(REVERSE SOEMDSP_WEBVIEW2_ROOTS)
    if(SOEMDSP_WEBVIEW2_ROOTS)
        list(GET SOEMDSP_WEBVIEW2_ROOTS 0 SOEMDSP_WEBVIEW2_ROOT)
        set(SOEMDSP_WEBVIEW2_INCLUDE "${SOEMDSP_WEBVIEW2_ROOT}/build/native/include")
        set(SOEMDSP_WEBVIEW2_DLL "${SOEMDSP_WEBVIEW2_ROOT}/build/native/x64/WebView2Loader.dll")
        if(EXISTS "${SOEMDSP_WEBVIEW2_INCLUDE}/WebView2.h")
            if(TARGET juce_gui_extra)
                target_include_directories(juce_gui_extra INTERFACE "${SOEMDSP_WEBVIEW2_INCLUDE}")
            endif()
            if(TARGET clap_juce_shim)
                target_include_directories(clap_juce_shim PRIVATE "${SOEMDSP_WEBVIEW2_INCLUDE}")
            endif()
            if(TARGET sst-jucegui)
                target_include_directories(sst-jucegui PRIVATE "${SOEMDSP_WEBVIEW2_INCLUDE}")
            endif()
        endif()
    endif()
endif()

make_clapfirst_plugins(
        TARGET_NAME ${PROJECT_NAME}
        IMPL_TARGET ${PROJECT_NAME}-impl

        OUTPUT_NAME "${PRODUCT_NAME}"

        ENTRY_SOURCE src/clap/plugin-clap-entry.cpp

        BUNDLE_IDENTIFER "org.soundemote.soemdsp"
        BUNDLE_VERSION ${PROJECT_VERSION}

        COPY_AFTER_BUILD ${COPY_AFTER_BUILD}

        PLUGIN_FORMATS CLAP VST3 AUV2

        ASSET_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_assets

        # You can add a target-per-standalone you want. Syntax here is
        #   target-postfix output-name clap-id
        # This allows you to make multiple standalones from a multi-plugin clap
        STANDALONE_CONFIGURATIONS
            standalone "${PRODUCT_NAME}" "org.soundemote.soemdsp"

        # STANDALONE_MACOS_ICON "${CMAKE_SOURCE_DIR}/resources/mac_installer/Icon.icns"
)

set(SOEMDSP_SANDBOX_PUBLIC "${CMAKE_SOURCE_DIR}/../soemdsp-sandbox/public")
set(SOEMDSP_PERFORM_PACK "${CMAKE_BINARY_DIR}/soemdsp-perform-pack")
if(EXISTS "${SOEMDSP_SANDBOX_PUBLIC}/perform.html")
    add_custom_target(soemdsp-perform-pack ALL
            COMMAND ${CMAKE_COMMAND} -E make_directory "${SOEMDSP_PERFORM_PACK}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${SOEMDSP_SANDBOX_PUBLIC}/perform.html" "${SOEMDSP_PERFORM_PACK}/perform.html"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${SOEMDSP_SANDBOX_PUBLIC}/index.html" "${SOEMDSP_PERFORM_PACK}/index.html"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${SOEMDSP_SANDBOX_PUBLIC}" "${SOEMDSP_PERFORM_PACK}/public")
    foreach(SOEMDSP_HOST_TARGET ${PROJECT_NAME}_clap ${PROJECT_NAME}_vst3 ${PROJECT_NAME}_standalone)
        if(TARGET ${SOEMDSP_HOST_TARGET})
            add_dependencies(${SOEMDSP_HOST_TARGET} soemdsp-perform-pack)
            add_custom_command(TARGET ${SOEMDSP_HOST_TARGET} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${SOEMDSP_PERFORM_PACK}"
                    "$<TARGET_FILE_DIR:${SOEMDSP_HOST_TARGET}>/soemdsp-perform")
        endif()
    endforeach()
endif()

if(WIN32 AND SOEMDSP_WEBVIEW2_DLL AND EXISTS "${SOEMDSP_WEBVIEW2_DLL}")
    foreach(SOEMDSP_HOST_TARGET ${PROJECT_NAME}_clap ${PROJECT_NAME}_vst3 ${PROJECT_NAME}_standalone)
        if(TARGET ${SOEMDSP_HOST_TARGET})
            add_custom_command(TARGET ${SOEMDSP_HOST_TARGET} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${SOEMDSP_WEBVIEW2_DLL}"
                    "$<TARGET_FILE_DIR:${SOEMDSP_HOST_TARGET}>/WebView2Loader.dll")
        endif()
    endforeach()
endif()

add_subdirectory(tests)




