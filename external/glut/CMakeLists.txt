add_library(glut INTERFACE)

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
target_link_libraries(glut INTERFACE ${GLUT_LIBRARIES})

target_include_directories(glut INTERFACE ${GLUT_INCLUDE_DIRS})