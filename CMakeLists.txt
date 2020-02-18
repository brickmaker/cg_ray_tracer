cmake_minimum_required(VERSION 3.15)
project(cg_ray_tracer)

set(CMAKE_CXX_STANDARD 14)

add_executable(cg_ray_tracer libs/glm/gtx/intersect.hpp main.cpp defs.h Mesh.cpp Mesh.h Utils.cpp Utils.h Renderer.cpp Renderer.h Light.cpp Light.h Camera.cpp Camera.h KDTree.cpp KDTree.h Random.cpp Random.h NormalCoord.cpp NormalCoord.h)
