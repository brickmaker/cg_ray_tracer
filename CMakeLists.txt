cmake_minimum_required(VERSION 3.15)
project(cg_ray_tracer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

if(APPLE)
    set(CMAKE_C_COMPILER clang)
    set(CMAKE_CXX_COMPILER clang++)

    if(CMAKE_C_COMPILER_ID MATCHES "Clang\$")
        set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp")
        set(OpenMP_C_LIB_NAMES "libomp")
        set(OpenMP_omp_LIBRARY omp)
    endif()

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang\$")
        set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp")
        set(OpenMP_CXX_LIB_NAMES "libomp")
        set(OpenMP_omp_LIBRARY omp)
    endif()

endif()

find_package(OpenMP REQUIRED)

set(CMAKE_CXX_STANDARD 14)

add_executable(cg_ray_tracer libs/glm/gtx/intersect.hpp main.cpp defs.h Mesh.cpp Mesh.h Utils.cpp Utils.h Renderer.cpp Renderer.h Light.cpp Light.h Camera.cpp Camera.h KDTree.cpp KDTree.h Random.cpp Random.h NormalCoord.cpp NormalCoord.h)

target_link_libraries(cg_ray_tracer OpenMP::OpenMP_CXX)
