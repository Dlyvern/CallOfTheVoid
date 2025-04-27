cmake_minimum_required(VERSION 3.7)
project(CallOfTheVoid)

set(CMAKE_CXX_STANDARD 23)
set(INCLUDE_DIR include)
set(SRC_DIR src)
set(LIBRARIES_DIR libraries)
set(PHYSX_ROOT "~/Libraries/PhysX/physx")

file(GLOB_RECURSE PROJECT_HEADERS  ${INCLUDE_DIR}/*.h ${INCLUDE_DIR}/*.hpp)
file(GLOB_RECURSE PROJECT_SOURCES ${SRC_DIR}/*.cpp)

set(PHYSX_BUILD_TYPE "checked" CACHE STRING "The build type of PhysX")
set_property(CACHE PHYSX_BUILD_TYPE PROPERTY STRINGS debug checked profile release)

if(NOT CMAKE_BUILD_TYPE)
    if(PHYSX_BUILD_TYPE STREQUAL "debug" OR PHYSX_BUILD_TYPE STREQUAL "checked")
        set(CMAKE_BUILD_TYPE "Debug")
    endif()
endif()

link_directories("${PHYSX_ROOT}/bin/linux.x86_64/${PHYSX_BUILD_TYPE}")

link_directories(/usr/local/lib)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Building snippet in debug with PhysX ${PHYSX_BUILD_TYPE} configuration")
    add_compile_definitions(_DEBUG)
else()
    message("Building snippet in release configuration with PhysX ${PHYSX_BUILD_TYPE} configuration")
    add_compile_definitions(NDEBUG)
endif()

option(USE_VULKAN "Use Vulkan backend" OFF)

if(!USE_VULKAN)
    add_definitions(-DVULKAN_API)
    add_subdirectory(API/VulkanShit)
else()
    add_definitions(-DOPENGL_API)
    add_subdirectory(API/GLitch)
endif ()

find_package(OpenGL REQUIRED)
find_package(glfw3 3.4 REQUIRED)
find_package(assimp 5.0 REQUIRED)
find_package(glm REQUIRED)
find_package(Freetype REQUIRED)
find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME})

if(!USE_VULKAN)
    target_include_directories(${PROJECT_NAME} PUBLIC API/VulkanShit/include)
    target_sources(${PROJECT_NAME} PRIVATE API/VulkanShit/src/Shader.cpp)
else()
    target_include_directories(${PROJECT_NAME} PUBLIC API/GLitch/include)
    target_sources(${PROJECT_NAME} PRIVATE API/GLitch/src/Shader.cpp API/GLitch/src/Texture.cpp)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
        ${INCLUDE_DIR} ${LIBRARIES_DIR} ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS}
        ${GLFW_INCLUDE_DIR} ${GLM_INCLUDE_DIR} ${ASSIMP_INCLUDE_DIR}
        ${FREETYPE_INCLUDE_DIRS} ${PHYSX_ROOT}/include  libraries/imgui)

target_link_libraries(${PROJECT_NAME} PUBLIC OpenGL::OpenGL ${GLUT_LIBRARY} ${FREETYPE_LIBRARIES} glfw glm::glm assimp::assimp Threads::Threads
        -Wl,--start-group
        PhysXExtensions_static_64
        PhysX_static_64
        PhysXPvdSDK_static_64
        PhysXCommon_static_64
        PhysXFoundation_static_64
        PhysXCharacterKinematic_static_64
        -Wl,--end-group

#        PhysXGpu_64
#        PVDRuntime_64
#        PhysXVehicle2_static_64

        glad
#        PhysXCooking_static_64

        pthread
        dl
)

target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES} PRIVATE ${PROJECT_HEADERS}
        libraries/imgui/imgui.cpp
        libraries/imgui/imgui_draw.cpp
        libraries/imgui/imgui_widgets.cpp
        libraries/imgui/imgui_tables.cpp
        libraries/imgui/backends/imgui_impl_glfw.cpp
        libraries/imgui/backends/imgui_impl_opengl3.cpp)