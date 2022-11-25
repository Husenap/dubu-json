set(target_name "dubu_json")

set(src_dubu_json
    "src/dubu_json/parser/parser.hpp"
    "src/dubu_json/dubu_json.hpp")

set(src_files
    ${src_dubu_json})

# Project
add_library(${target_name} STATIC ${src_files})
set_target_properties(${target_name} PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(${target_name}
    compiler_features
    compiler_warnings)

target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

set_target_properties(${target_name} PROPERTIES FOLDER ${${PROJECT_NAME}_FOLDER})
