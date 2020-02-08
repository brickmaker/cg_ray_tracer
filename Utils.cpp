//
// Created by Xiaodong Zhao on 2020/2/6.
//

#include "Utils.h"

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
                if (pos.x >= 0 and pos.x < width and pos.y > 0 and pos.y <= height) {
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
