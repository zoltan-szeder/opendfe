cmake_minimum_required(VERSION 3.10)

project(OpenDFE)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)


# Create Shared Library

set(PROJECT_INCLUDE_DIR include)
file(GLOB_RECURSE PROJECT_HEADERS ${PROJECT_INCLUDE_DIR}/*.h)

set(PROJECT_LIB_DIR src/odf)
file(GLOB_RECURSE PROJECT_LIB_C_SOURCES ${PROJECT_LIB_DIR}/*.c)
file(GLOB_RECURSE PROJECT_LIB_CXX_SOURCES ${PROJECT_LIB_DIR}/*.cpp)

add_library(OpenDFELib SHARED ${PROJECT_LIB_C_SOURCES} ${PROJECT_LIB_CXX_SOURCES} ${PROJECT_HEADERS})
target_include_directories(OpenDFELib PRIVATE ${PROJECT_INCLUDE_DIR})

set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
# set(GL_LIBRARY GL GLU X11)
target_link_libraries(OpenDFELib PUBLIC glfw m GLEW)


# Test
enable_testing()

set(PROJECT_TEST_DIR tests)
file(GLOB_RECURSE PROJECT_TESTS ${PROJECT_TEST_DIR}/*.c)

set(PROJECT_TEST_INCLUDE_DIR tests/include)
file(GLOB_RECURSE PROJECT_TEST_HEADERS ${PROJECT_TEST_INCLUDE_DIR}/*.h)

find_package(cmocka REQUIRED)

set(TEST_TARGETS "")
foreach(TEST_SRC ${PROJECT_TESTS})
    get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SRC})
    target_include_directories(${TEST_NAME} PRIVATE ${PROJECT_INCLUDE_DIR} ${PROJECT_TEST_INCLUDE_DIR})
    target_link_libraries(${TEST_NAME} OpenDFELib cmocka)
    list(APPEND TEST_TARGETS ${TEST_NAME})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    configure_file(tests/resources/test.bm ${CMAKE_CURRENT_BINARY_DIR}/tests/resources/test.bm COPYONLY)
    configure_file(tests/resources/test.gob ${CMAKE_CURRENT_BINARY_DIR}/tests/resources/test.gob COPYONLY)
    configure_file(tests/resources/test.pal ${CMAKE_CURRENT_BINARY_DIR}/tests/resources/test.pal COPYONLY)
endforeach()

add_custom_target(check DEPENDS ${TEST_TARGETS} COMMAND ctest --output-on-failure)

# Build executable project
set(PROJECT_SRC_DIR src)
file(GLOB_RECURSE PROJECT_MAIN ${PROJECT_SRC_DIR}/dark.c)

add_executable(OpenDFE ${PROJECT_MAIN} ${PROJECT_HEADERS})
target_include_directories(OpenDFE PRIVATE ${PROJECT_INCLUDE_DIR})
target_link_libraries(OpenDFE PRIVATE OpenDFELib)


