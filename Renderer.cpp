//
// Created by Xiaodong Zhao on 2020/2/6.
//

#define GLM_ENABLE_EXPERIMENTAL

#include "Renderer.h"
#include "Utils.h"

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
    glm::vec3 res(0.);
    IntersectInfo intersect_info{};
    if (intersect(ray, intersect_info)) {
        auto material = mesh.materials[intersect_info.material_id];
        float_t Ni = material.ior;
        glm::vec3 refraction(0);
        if (glm::abs(Ni - 1.) > EPSILON) {
            bool is_in_dir = glm::dot(ray.direction, intersect_info.normal) < 0; // from out into object
            glm::vec3 out_dir = Utils::refract_direction(intersect_info.normal, ray.direction, Ni);
//                Ray refraction_ray(intersect_info.pos + EPSILON * intersect_info.normal * (is_in_dir ? -1. : 1.), out_dir);
            Ray refraction_ray(intersect_info.pos + EPSILON * out_dir, out_dir);
            refraction = cast(refraction_ray);
//            std::cout << refraction.x << std::endl;
            return refraction;
        }
        Ray shadow_ray(intersect_info.pos + EPSILON * -light.direction, -light.direction);
        IntersectInfo shadow_intersect_info{};
        if (!intersect(shadow_ray, shadow_intersect_info)) {
//            float_t Kd = 1.;
//            float_t Ns = 5;
//            float_t Ks = 0.5;
            auto material = mesh.materials[intersect_info.material_id];
            glm::vec3 Kd(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
            glm::vec3 Ks(material.specular[0], material.specular[1], material.specular[2]);
            float_t Ns = material.shininess;
            glm::vec3 diffuse = Kd * glm::vec3(1.) * glm::max(glm::dot(-light.direction, intersect_info.normal), 0.f);
            glm::vec3 specular = Ks * glm::vec3(1.) * glm::pow(glm::max(glm::dot(-light.direction, Utils::reflect_direction(intersect_info.normal, light.direction)), 0.f), Ns);
            res += diffuse + specular;
        }
//        return glm::vec3(1.) * glm::abs(glm::dot(-light, intersect_info.normal));
    }

    return res;
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
//            Ray ray;
//            ray.origin = glm::vec3(x, y, 1);
//            ray.direction = glm::vec3(0, 0, -1);
//            Ray cast_ray = ortho_ray_transform(ray, width, height);
//            buffer[height - y - 1][x] = cast(cast_ray);
            Ray cast_ray = camera.generate_ray(x, y);


            buffer[height - y - 1][x] = cast(cast_ray);
        }
        fprintf(stderr, "\r%3d%c", uint32_t(y / (float) height * 100), '%');
    }
    return buffer;
}

Renderer::Renderer(Mesh &mesh, Light &light, Camera &camera, const int width, const int height) : mesh(mesh), light(light), camera(camera), width(width), height(height) {}

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
                intersectInfo.material_id = mesh.shapes[shape_i].mesh.material_ids[face_i];
                intersectInfo.pos = ray.origin + distance * ray.direction;
                intersectInfo.normal = (1 - bary_pos.x - bary_pos.y) * normals[0] + bary_pos.x * normals[1] + bary_pos.y * normals[2];
//                intersectInfo.normal = normal;
            }
        }
    }

    return flag_intersect;
}
