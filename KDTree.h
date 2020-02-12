//
// Created by Xiaodong Zhao on 2020/2/12.
//

#ifndef CG_RAY_TRACER_KDTREE_H
#define CG_RAY_TRACER_KDTREE_H

#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include "defs.h"
#include "Mesh.h"
#include "libs/glm/ext.hpp"

struct KDTree {
    Mesh &mesh;

    KDTree(Mesh &mesh) : mesh(mesh) {}

    bool intersect(Ray &ray, IntersectInfo &intersectInfo) {
        return intersect_naive(ray, intersectInfo);
    }

    bool intersect_naive(Ray &ray, IntersectInfo &intersectInfo) {

        bool flag_intersect = false;
        float_t near_dist = FLOAT_INF; // record nearest distance


        for (size_t shape_i = 0; shape_i < mesh.shapes.size(); shape_i++) {
            size_t offset = 0;
            for (size_t face_i = 0; face_i < mesh.shapes[shape_i].mesh.num_face_vertices.size(); face_i++) {
                // TODO: only support triangle
                size_t face_vertex_num = 3;
//                size_t face_vertex_num = mesh.shapes[shape_i].mesh.num_face_vertices[face_i];
                std::vector<glm::vec3> vertices(face_vertex_num);
                std::vector<glm::vec3> normals(face_vertex_num);
                for (size_t v_i = 0; v_i < face_vertex_num; v_i++) {
                    tinyobj::index_t indices = mesh.shapes[shape_i].mesh.indices[v_i + offset];
                    float_t vx = mesh.attrib.vertices[3 * indices.vertex_index + 0];
                    float_t vy = mesh.attrib.vertices[3 * indices.vertex_index + 1];
                    float_t vz = mesh.attrib.vertices[3 * indices.vertex_index + 2];
                    vertices[v_i] = glm::vec3(vx, vy, vz);

                    float_t nx = mesh.attrib.normals[3 * indices.normal_index + 0];
                    float_t ny = mesh.attrib.normals[3 * indices.normal_index + 1];
                    float_t nz = mesh.attrib.normals[3 * indices.normal_index + 2];
                    normals[v_i] = glm::vec3(nx, ny, nz);
                }
                offset += face_vertex_num;

                glm::vec2 bary_pos;
                float_t distance;
                bool is_intersect = glm::intersectRayTriangle(
                        ray.origin,
                        ray.direction,
                        vertices[0],
                        vertices[1],
                        vertices[2],
                        bary_pos,
                        distance
                );

//            glm::vec3 normal = glm::cross(vertices[1] - vertices[0], vertices[2] - vertices[0]);
//            normal = glm::normalize(normal);

                if (is_intersect && distance > 0 && distance < near_dist) {
                    near_dist = distance;
                    flag_intersect = true;
                    intersectInfo.shape_i = shape_i;
                    intersectInfo.face_i = face_i;
                    intersectInfo.material_id = mesh.shapes[shape_i].mesh.material_ids[face_i];
                    intersectInfo.pos = ray.origin + distance * ray.direction;
                    intersectInfo.normal = (1 - bary_pos.x - bary_pos.y) * normals[0] + bary_pos.x * normals[1] + bary_pos.y * normals[2];
//                intersectInfo.normal = normal;
                }
            }
        }

        return flag_intersect;
    }
};


#endif //CG_RAY_TRACER_KDTREE_H
