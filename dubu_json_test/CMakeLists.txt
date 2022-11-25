set(target_name "dubu_json_test")

set(src_files
    "src/type.cpp"
    "src/unit.cpp")

add_executable(${target_name} ${src_files})

target_link_libraries(${target_name}
    dubu_json
    gmock_main)

set_target_properties(${target_name} PROPERTIES FOLDER ${${PROJECT_NAME}_FOLDER}/test)

gtest_discover_tests(${target_name})
