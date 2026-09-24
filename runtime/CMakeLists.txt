# Native soemdsp graph engine + Gain.
# graph_engine.cpp hits MSVC C1061 (nested blocks); compile that file with clang-cl.
set(SOEMDSP_SANDBOX "${CMAKE_CURRENT_SOURCE_DIR}/../../soemdsp-sandbox" CACHE PATH
    "Path to soemdsp-sandbox (native_modules)")

if(NOT EXISTS "${SOEMDSP_SANDBOX}/native_modules/graph_engine/graph_engine.cpp")
    message(FATAL_ERROR "soemdsp-sandbox not found at ${SOEMDSP_SANDBOX}")
endif()

set(GRAPH_ENGINE_SRC "${SOEMDSP_SANDBOX}/native_modules/graph_engine/graph_engine.cpp")
set(GRAPH_ENGINE_OBJ "${CMAKE_CURRENT_BINARY_DIR}/graph_engine.obj")

find_program(SOEMDSP_CLANG_CL
    NAMES clang-cl
    PATHS
        "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/x64/bin"
        "C:/Program Files/LLVM/bin"
    NO_DEFAULT_PATH
)
if(NOT SOEMDSP_CLANG_CL)
    find_program(SOEMDSP_CLANG_CL NAMES clang-cl)
endif()
if(NOT SOEMDSP_CLANG_CL)
    message(FATAL_ERROR "clang-cl required to compile graph_engine.cpp (MSVC C1061)")
endif()

add_custom_command(
    OUTPUT "${GRAPH_ENGINE_OBJ}"
    COMMAND "${SOEMDSP_CLANG_CL}" /nologo /c /O2 /std:c++17 /EHsc /MD
        /DSOEMDSP_NATIVE_HOST=1
        "/I${SOEMDSP_SANDBOX}/native_modules"
        "/I${SOEMDSP_SANDBOX}/native_modules/sandbox_native_maths"
        "/Fo${GRAPH_ENGINE_OBJ}"
        "${GRAPH_ENGINE_SRC}"
    DEPENDS "${GRAPH_ENGINE_SRC}"
    COMMENT "clang-cl graph_engine.cpp (avoid MSVC C1061)"
    VERBATIM
)

add_library(soemdsp-graph-runtime STATIC
    "${GRAPH_ENGINE_OBJ}"
    "${SOEMDSP_SANDBOX}/native_modules/gain/gain.cpp"
    "${SOEMDSP_SANDBOX}/native_modules/transport/transport.cpp"
    "${SOEMDSP_SANDBOX}/native_modules/polyblep/polyblep.cpp"
    "${SOEMDSP_SANDBOX}/native_modules/gravity_walker/gravity_walker.cpp"
    "${SOEMDSP_SANDBOX}/native_modules/pluck_envelope_3/pluck_envelope_3.cpp"
    "${SOEMDSP_SANDBOX}/native_modules/passive_filter/passive_filter.cpp"
    "${SOEMDSP_SANDBOX}/native_modules/sample_hold/sample_hold.cpp"
    "${SOEMDSP_SANDBOX}/native_modules/vibrato_generator/vibrato_generator.cpp"
    "${SOEMDSP_SANDBOX}/native_modules/amp_curve/amp_curve.cpp"
    "${SOEMDSP_SANDBOX}/native_modules/pitch_quantizer/pitch_quantizer.cpp"
    "${SOEMDSP_SANDBOX}/native_modules/sabrina_reverb/sabrina_reverb.cpp"
    graph_engine_stubs.cpp
)
set_source_files_properties("${GRAPH_ENGINE_OBJ}" PROPERTIES
    EXTERNAL_OBJECT TRUE
    GENERATED TRUE
)
target_include_directories(soemdsp-graph-runtime PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/wasm_simd128_shim"
    "${SOEMDSP_SANDBOX}/native_modules"
    "${SOEMDSP_SANDBOX}/native_modules/sandbox_native_maths"
)
target_compile_features(soemdsp-graph-runtime PUBLIC cxx_std_17)
if(MSVC)
    target_compile_options(soemdsp-graph-runtime PRIVATE
        /FI"${CMAKE_CURRENT_SOURCE_DIR}/msvc_builtins.h"
        /wd4244 /wd4267 /wd4305)
endif()
