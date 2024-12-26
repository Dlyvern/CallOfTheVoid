cmake_minimum_required(VERSION 3.7)
project(CallOfTheVoid)

set(CMAKE_CXX_STANDARD 23)
set(INCLUDE_DIR include)
set(SRC_DIR src)
set(LIBRARIES_DIR libraries)

file(GLOB_RECURSE PROJECT_HEADERS  ${INCLUDE_DIR}/*.h ${INCLUDE_DIR}/*.hpp)
file(GLOB_RECURSE PROJECT_SOURCES ${SRC_DIR}/*.cpp ${SRC_DIR}/*.c)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.4 REQUIRED)
find_package(assimp 5.0 REQUIRED)
find_package(glm REQUIRED)
find_package(Freetype REQUIRED)
find_package(Threads REQUIRED)
# find_package(GLAD_LIBRARY glad REQUIRED)

add_executable(${PROJECT_NAME})

include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${GLFW_INCLUDE_DIR} ${GLM_INCLUDE_DIR} ${ASSIMP_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIRS})

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR} ${LIBRARIES_DIR} ${FREETYPE_LIBRARY_DIRS})

target_link_libraries(${PROJECT_NAME} PUBLIC OpenGL::OpenGL ${GLUT_LIBRARY} ${FREETYPE_LIBRARIES} glfw glm::glm assimp::assimp Threads::Threads)

target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES} PRIVATE ${PROJECT_HEADERS})