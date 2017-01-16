
set(TESTS_SRCS
  test_example.cpp
)

set(TESTS_HDRS
  test_example.h
)

# required packages

find_package(Qt5Test)

# variables

set(PROJECT_NAME scc)
set(TESTS_NAME "${PROJECT_NAME}-tests")

# MOC files

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
qt5_generate_moc (${TESTS_NAME} TESTS_MOC_FILES ${TESTS_HDRS})

# generate

add_executable(${TESTS_NAME} ${TESTS_SRCS} ${TESTS_MOC_FILES})
target_link_libraries(${TESTS_NAME} Qt5::Test)

# tests

add_test(${TESTS_NAME} ${TESTS_NAME})
