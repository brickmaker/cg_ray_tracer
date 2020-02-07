//
// Created by Xiaodong Zhao on 2020/2/6.
//

#define GLM_ENABLE_EXPERIMENTAL

#include "Renderer.h"

glm::vec3 Renderer::castSphere(Ray ray) {
    float_t x = ray.origin.x;
    float_t y = ray.origin.y;
    float_t r = 0.8;
    if (x * x + y * y < r) {
        std::cout << x << " " << y << std::endl;
        float_t z = glm::sqrt(r - (x * x + y * y));
        glm::vec3 normal(x, y, z);
        normal = glm::normalize(normal);
        // simulate light
        glm::vec3 light(0, 0, -1);
        return glm::vec3(1.) * glm::max(glm::dot(-light, normal), 0.f);
    }
    return glm::vec3(0);
}

glm::vec3 Renderer::cast(Ray ray) {
    IntersectInfo intersectInfo{};
    if (intersect(ray, intersectInfo)) {
        // simulate light
        glm::vec3 light(0, 0, -1);
        return glm::vec3(1.) * glm::max(glm::dot(-light, intersectInfo.normal), 0.f);
//        return glm::vec3(1.) * glm::abs(glm::dot(-light, intersectInfo.normal));
    }

    return glm::vec3(0.);
}

Ray Renderer::ortho_ray_transform(Ray ray, int width, int height) {
    Ray ret(ray);
    ret.origin.x = (ray.origin.x - (float_t) width / 2) * 2.f / height;
    ret.origin.y = (ray.origin.y - (float_t) height / 2) * 2.f / height;
    return ret;
}

buffer_t Renderer::render() {
    buffer_t buffer(height, buffer_row_t(width));
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Ray ray;
            ray.origin = glm::vec3(x, y, 1);
            ray.direction = glm::vec3(0, 0, -1);
            buffer[height - y - 1][x] = cast(ortho_ray_transform(ray, width, height));
        }
        fprintf(stderr, "\r%3d%c", uint32_t(y / (float) height * 100), '%');
    }
    return buffer;
}

Renderer::Renderer(Mesh &mesh, const int width, const int height) : mesh(mesh), width(width), height(height) {}

bool Renderer::intersect(Ray ray, IntersectInfo &intersectInfo) {

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
                intersectInfo.pos = ray.origin + distance * ray.direction;
                intersectInfo.normal = (1 - bary_pos.x - bary_pos.y) * normals[0] + bary_pos.x * normals[1] + bary_pos.y * normals[2];
//                intersectInfo.normal = normal;
            }
        }
    }

    return flag_intersect;
}
