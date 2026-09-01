# Modern AzerothCore module loader: source files are discovered automatically.
# Keep UTF-8 explicitly enabled for MSVC because this module contains multilingual text.
if (MSVC)
    set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/src/mod-mall-teleport.cpp" PROPERTIES COMPILE_OPTIONS "/utf-8")
endif()

message("-- Mall Teleport CONFIGURED --")
