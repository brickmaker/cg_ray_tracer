//
// Created by Xiaodong Zhao on 2020/2/6.
//


#include "Renderer.h"
#include "Utils.h"

glm::vec3 Renderer::castSphere(Ray ray) {
    float_t x = ray.origin.x;
    float_t y = ray.origin.y;
    float_t r = 0.8;
    if (x * x + y * y < r) {
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
    if (tree.intersect(ray, intersect_info)) {

        auto material = mesh.materials[intersect_info.material_id];
        float_t Ni = material.ior;
        glm::vec3 Ka(material.ambient[0], material.ambient[1], material.ambient[2]);
        glm::vec3 refraction(0);

        if (Ka.x > EPSILON) {
            // is light
            res = Ka;
        } else if (glm::abs(Ni - 1.) > EPSILON) {
            // transparent
            bool is_in_dir = glm::dot(ray.direction, intersect_info.normal) < 0; // from out into object
            glm::vec3 out_dir = Utils::refract_direction(intersect_info.normal, ray.direction, Ni);
            Ray refraction_ray(intersect_info.pos + EPSILON * out_dir, out_dir);
            res = cast(refraction_ray);
        } else {
            // diffuse&specular
            for (PointLight &light : lights) {
                glm::vec3 light_dir = glm::normalize(intersect_info.pos - light.pos);
                Ray shadow_ray(intersect_info.pos + EPSILON * -light_dir, -light_dir);
                IntersectInfo shadow_intersect_info{};
                bool shadow_intersect = tree.intersect(shadow_ray, shadow_intersect_info);
                if (mesh.materials[shadow_intersect_info.material_id].name == light.material_name) {
                    // simulate spherical light
                    float_t final_intensity = (light.intensity * light.r * light.r) / (glm::length2(intersect_info.pos - light.pos));
                    material = mesh.materials[intersect_info.material_id];
                    glm::vec3 Kd(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
                    glm::vec3 Ks(material.specular[0], material.specular[1], material.specular[2]);
                    float_t Ns = material.shininess;
                    glm::vec3 diffuse = Kd * final_intensity * glm::max(glm::dot(-light_dir, intersect_info.normal), 0.f);
                    glm::vec3 specular = Ks * final_intensity * glm::pow(glm::max(glm::dot(-light_dir, Utils::reflect_direction(intersect_info.normal, ray.direction)), 0.f), Ns);
                    res += diffuse + specular;
                }
            }
        }
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
        fprintf(stderr, "\r%3d%c", uint32_t((float) (y + 1) / height * 100), '%');
    }
    return buffer;
}

Renderer::Renderer(Mesh &mesh, KDTree &tree, vector<PointLight> &lights, Camera &camera, int width, int height) : mesh(mesh), tree(tree), lights(lights), camera(camera), width(width),
                                                                                                                  height(height) {}

