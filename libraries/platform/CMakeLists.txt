add_library(platform STATIC)
generate_export_header(platform)
target_include_directories(platform
PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/include"
    "${CMAKE_CURRENT_BINARY_DIR}"
)
target_link_libraries(platform
PRIVATE
    core
)
target_sources(platform
PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/sources/${PLATFORM}/text/text_converter.cpp"
)
