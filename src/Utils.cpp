//
// Created by Xiaodong Zhao on 2020/2/6.
//

#include "Utils.h"
#include "../libs/glm/gtx/norm.hpp"

void Utils::save_img(buffer_t buffer) {
    const int height = buffer.size();
    const int width = buffer[0].size();
    // save framebuffer to file
    std::ofstream ofs;
    ofs.open("out.ppm");
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            char r = (char) (255 * glm::clamp(buffer[y][x].x, 0.f, 1.f));
            char g = (char) (255 * glm::clamp(buffer[y][x].y, 0.f, 1.f));
            char b = (char) (255 * glm::clamp(buffer[y][x].z, 0.f, 1.f));
            ofs << r << g << b;
        }
    }
    ofs.close();
}

glm::vec2 test_transform(glm::vec2 p, int width, int height) {
//    std::cout << p.x << " " << p.y << std::endl;
    glm::vec2 origin(width / 2, height / 2);
    return glm::vec2(p.x * width / 2., p.y * height / 2.) + origin;
//    return p * (float_t) (height / 2.) + origin;
}


/**
 * Show vertices of model for test
 * @param mesh
 * @param width
 * @param height
 * @return
 */
buffer_t Utils::show_vertices(Mesh &mesh, int width, int height) {
    buffer_t buffer(height, buffer_row_t(width));

    glm::mat4 view = glm::lookAt(glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 projection = glm::perspective(glm::radians(90.f), (float_t) width / height, 0.1f, 100.f);
//    glm::mat4 projection = glm::frustum(-1.f, 1.f, -1.f, 1.f, -100.f, 100.f);
//    glm::mat4 projection = glm::frustum(width * -1.f / 2, width * 1.f / 2, height * -1.f / 2, height * 1.f / 2, -100.f, 100.f);
    glm::mat4 vp_matrix = projection * view;

// Loop over shapes
    for (size_t s = 0; s < mesh.shapes.size(); s++) {
// Loop over faces(polygon)
        size_t index_offset = 0;
//        std::cout << mesh.shapes[s].mesh.num_face_vertices.size() << std::endl;
        for (size_t f = 0; f < mesh.shapes[s].mesh.num_face_vertices.size(); f++) {
            int fv = mesh.shapes[s].mesh.num_face_vertices[f];

// Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
// access to vertex
                tinyobj::index_t idx = mesh.shapes[s].mesh.indices[index_offset + v];
                float_t vx = mesh.attrib.vertices[3 * idx.vertex_index + 0];
                float_t vy = mesh.attrib.vertices[3 * idx.vertex_index + 1];
                float_t vz = mesh.attrib.vertices[3 * idx.vertex_index + 2];
//                glm::vec3 pos = test_transform(glm::vec3(vx, vy, vz), width, height);
                glm::vec4 vertex(vx, vy, vz, 1);
                glm::vec4 vv = vp_matrix * vertex;
                glm::vec2 pos = test_transform(glm::vec2(vv.x, vv.y), width, height);
//                std::cout << pos.x << " " << pos.y << std::endl;
                if (pos.x >= 0 && pos.x < width && pos.y > 0 && pos.y <= height) {
                    buffer[height - pos.y][pos.x] = glm::vec3(1);
                }
                float_t nx = mesh.attrib.normals[3 * idx.normal_index + 0];
                float_t ny = mesh.attrib.normals[3 * idx.normal_index + 1];
                float_t nz = mesh.attrib.normals[3 * idx.normal_index + 2];
                float_t tx = mesh.attrib.texcoords[2 * idx.texcoord_index + 0];
                float_t ty = mesh.attrib.texcoords[2 * idx.texcoord_index + 1];
            }
            index_offset += fv;
        }
    }
    return buffer;
}

glm::vec3 Utils::reflect_direction(glm::vec3 normal, glm::vec3 in_direction) {
    return in_direction - 2 * glm::dot(in_direction, normal) * normal;
}

glm::vec3 Utils::refract_direction(glm::vec3 normal, glm::vec3 in_direction, float_t n) {
    float_t cos1_val = glm::clamp(glm::dot(normal, in_direction), -1.f, 1.f);

    // out
    glm::vec3 new_normal = normal;
    float_t r = 1. / n; // ratio

    if (cos1_val < 0) {
        // in
        cos1_val = -cos1_val;
        new_normal = -normal;
    } else {
        // out
        r = n;
    }

    float_t cos2_val = 1 - (1 - cos1_val * cos1_val) * (r * r);

    // full reflection
    if (cos2_val < 0) {
        return glm::vec3(0);
    }

    return glm::normalize(in_direction * r - (cos1_val * r - glm::sqrt(cos2_val)) * new_normal);
}

glm::vec3 Utils::random_in_sphere() {
    glm::vec3 p;
    do {
        p = 2.0f * glm::vec3(Utils::rand(), Utils::rand(), Utils::rand()) - glm::vec3(1, 1, 1);
    } while (glm::length2(p) >= 1);
    return p;
}

float_t Utils::rand() {
    return (float_t) std::rand() / RAND_MAX;
}

glm::vec3 Utils::get_barycoord(glm::vec3 &p, glm::vec3 &a, glm::vec3 &b, glm::vec3 &c) {
    glm::vec3 v0 = c - a;
    glm::vec3 v1 = b - a;
    glm::vec3 v2 = p - a;
    float_t d00 = glm::dot(v0, v0);
    float_t d01 = glm::dot(v0, v1);
    float_t d02 = glm::dot(v0, v2);
    float_t d11 = glm::dot(v1, v1);
    float_t d12 = glm::dot(v1, v2);
    float_t denom = d00 * d11 - d01 * d01;

    if (denom == 0) {
        return glm::vec3(0);
    }

    float_t u = 1. / denom * (d11 * d02 - d01 * d12);
    float_t v = 1. / denom * (d00 * d12 - d01 * d02);

    return glm::vec3(1 - u - v, v, u);
}

float_t Utils::fresnel(glm::vec3 &normal, glm::vec3 &in_direction, float_t n) { // return reflection ratio
    float_t cos1_val = glm::clamp(glm::dot(normal, in_direction), -1.f, 1.f);
    float_t eta1 = 1., eta2 = n;
    if (cos1_val > 0) {
        std::swap(eta1, eta2);
    } else {
        cos1_val = -cos1_val;
    }

    float_t sin2_val = (eta1 / eta2) * glm::sqrt(glm::max(0.f, 1 - cos1_val * cos1_val));

    if (sin2_val >= 1) { // 全反射
        return 1.;
    }

    float_t cos2_val = glm::sqrt(glm::max(0.f, 1 - sin2_val * sin2_val));
    float Rs = ((eta2 * cos1_val) - (eta1 * cos2_val)) / ((eta2 * cos1_val) + (eta1 * cos2_val));
    float Rp = ((eta1 * cos1_val) - (eta2 * cos2_val)) / ((eta1 * cos1_val) + (eta2 * cos2_val));
    return (Rs * Rs + Rp * Rp) / 2;
}
