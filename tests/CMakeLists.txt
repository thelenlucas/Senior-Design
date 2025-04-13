# All test files in this directory
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS *.cpp)

# Re‑use the production glob that was defined in the parent CMakeLists
# (it’s called “SOURCES” there).  We only need to remove main.cpp.
set(PROD_SOURCES ${SOURCES})               # copy the list
list(FILTER PROD_SOURCES EXCLUDE REGEX ".*/main\\.cpp$")

add_executable(model_tests
    ${TEST_SOURCES}
    ${PROD_SOURCES}        # everything except main()
)

set_property(TARGET model_tests PROPERTY AUTOMOC ON)
set_property(TARGET model_tests PROPERTY AUTOUIC ON)
set_property(TARGET model_tests PROPERTY AUTORCC ON)

set_target_properties(model_tests PROPERTIES
    AUTOUIC_SEARCH_PATHS "${CMAKE_SOURCE_DIR}/ui"
    AUTOMOC_SEARCH_PATHS "${CMAKE_SOURCE_DIR}/include;${CMAKE_SOURCE_DIR}/src"
)

file(GLOB UI_FILES "${CMAKE_SOURCE_DIR}/ui/*.ui")
target_sources(model_tests PRIVATE ${UI_FILES})

target_include_directories(model_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(model_tests
    PRIVATE
        gtest_main
        Qt5::Widgets
        Qt5::Sql
        SQLiteCpp
)

include(GoogleTest)                     # ships with CMake ≥3.10
gtest_discover_tests(model_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}   # where the test will run
    DISCOVERY_TIMEOUT 30                    # seconds (optional)
)