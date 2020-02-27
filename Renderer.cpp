//
// Created by Xiaodong Zhao on 2020/2/6.
//


#include "Renderer.h"
#include "Utils.h"
#include "NormalCoord.h"
#include "Random.h"

float_t get_sphere_pdf_inv(glm::vec3 origin, float_t r, glm::vec3 pos) {
    float_t cos_theta = glm::sqrt(1. - (r * r) / (glm::dot(origin - pos, origin - pos)));
    return (2. * M_PI * (1. - cos_theta));
}

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

glm::vec3 Renderer::sample_lights(IntersectInfo &intersect_info, Ray &ray, glm::vec3 &Kd, glm::vec3 &Ks, float_t Ns, float_t P = 0.) {
    // sample on light
    glm::vec3 light_accumulate(0);
    for (Light *light : lights) {
        light_accumulate += light->sample(mesh, tree, intersect_info.pos, intersect_info.normal, ray.direction, Kd, Ks, Ns, P);
    }
    return light_accumulate;

    /*
    for (PointLight &light : lights) {

        // random sample
        float_t pdf_inv = get_sphere_pdf_inv(light.pos, light.r, intersect_info.pos);

        float_t dist = glm::length(light.pos - intersect_info.pos);

        // TODO: 这个坐标系也有问题，要不要normalize
        NormalCoord light_normal_coord(glm::normalize(light.pos - intersect_info.pos));
//        NormalCoord light_normal_coord(light.pos - intersect_info.pos);
        glm::vec3 light_dir = -light_normal_coord.local_to_world(Random::sphere(light.r, dist));
        glm::vec3 light_dir_normalized = glm::normalize(light_dir);
        Ray shadow_ray(intersect_info.pos + EPSILON * -light_dir_normalized, -light_dir_normalized);
        IntersectInfo shadow_intersect_info{};
        bool shadow_intersect = tree.intersect(shadow_ray, shadow_intersect_info);
        if (mesh.materials[shadow_intersect_info.material_id].name == light.material_name) {
            // simulate spherical light
//                        float_t final_intensity = (light.intensity * light.r * light.r) / (glm::length2(intersect_info.pos - light.pos));
            glm::vec3 final_intensity = light.intensity;
//            float_t light_dist_2 = glm::length2(shadow_intersect_info.pos - intersect_info.pos);
//            glm::vec3 diffuse = Kd * final_intensity * light_dist_2 * glm::max(glm::dot(-light_dir_normalized, intersect_info.normal), 0.f);
            glm::vec3 diffuse = Kd * final_intensity * glm::max(glm::dot(-light_dir_normalized, intersect_info.normal), 0.f);
//                    glm::vec3 diffuse = glm::vec3(0);
            // Phong
//            glm::vec3 specular = Ks * final_intensity * glm::pow(glm::max(glm::dot(-light_dir, Utils::reflect_direction(intersect_info.normal, ray.direction)), 0.f), Ns);
            // Bling-Phong
//            glm::vec3 specular = Ks * final_intensity * glm::pow(glm::max(glm::dot(intersect_info.normal, glm::normalize(-ray.direction - light_dir_normalized)), 0.f), Ns);
            // specular
            glm::vec3 specular = Ks * final_intensity * (Ns + 1) * 0.5f * M_PI *
                                 glm::pow(glm::max(glm::dot(-light_dir, Utils::reflect_direction(intersect_info.normal, ray.direction)), 0.f), Ns);
//            glm::vec3 specular = Ks * final_intensity * (Ns + 1) * 0.5f * M_PI *
//                                 glm::pow(glm::max(glm::dot(intersect_info.normal, glm::normalize(-ray.direction - light_dir_normalized)), 0.f), Ns);
//            glm::vec3 specular = glm::vec3(0);

//            float_t area = M_PI * light.r * light.r * (1 - glm::dot(glm::normalize(shadow_intersect_info.pos - light.pos), glm::normalize(intersect_info.pos - light.pos)));

            light_accumulate += (diffuse + specular) * pdf_inv * M_1_PI * (1 - P); // TODO: brdf pdf?
//            light_accumulate += (diffuse + specular) * pdf_inv * (1 - P) * (1. / area);
        }
    }

//            res += light_accumulate;
    return light_accumulate;
     */
}

glm::vec3 Renderer::cast(Ray ray, int depth = 0, bool is_sample_light = true) {
    glm::vec3 res(0.);
    if (depth > MAX_DEPTH) {
        return res;
    }
    IntersectInfo intersect_info{};
    if (tree.intersect(ray, intersect_info)) {
        auto material = mesh.materials[intersect_info.material_id];
        float_t Ni = material.ior;
        glm::vec3 Ka(material.ambient[0], material.ambient[1], material.ambient[2]);
        glm::vec3 Kd(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
        glm::vec3 Ks(material.specular[0], material.specular[1], material.specular[2]);
        float_t Ns = material.shininess;
        glm::vec3 refraction(0);

        /*
        cout << material.name << "\n";
        cout << "pos:("
             << intersect_info.pos.x << "," <<
             intersect_info.pos.y << "," <<
             intersect_info.pos.z << ")\n";
        cout << "normal:("
             << intersect_info.normal.x << "," <<
             intersect_info.normal.y << "," <<
             intersect_info.normal.z << ")\n";

//        return glm::vec3(1.) * glm::dot(intersect_info.normal, -ray.direction);
         */

        float_t russian_roulette = Random::num();

        /*
        float_t max_Kd = glm::max(Kd.x, Kd.y, Kd.z);
        if (russian_roulette < max_Kd) {
            Kd /= max_Kd;
        } else {
            return is_sample_light ? Ka : glm::vec3(0);
        }
         */

        if (Ka.x > EPSILON) {
            // is light
            return is_sample_light ? Ka : glm::vec3(0);
        } else if (glm::abs(Ni - 1.) > EPSILON) {
            return glm::vec3(0, 0, 0);
            // TODO: not complete
//            std::cerr << "Not implemented!!" << std::endl;
//            exit(1);
            // transparent
            bool is_in_dir = glm::dot(ray.direction, intersect_info.normal) < 0; // from out into object
            glm::vec3 out_dir = Utils::refract_direction(intersect_info.normal, ray.direction, Ni);
            Ray refraction_ray(intersect_info.pos + EPSILON * out_dir, out_dir);
            res = cast(refraction_ray);
        } else {
            // diffuse&specular


            float_t ratio_Ks_Kd = glm::dot(Ks, glm::vec3(1)) / (glm::dot(Ks, glm::vec3(1)) + glm::dot(Kd, glm::vec3(1)));

            russian_roulette = Random::num();

            if (russian_roulette < ratio_Ks_Kd) {
                // Specular
                glm::vec3 reflect_dir = Utils::reflect_direction(intersect_info.normal, ray.direction);
                NormalCoord normal_coord(reflect_dir);
                glm::vec3 dir = normal_coord.local_to_world(Random::specular(Ns));
                Ray new_ray(intersect_info.pos + EPSILON * dir, dir);
                glm::vec3 next = cast(new_ray, depth + 1, true);
                res += 1. / ratio_Ks_Kd * (sample_lights(intersect_info, ray, Kd, Ks, Ns, ratio_Ks_Kd) + Ks * next);
            } else {
                // Diffuse
                float_t max_Kd = glm::max(Kd.x, Kd.y, Kd.z);
                /*
                russian_roulette = Random::num();
                if (russian_roulette < max_Kd) {
                    Kd /= max_Kd;
                } else {
                    return glm::vec3(0);
                }
                 */
                NormalCoord normal_coord(intersect_info.normal);
                glm::vec3 dir = normal_coord.local_to_world(Random::hemi_sphere());
                Ray new_ray(intersect_info.pos + EPSILON * dir, dir);
                glm::vec3 next = cast(new_ray, depth + 1, true);
                res += 1. / (1. - ratio_Ks_Kd) * (sample_lights(intersect_info, ray, Kd, Ks, Ns, ratio_Ks_Kd) + Kd * next);
            }


            return res;
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
    int spp = 128;
    buffer_t buffer(height, buffer_row_t(width));
#pragma omp parallel for schedule(dynamic)
    for (int y = 0; y < height; y++) {
#pragma omp parallel for schedule(dynamic)
        for (int x = 0; x < width; x++) {
//            if (y != (512 - 300) or x < 180 or x > 190)
//            if (y != (512 - 300) || x < 50 || x > 70)
//                continue;
//            std::cout << "(" << x << ", " << y << ")\n";
//            Ray ray;
//            ray.origin = glm::vec3(x, y, 1);
//            ray.direction = glm::vec3(0, 0, -1);
//            Ray cast_ray = ortho_ray_transform(ray, width, height);
//            buffer[height - y - 1][x] = cast(cast_ray);
            glm::vec3 accumulate(0);
#pragma omp parallel for schedule(dynamic)
            for (int it = 0; it < spp; it++) {
                float_t xx = x + (spp == 1 ? 0.5 : Random::num());
                float_t yy = y + (spp == 1 ? 0.5 : Random::num());
                Ray cast_ray = camera.generate_ray(xx, yy);
//            std::cout << cast_ray.direction.x << cast_ray.direction.z << std::endl;
                accumulate += glm::clamp(glm::sqrt(cast(cast_ray)));
            }

            buffer[height - y - 1][x] = accumulate / spp;
        }
        fprintf(stderr, "\r%3d%c", uint32_t((float) (y + 1) / height * 100), '%');
    }
    return buffer;
}

Renderer::~Renderer() {
    for (Light *light : lights) {
        delete (light);
    }
}

Renderer::Renderer(Mesh &mesh, KDTree &tree, vector<Light *> &lights, Camera &camera, int width, int height) : mesh(mesh), tree(tree), lights(lights), camera(camera), width(width),
                                                                                                               height(height) {}

